#include <drogon/drogon.h>
#include <plugins/Authorizer.h>
#include <plugins/DataManager.h>
#include <plugins/Perfmon.h>

using namespace drogon;
using namespace tech::plugins;

int main() {
    app().loadConfigFile("config.json");
    app().run();
    app().getPlugin<Authorizer>();
    app().getPlugin<DataManager>();
    app().getPlugin<Perfmon>();
    return 0;
}
