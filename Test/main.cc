#define DROGON_TEST_MAIN

#include <drogon/drogon.h>
#include <drogon/drogon_test.h>
#include <plugins/ConnectTester.h>
#include <plugins/UserTester.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;

DROGON_TEST(Connect) {
    WebHelper::colorOut("Start testing Connect node", WebHelper::Color::magenta);
    auto connectTester = app().getPlugin<ConnectTester>();
    uint32_t gamingPort = 62622;
    uint32_t messagePort = 62632;
    uint32_t transferPort = 62642;
    uint32_t userPort = 62651;

    WebHelper::colorOut("Wait 1 second to generate hardware info", WebHelper::Color::cyan);
    this_thread::sleep_for(std::chrono::seconds(1));

    WebHelper::colorOut("Start testing 'heartbeat'", WebHelper::Color::blue);
    REQUIRE(connectTester->heartbeat("gaming", gamingPort));
    REQUIRE(connectTester->heartbeat("message", messagePort));
    REQUIRE(connectTester->heartbeat("transfer", transferPort));
    WebHelper::colorOut("End testing 'heartbeat'", WebHelper::Color::green);

    WebHelper::colorOut("Start testing 'allocator'", WebHelper::Color::blue);
    CHECK(connectTester->allocator("gaming")["data"].asString() ==
          connectTester->toHost(gamingPort));
    CHECK(connectTester->allocator("message")["data"].asString() ==
          connectTester->toHost(messagePort));
    CHECK(connectTester->allocator("transfer")["data"][0].asString() ==
          connectTester->toHost(transferPort));
    CHECK(connectTester->allocator("user")["data"].asString() ==
          connectTester->toHost(userPort));
    CHECK(connectTester->allocator("connect")["code"] == 402);
    CHECK(connectTester->allocator("all")["code"] == 402);
    WebHelper::colorOut("End testing 'allocator'", WebHelper::Color::green);

    WebHelper::colorOut("Start testing 'Monitor'", WebHelper::Color::blue);
    CHECK(connectTester->monitor("connect")["data"]["cpu"].isDouble());
    CHECK(connectTester->monitor("gaming")["data"][0]["info"]["cpu"].isDouble());
    CHECK(connectTester->monitor("message")["data"][0]["info"]["cpu"].isDouble());
    CHECK(connectTester->monitor("transfer")["data"][0]["info"]["cpu"].isDouble());
    CHECK(connectTester->monitor("user")["data"][0]["info"]["cpu"].isDouble());
    WebHelper::colorOut("End testing 'Monitor'", WebHelper::Color::green);

    WebHelper::colorOut("End testing Connect node", WebHelper::Color::magenta);
    WebHelper::colorOut("");
}

DROGON_TEST(User) {
    WebHelper::colorOut("Start testing User node", WebHelper::Color::magenta);
    auto userTester = app().getPlugin<UserTester>();

    WebHelper::colorOut("Start testing 'auth'", WebHelper::Color::blue);
    REQUIRE(userTester->verifyCode());
    REQUIRE(userTester->loginCode());
    REQUIRE(userTester->verifyCode());
    REQUIRE(userTester->resetPassword());
    REQUIRE(userTester->loginPassword());
    REQUIRE(userTester->verifyCode());
    REQUIRE(userTester->migrateEmail());
    REQUIRE(userTester->loginPassword());
    REQUIRE(userTester->checkToken());
    REQUIRE(userTester->refreshToken());
    REQUIRE(userTester->checkToken());
    WebHelper::colorOut("End testing 'auth'", WebHelper::Color::green);

    WebHelper::colorOut("Start testing 'user'", WebHelper::Color::blue);
    REQUIRE(userTester->updateInfo());
    REQUIRE(userTester->resetInfo());
    REQUIRE(userTester->updateInfo());
    CHECK(userTester->getAvatar());
    REQUIRE(userTester->updateFullData());
    CHECK(userTester->updatePartialData());
    CHECK(userTester->updateSkipData());
    CHECK(userTester->updateOverwriteData());
    REQUIRE(userTester->updateFullData());
    WebHelper::colorOut("End testing 'user'", WebHelper::Color::green);

    WebHelper::colorOut("End testing User node", WebHelper::Color::magenta);
    WebHelper::colorOut("");
}

int main(int argc, char **argv) {
    promise<void> loopStart;
    auto checkLoopStart = loopStart.get_future();

    /** Start the main loop on another thread */
    thread loopHandler([&]() {
        /** Queues the promise to be fulfilled after starting the loop */
        app().getLoop()->queueInLoop([&loopStart]() { loopStart.set_value(); });
        app().loadConfigFile("config.json");
        app().run();
    });

    /** The future is only satisfied after the event loop started */
    checkLoopStart.get();
    auto status = test::run(argc, argv);

    /** Ask the event loop to shutdown and wait */
    app().getLoop()->queueInLoop([]() { app().quit(); });
    loopHandler.join();

    return status;
}
