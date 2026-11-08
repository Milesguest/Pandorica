#include "../include/pg_context.hpp"
#include "../include/pg_main.hpp"

namespace pg {

Context* cContext;

Context::Context() {
    basePath = SDL_GetBasePath();
    logFile.open(std::format("{}/{}", basePath, settings::LOGFILE_NAME));
};

Context::~Context() {
    delete window;
    delete gpu;
    logFile.close();
};

bool LinkPluginFunction(void (*pluginFunction)(), void (**newPluginLink)()) {
    if (!pluginFunction) {
        Log(1, "PluginFunction is a nullptr. Cannot link the plugin function");
        return 0;
    }

    *cContext->lastPluginLink = pluginFunction;
    cContext->lastPluginLink  = newPluginLink;

    Log(0, "Successfully linked plugin function");
    return 1;
}

} // namespace pg
