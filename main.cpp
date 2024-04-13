#include <iostream>

#include "app.h"

int main()
{
    App app;
    const GLubyte *version;
    version = glGetString(GL_VERSION);
    std::cout << "version: " << version << std::endl;
    GLint maxSamples;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    std::cout << "maxsamples: " << maxSamples << std::endl;
    app.run();
    return 0;
}