#include <filters/IdentifyUser.h>
#include <filters/VerifyGamingNode.h>
#include <plugins/Authorizer.h>
#include <plugins/NodeMaintainer.h>
#include <plugins/Perfmon.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::plugins;

int main() {
    app().loadConfigFile("config.json");
    app().registerFilter(make_shared<IdentifyUser>());
    app().registerFilter(make_shared<VerifyGamingNode>());
    app().run();
    app().getPlugin<Authorizer>();
    app().getPlugin<NodeMaintainer>();
    app().getPlugin<Perfmon>();
    return 0;
}
