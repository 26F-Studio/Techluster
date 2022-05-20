//
// Created by Parti on 2021/2/4.
//

#include <algorithm>
#include <drogon/drogon.h>
#include <helpers/ResponseJson.h>
#include <magic_enum.hpp>
#include <plugins/Authorizer.h>
#include <plugins/Perfmon.h>
#include <structures/Exceptions.h>

#if _WIN32

#include <iphlpapi.h>

#pragma comment(lib, "IPHlpApi.lib")

#elif __APPLE__

#elif __linux__

#include <sys/sysinfo.h>
#include <sys/statvfs.h>

#elif __unix__

#endif

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace trantor;

namespace {
#if _WIN32

    constexpr uint64_t fileTimeToInt64(const FILETIME &ft) {
        return (static_cast<uint64_t>(ft.dwHighDateTime) << 32) | static_cast<uint64_t>(ft.dwLowDateTime);
    }

    double getCpuLoad(uint64_t &previousTotalTicks, uint64_t &previousIdleTicks) {
        FILETIME idleTime, kernelTime, userTime;
        if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
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
    }

    bool getSysNetworkFlow(unsigned long &bitTotalReceive, unsigned long &bitTotalSend) {
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
    }

#endif
}

void Perfmon::initAndStart(const Json::Value &config) {
    if (!(
            config["perfmon"]["cpuInterval"].isUInt() &&
            config["perfmon"]["cpuInterval"].asUInt() <= 1000 &&
            config["perfmon"]["taskInterval"].isDouble() &&
            config["perfmon"]["taskInterval"].asDouble() >= 5.0
    )) {
        LOG_ERROR << R"("Invalid perfmon config")";
        abort();
    } else {
        _cpuInterval = config["perfmon"]["cpuInterval"].asUInt();
        _taskInterval = config["perfmon"]["taskInterval"].asDouble();
    }

    app().getLoop()->runEvery(_taskInterval, [this] {
        _updateInfo();
    });

    _updateInfo();

    if (config["report"]["address"].isString() &&
        config["report"]["localhost"].isBool() &&
        config["report"]["type"].isString() &&
        config["report"]["description"].isString()) {

        _connectAddress = config["report"]["address"].asString();
        _nodeType = config["report"]["type"].asString();

        _heartbeatBody["ip"] = config["report"]["localhost"].asBool() ? "127.0.0.1" :
                               string(HttpClient::newHttpClient(
                                       "https://api.ipify.org/"
                               )->sendRequest(
                                       HttpRequest::newHttpRequest()
                               ).second->body());
        _heartbeatBody["port"] = app().getListeners()[0].toPort();
        _heartbeatBody["taskInterval"] = _taskInterval.load();
        _heartbeatBody["description"] = config["report"]["description"].asString();

        app().getLoop()->runEvery(_taskInterval, [this]() {
            _report();
        });
    }

    LOG_INFO << "Perfmon loaded.";
}

void Perfmon::shutdown() { LOG_INFO << "Perfmon shutdown."; }

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

void Perfmon::_report() {
    _heartbeatBody["info"] = parseInfo();
    auto client = HttpClient::newHttpClient("http://" + _connectAddress);
    auto req = HttpRequest::newHttpJsonRequest(_heartbeatBody);
    req->setMethod(Post);
    req->setPath("/tech/api/v2/node/report");
    req->addHeader("x-credential", app().getPlugin<Authorizer>()->getCredential());
    req->setParameter("nodeType", _nodeType);
    client->sendRequest(req, [](ReqResult result, const HttpResponsePtr &responsePtr) {
        if (result != ReqResult::Ok) {
            LOG_ERROR << "Request failed (" << static_cast<int>(result) << ")";
            return;
        }
        try {
            if (responsePtr->statusCode() != k200OK) {
                ResponseJson responseJson(responsePtr);
                LOG_WARN << "Request failed (" << responsePtr->statusCode() << "): \n"
                         << responseJson.stringify();
            }
        } catch (const json_exception::InvalidFormat &e) {
            LOG_WARN << "Invalid response body (" << responsePtr->statusCode() << "): \n"
                     << e.what();
        }
    }, 3);
}

void Perfmon::_updateInfo() {
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
        uint64_t previousTotalTicks{}, previousIdleTicks{};
        uint32_t count{};
        double cumulateLoad{};
        for (uint32_t timer = 0; timer < 1000; timer += _cpuInterval) {
            auto load = getCpuLoad(previousTotalTicks, previousIdleTicks);
            if (load < 0) {
                continue;
            }
            cumulateLoad += load;
            this_thread::sleep_for(chrono::milliseconds(_cpuInterval));
            count++;
        }
        _cpuLoad = cumulateLoad / count;
    });
    threads.emplace_back([this]() {
        char buffer[MAX_PATH] = {0};
        GetModuleFileName(nullptr, buffer, MAX_PATH);
        auto nowPartition = string(buffer).substr(0, 3);

        int dsLength = GetLogicalDriveStrings(0, nullptr);

        char *dStr = new char[dsLength];
        GetLogicalDriveStrings(dsLength, (LPTSTR) dStr);

        uint64_t i64FreeBytesToCaller,
                i64TotalBytes,
                i64FreeBytes;

        for (int i = 0; i < dsLength / 4; ++i) {
            string partition = string(1, dStr[i * 4]) + R"(:\)";
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
        unsigned long previousReceive{}, previousSend{}, nowReceive{}, nowSend{};
        if (!getSysNetworkFlow(previousReceive, previousSend)) {
            _netDown = _netUp = -1;
        }
        this_thread::sleep_for(chrono::seconds(1));
        if (!getSysNetworkFlow(nowReceive, nowSend)) {
            _netDown = _netUp = -1;
        }
        _netDown = nowReceive - previousReceive;
        _netUp = nowSend - previousSend;
    });
    threads.emplace_back([this]() {
        DWORD dwSize{};
        GetTcpTable(nullptr, &dwSize, {});
        unique_ptr<MIB_TCPTABLE> tcpTablePtr{reinterpret_cast<MIB_TCPTABLE *>(malloc(dwSize))};

        auto result = GetTcpTable(tcpTablePtr.get(), &dwSize, true);
        if (result == NO_ERROR) {
            uint64_t counter{};
            _connections.resize(tcpTablePtr->dwNumEntries);
            for (unsigned long i = 0; i < tcpTablePtr->dwNumEntries; i++) {
                auto tcpStateOpt = enum_cast<TcpState>(static_cast<int>(tcpTablePtr->table[i].dwState));
                if (tcpStateOpt && tcpStateOpt.value() == TcpState::established) {
                    counter++;
                }
                _connections[i] = {
                        InetAddress{{
                                            AF_INET,
                                            static_cast<USHORT>(tcpTablePtr->table[i].dwLocalPort),
                                            {.S_un={.S_addr=static_cast<ULONG>(tcpTablePtr->table[i].dwLocalAddr)}}
                                    }},
                        InetAddress{{
                                            AF_INET,
                                            static_cast<USHORT>(tcpTablePtr->table[i].dwRemotePort),
                                            {.S_un={.S_addr=static_cast<ULONG>(tcpTablePtr->table[i].dwRemoteAddr)}}
                                    }},
                        tcpStateOpt ? tcpStateOpt.value() : TcpState::Reserved
                };
            }
            _netConn = counter;
        } else {
            LOG_WARN << "GetTcpTable failed with " << result;
        }
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
