#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#include <stdio.h>
#include <stdlib.h>
#include "Engine.hpp"

class Engine;

int main(int argc, char* argv[]) {
	GLint width = 1440;
	GLint height = 810;
    //width = 2560;
    //height = 1080;
    GLFWwindow* window;

    Engine engine(width, height);
    window = engine.GetWindow();
    engine.MainLoop();
    return EXIT_SUCCESS;
}
