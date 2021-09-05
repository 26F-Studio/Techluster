#include <drogon/drogon.h>

using namespace drogon;

int main() {
    app().loadConfigFile("config.json");
    app().run();
    return 0;
}
