#include <drogon/drogon.h>

// TODO: Think another way to trick the linker to link these headers
#include <plugins/Authorizer.h>
#include <plugins/DataManager.h>
#include <plugins/Perfmon.h>

using namespace drogon;

int main() {
    app().setIntSignalHandler([]() {
        LOG_INFO << "SIGINT received, exiting...";
        app().getPlugin<tech::plugins::DataManager>()->shutdown();
        app().getPlugin<tech::plugins::Perfmon>()->shutdown();
        app().quit();
        exit(0);
    });
    app().loadConfigFile("config.json");
    app().run();
    return 0;
}
