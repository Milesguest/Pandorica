#include "../include/pg_context.hpp"

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

} // namespace pg
