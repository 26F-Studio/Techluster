#define DROGON_TEST_MAIN

#include <drogon/drogon.h>
#include <drogon/drogon_test.h>
#include <plugins/ConnectTester.h>
#include <plugins/GamingTester.h>
#include <plugins/UserTester.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;

DROGON_TEST(Connect) {
    WebHelper::colorOut("Start testing Connect node", WebHelper::Color::magenta);
    auto connectTester = app().getPlugin<ConnectTester>();
    uint32_t botPort = 62602;
    uint32_t forwardingPort = 62622;
    uint32_t gamingPort = 62632;
    uint32_t messagePort = 62642;
    uint32_t userPort = 62651;
    uint32_t workshopPort = 62661;

    WebHelper::colorOut("Wait 1 second to generate hardware info", WebHelper::Color::cyan);
    this_thread::sleep_for(std::chrono::seconds(1));

    WebHelper::colorOut("Start testing 'report'", WebHelper::Color::blue);
    REQUIRE(connectTester->report("Bot", gamingPort));
    REQUIRE(connectTester->report("Forwarding", forwardingPort));
    REQUIRE(connectTester->report("Gaming", gamingPort));
    REQUIRE(connectTester->report("Message", messagePort));
    WebHelper::colorOut("End testing 'report'", WebHelper::Color::green);

    WebHelper::colorOut("Start testing 'allocate'", WebHelper::Color::blue);
    CHECK(connectTester->allocate("All")["code"] == 402);
    CHECK(connectTester->allocate("Bot")["data"].asString() == connectTester->toHost(gamingPort));
    CHECK(connectTester->allocate("Connect")["code"] == 402);
    CHECK(connectTester->allocate("Forwarding")["data"][0].asString() == connectTester->toHost(forwardingPort));
    CHECK(connectTester->allocate("Gaming")["data"].asString() == connectTester->toHost(gamingPort));
    CHECK(connectTester->allocate("Message")["data"].asString() == connectTester->toHost(messagePort));
    CHECK(connectTester->allocate("User")["data"].asString() == connectTester->toHost(userPort));
    WebHelper::colorOut("End testing 'allocate'", WebHelper::Color::green);

    WebHelper::colorOut("Start testing 'Monitor'", WebHelper::Color::blue);
    CHECK(connectTester->monitor("Bot").retrieveByPath("data.0.info.cpu").isDouble());
    CHECK(connectTester->monitor("Connect").retrieveByPath("data.cpu").isDouble());
    CHECK(connectTester->monitor("Forwarding").retrieveByPath("data.0.info.cpu").isDouble());
    CHECK(connectTester->monitor("Gaming").retrieveByPath("data.0.info.cpu").isDouble());
    CHECK(connectTester->monitor("Message").retrieveByPath("data.0.info.cpu").isDouble());
    CHECK(connectTester->monitor("User").retrieveByPath("data.0.info.cpu").isDouble());
    WebHelper::colorOut("End testing 'Monitor'", WebHelper::Color::green);

    WebHelper::colorOut("End testing Connect node", WebHelper::Color::magenta);
    WebHelper::colorOut("");
}

DROGON_TEST(Gaming) {
    WebHelper::colorOut("Start testing Gaming node", WebHelper::Color::magenta);
    auto gamingTester = app().getPlugin<GamingTester>();

    WebHelper::colorOut("Wait for all players to connect", WebHelper::Color::cyan);
    CHECK_NOTHROW(gamingTester->connect());

    WebHelper::colorOut("Wait for all players to close", WebHelper::Color::cyan);
    gamingTester->disconnect();

    WebHelper::colorOut("End testing Gaming node", WebHelper::Color::magenta);
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
