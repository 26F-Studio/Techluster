#include <drogon/drogon.h>

// TODO: Think another way to trick the linker to link these headers
#include <filters/CheckCredential.h>
#include <filters/CheckNodeType.h>

using namespace drogon;

int main() {
    app().loadConfigFile("config.json");
    app().run();
    return 0;
}
