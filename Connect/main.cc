#include <drogon/drogon.h>
#include <filters/CheckCredential.h>
#include <filters/CheckNodeType.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;

int main() {
    app().loadConfigFile("config.json");
    app().registerFilter(make_shared<CheckCredential>());
    app().registerFilter(make_shared<CheckNodeType>());
    app().run();
    return 0;
}
