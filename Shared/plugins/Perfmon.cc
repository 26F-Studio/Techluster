//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <plugins/Perfmon.h>

#if _WIN32

#include <windows.h>
#include <iphlpapi.h>

#pragma comment(lib, "IPHlpApi.lib")

#elif __APPLE__

#elif __linux__

#include <sys/sysinfo.h>
#include <sys/statvfs.h>

#elif __unix__

#endif

using namespace drogon;
using namespace std;
using namespace tech::plugins;

void Perfmon::initAndStart(const Json::Value &config) {
    if (!(
            config.isMember("perfmon") && config["perfmon"].isObject() &&
            config["perfmon"].isMember("cpuInterval") && config["perfmon"]["cpuInterval"].isUInt() &&
            config["perfmon"]["cpuInterval"].asUInt() <= 1000 &&
            config["perfmon"].isMember("taskInterval") && config["perfmon"]["taskInterval"].isDouble() &&
            config["perfmon"]["taskInterval"].asDouble() >= 5.0
    )) {
        LOG_ERROR << R"("Invalid perfmon config")";
        abort();
    } else {
        _cpuInterval = config["perfmon"]["cpuInterval"].asUInt();
        _taskInterval = config["perfmon"]["taskInterval"].asDouble();
    }

    app().getLoop()->runEvery(_taskInterval, [this] {
        updateInfo();
    });

    LOG_INFO << "Perfmon loaded.";
}

void Perfmon::shutdown() { LOG_INFO << "Perfmon shutdown."; }

double Perfmon::getTaskInterval() const { return _taskInterval; }

Json::Value Perfmon::parseInfo() const {
    Json::Value result;
    result["cpu"] = _cpuLoad.load();
    result["memory"]["virtual"]["available"] = _vMemAvail.load();
    result["memory"]["virtual"]["total"] = _vMemTotal.load();
    result["memory"]["physical"]["available"] = _rMemAvail.load();
    result["memory"]["physical"]["total"] = _rMemTotal.load();
    result["network"]["downstream"] = _netDown.load();
    result["disk"]["user"] = _diskUser.load();
    result["disk"]["available"] = _diskAvail.load();
    result["disk"]["total"] = _diskTotal.load();
    result["network"]["upstream"] = _netUp.load();
    result["network"]["connections"] = _netConn.load();

    return result;
}

void Perfmon::updateInfo() {
#if _WIN32
    vector<thread> threads;
    threads.emplace_back([this]() {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);

        _vMemTotal = memInfo.ullTotalPageFile;
        _vMemAvail = memInfo.ullAvailPageFile;
        _rMemTotal = memInfo.ullTotalPhys;
        _rMemAvail = memInfo.ullAvailPhys;
    });
    threads.emplace_back([this]() {
        auto getLoad = [](
                uint64_t &previousTotalTicks,
                uint64_t &previousIdleTicks
        ) -> double {
            FILETIME idleTime, kernelTime, userTime;
            if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
                auto fileTimeToInt64 = [](const FILETIME &ft) -> uint64_t {
                    return (static_cast<uint64_t>(ft.dwHighDateTime) << 32) | static_cast<uint64_t>(ft.dwLowDateTime);
                };
                auto idleTicks = fileTimeToInt64(idleTime), totalTicks = fileTimeToInt64(kernelTime) + fileTimeToInt64(userTime);

                uint64_t totalTicksSinceLastTime = totalTicks - previousTotalTicks,
                        idleTicksSinceLastTime = idleTicks - previousIdleTicks;

                previousTotalTicks = totalTicks;
                previousIdleTicks = idleTicks;

                double result = 1;
                if (totalTicksSinceLastTime > 0) {
                    result -= static_cast<double>(idleTicksSinceLastTime) / static_cast<double>(totalTicksSinceLastTime);
                }
                return result;
            } else {
                return -1.0;
            }
        };

        uint64_t previousTotalTicks{}, previousIdleTicks{};
        uint32_t count{};
        double cumulateLoad{};
        for (uint32_t timer = 0; timer < 1000; timer += _cpuInterval) {
            cumulateLoad += getLoad(previousTotalTicks, previousIdleTicks);
            this_thread::sleep_for(chrono::milliseconds(_cpuInterval));
            count++;
        }
        _cpuLoad = cumulateLoad / count;
    });
    threads.emplace_back([this]() {
        char buffer[MAX_PATH] = {0};
        GetModuleFileName(nullptr, buffer, MAX_PATH);
        auto nowPartition = string(buffer).substr(0, 3);

        int DSLength = GetLogicalDriveStrings(0, nullptr);

        char *DStr = new char[DSLength];
        GetLogicalDriveStrings(DSLength, (LPTSTR) DStr);

        uint64_t i64FreeBytesToCaller,
                i64TotalBytes,
                i64FreeBytes;

        for (int i = 0; i < DSLength / 4; ++i) {
            string partition = string(1, DStr[i * 4]) + R"(:\)";
            if (partition == nowPartition) {
                if (GetDiskFreeSpaceEx(
                        partition.c_str(),
                        reinterpret_cast<PULARGE_INTEGER>(&i64FreeBytesToCaller),
                        reinterpret_cast<PULARGE_INTEGER>(&i64TotalBytes),
                        reinterpret_cast<PULARGE_INTEGER>(&i64FreeBytes)
                )) {
                    _diskUser = i64FreeBytesToCaller;
                    _diskAvail = i64FreeBytes;
                    _diskTotal = i64TotalBytes;
                } else {
                    LOG_WARN << "Partition " << partition << " is not available now";
                }
                break;
            }
        }
    });
    threads.emplace_back([this]() {
        auto getNetworkTraffic = [](atomic<int64_t> &netDown, atomic<int64_t> &netUp) -> void {
            auto getSysNetworkFlow = [](
                    unsigned long &bitTotalReceive,
                    unsigned long &bitTotalSend
            ) -> bool {
                unsigned long dwBufferLen = 0;
                GetIfTable(nullptr, &dwBufferLen, 0);
                unique_ptr<MIB_IFTABLE> pMibIfTable((MIB_IFTABLE *) malloc(dwBufferLen));
                unsigned long dwRet = GetIfTable(pMibIfTable.get(), &dwBufferLen, 0);
                if (dwRet) {
                    LOG_ERROR << "Get ifTable failed, code: " << dwRet;
                    return false;
                }
                for (int i = 0; i != pMibIfTable->dwNumEntries; ++i) {
                    if (pMibIfTable->table[i].dwType == 6 ||
                        pMibIfTable->table[i].dwType == 71) {
                        bitTotalReceive += pMibIfTable->table[i].dwInOctets;
                        bitTotalSend += pMibIfTable->table[i].dwOutOctets;
                    }
                }
                bitTotalReceive *= 8;
                bitTotalSend *= 8;
                return true;
            };

            unsigned long previousReceive{},
                    previousSend{},
                    nowReceive{},
                    nowSend{};
            if (!getSysNetworkFlow(previousReceive, previousSend)) {
                netDown = netUp = -1;
            }
            this_thread::sleep_for(chrono::seconds(1));
            if (!getSysNetworkFlow(nowReceive, nowSend)) {
                netDown = netUp = -1;
            }

            netDown = nowReceive - previousReceive;
            netUp = nowSend - previousSend;
        };
        getNetworkTraffic(_netDown, _netUp);
    });
    threads.emplace_back([this]() {
        char cmd[] = R"(cmd /c netstat -ano | find /i "127.0.0.1" /v | find /i "ESTABLISHED" /c)";
        string strResult;
        HANDLE hPipeRead, hPipeWrite;

        SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES)};
        saAttr.bInheritHandle = TRUE; // Pipe handles are inherited by child process.
        saAttr.lpSecurityDescriptor = nullptr;

        // Create a pipe to get results from child's stdout.
        if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0)) {
            LOG_ERROR << "Create pipe failed";
            return;
        }

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        si.hStdOutput = hPipeWrite;
        si.hStdError = hPipeWrite;
        si.wShowWindow = SW_HIDE;

        BOOL fSuccess = CreateProcess(
                nullptr,
                cmd,
                nullptr,
                nullptr,
                TRUE,
                CREATE_NEW_CONSOLE,
                nullptr,
                nullptr,
                &si,
                &pi
        );
        if (!fSuccess) {
            LOG_ERROR << "Create process failed";
            CloseHandle(hPipeWrite);
            CloseHandle(hPipeRead);
            return;
        }

        bool bProcessEnded = false;
        for (; !bProcessEnded;) {
            // Give some timeSlice (50 ms), so we won't waste 100% CPU.
            bProcessEnded = WaitForSingleObject(pi.hProcess, 50) == WAIT_OBJECT_0;

            for (;;) {
                char buf[1024];
                DWORD dwRead = 0;
                DWORD dwAvail = 0;

                if (!::PeekNamedPipe(hPipeRead, nullptr, 0, nullptr, &dwAvail, nullptr))
                    break;

                if (!dwAvail)
                    break;

                if (!::ReadFile(hPipeRead, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, nullptr) || !dwRead)
                    break;

                buf[dwRead] = 0;
                strResult += buf;
            }
        } //for

        CloseHandle(hPipeWrite);
        CloseHandle(hPipeRead);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        _netConn = strtoull(strResult.c_str(), nullptr, 10);
    });

    for (auto &tempThread: threads) {
        tempThread.detach();
    }
#elif __APPLE__

#elif __linux__
    vector<thread> threads;
    threads.emplace_back([this]() {
        struct sysinfo memInfo{};

        sysinfo (&memInfo);
        uint64_t vMemTotal = memInfo.totalram,
                vMemAvail = memInfo.freeram;

        _vMemTotal = (vMemTotal + memInfo.totalswap) * memInfo.mem_unit;
        _vMemAvail = (vMemAvail + memInfo.freeswap) * memInfo.mem_unit;
        _rMemTotal = vMemTotal * memInfo.mem_unit;
        _rMemAvail = vMemAvail * memInfo.mem_unit;
    });
    threads.emplace_back([this]() {
        auto getLoad = [](
                uint64_t &lastTotalUser,
                uint64_t &lastTotalUserLow,
                uint64_t &lastTotalSys,
                uint64_t &lastTotalIdle
        ) -> double {
            double percent;
            FILE *file;
            string totalUserStr, totalUserLowStr, totalSysStr, totalIdleStr;
            totalUserStr.resize(32);
            totalUserLowStr.resize(32);
            totalSysStr.resize(32);
            totalIdleStr.resize(32);

            file = fopen("/proc/stat", "r");
            fscanf(file, "cpu %s %s %s %s", &totalUserStr[0], &totalUserLowStr[0], &totalSysStr[0], &totalIdleStr[0]);
            fclose(file);

            uint64_t totalUser = strtoull(totalUserStr.c_str(), nullptr, 10),
                    totalUserLow = strtoull(totalUserLowStr.c_str(), nullptr, 10),
                    totalSys = strtoull(totalSysStr.c_str(), nullptr, 10),
                    totalIdle = strtoull(totalIdleStr.c_str(), nullptr, 10),
                    total;

            if (totalUser < lastTotalUser ||
                totalUserLow < lastTotalUserLow ||
                totalSys < lastTotalSys || totalIdle < lastTotalIdle) {
                percent = -1.0;
            } else {
                total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
                        (totalSys - lastTotalSys);
                percent = static_cast<double>(total);
                total += (totalIdle - lastTotalIdle);
                percent /= static_cast<double>(total);
            }

            lastTotalUser = totalUser;
            lastTotalUserLow = totalUserLow;
            lastTotalSys = totalSys;
            lastTotalIdle = totalIdle;

            return percent;
        };

        uint64_t lastTotalUser{},
                lastTotalUserLow{},
                lastTotalSys{},
                lastTotalIdle{};

        uint32_t count{};
        double cumulateLoad{};
        for (uint32_t timer = 0; timer < 1000; timer += _cpuInterval) {
            cumulateLoad += getLoad(
                    lastTotalUser,
                    lastTotalUserLow,
                    lastTotalSys,
                    lastTotalIdle
            );
            this_thread::sleep_for(chrono::milliseconds(_cpuInterval));
            count++;
        }
        _cpuLoad = cumulateLoad / count;
    });
    threads.emplace_back([this]() {
        struct statvfs st{};
        statvfs("./", &st);
        _diskUser = st.f_bavail * st.f_bsize;
        _diskAvail = st.f_bfree * st.f_bsize;
        _diskTotal = st.f_blocks * st.f_bsize;
    });
    threads.emplace_back([this]() {
        auto getNetworkTraffic = [](int64_t &totalReceived, int64_t &totalSent) -> void {
            char buffer[128];
            std::string result;
            FILE *pipe = popen("cat /proc/net/dev | grep -e em -e eth -e wifi", "r");
            if (!pipe) {
                LOG_ERROR << "Get network traffic failed";
                totalReceived = totalSent = -1;
                return;
            }
            try {
                while (fgets(buffer, sizeof buffer, pipe)) {
                    result += buffer;
                }
            } catch (...) {
                pclose(pipe);
                totalReceived = totalSent = -1;
                return;
            }
            pclose(pipe);

            istringstream iss(result);
            vector<string> ret;

            copy(istream_iterator<string>(iss),
                 istream_iterator<string>(),
                 back_inserter(ret));

            for (uint32_t interface = 0; interface < ret.size(); interface += 17) {
                totalReceived += strtoll(ret[interface + 1].c_str(), nullptr, 10);
                totalSent += strtoll(ret[interface + 9].c_str(), nullptr, 10);
            }
        };
        int64_t previousReceive{},
                previousSend{},
                nowReceive{},
                nowSend{};
        getNetworkTraffic(previousReceive, previousSend);
        this_thread::sleep_for(chrono::seconds(1));
        getNetworkTraffic(nowReceive, nowSend);
        _netDown = nowReceive - previousReceive;
        _netUp = nowSend - previousSend;
    });
    threads.emplace_back([this]() {
        char buffer[128];
        std::string result;
        FILE *pipe = popen(R"(netstat -ano | grep -v "127.0.0.1" | grep -c "ESTABLISHED")", "r");
        if (!pipe) {
            LOG_ERROR << "Get network traffic failed";
            _netConn = 0;
        }
        try {
            while (fgets(buffer, sizeof buffer, pipe)) {
                result += buffer;
            }
        } catch (...) {
            pclose(pipe);
            _netConn = 0;
        }
        pclose(pipe);

        _netConn = strtoull(result.c_str(), nullptr, 10);
    });

    for (auto &tempThread: threads) {
        tempThread.detach();
    }
#elif __unix__

#endif
}