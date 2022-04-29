#include <plugins/Authorizer.h>
#include <plugins/NodeMaintainer.h>
#include <plugins/Perfmon.h>

using namespace drogon;
using namespace tech::plugins;

int main() {
    app().loadConfigFile("config.json");
    app().run();
    app().getPlugin<Authorizer>();
    app().getPlugin<NodeMaintainer>();
    app().getPlugin<Perfmon>();
    return 0;
}
