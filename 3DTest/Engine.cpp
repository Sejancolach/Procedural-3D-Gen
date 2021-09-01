#include "Engine.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <GL/GLU.h>
#include <GL/glew.h>
#include <string>
#include <stdio.h>

#include "Mesh.hpp"
#include "MeshRender.hpp"
#include "Transform.hpp"
#include "Shader.hpp"
#include "Mathf.hpp"
#include "SceneManager.hpp"
#include "WorldGeneration.hpp"

class Transform;
class util::Shader;
class Engine;
class Mesh;
namespace Component {
    class MeshRender;
}

GLuint loadBMP(std::string imagePath);

int Engine::InitOpenGL(void) { 
    // Initialise GLFW
    glewExperimental = true; // Needed for core profile
    if(!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(width, height, "3d Test", NULL, NULL);
    if(window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental = true; // Needed in core profile
    if(glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(.1f, .1f, .4f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe Render
    return 0;
}

Engine::Engine(int width, int height) { 
	this->width = width;
	this->height = height;

    this->InitOpenGL();
}

void Engine::MainLoop(void) { 
    SceneManager sceneManager;
    util::Shader::Init();

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = util::Shader::GetDefaultShader();

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    GLuint textureID = loadBMP("./textures/Grass.bmp");
    GLuint textureID2 = loadBMP("./textures/Rock.bmp");

    GameObject WorldGeneratorObject;
    WorldGeneration* worldGen = static_cast<WorldGeneration*>(WorldGeneratorObject.AddBehaviour(new WorldGeneration));
    worldGen->shaderID = programID;
    worldGen->textureID1 = textureID;
    worldGen->textureID2 = textureID2;
    worldGen->GenerateWorld();
    WorldGeneratorObject.name = "WGO";


    glm::mat4 Projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.01f, 500.0f);
    glm::mat4 View = glm::lookAt(
        glm::vec3(4, 3, -3), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    // Model matrix : an identity matrix
    glm::mat4 Model = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around
    // position
    glm::vec3 position = glm::vec3(20, 40, 128);
    // horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
    // vertical angle : 0, look at the horizon
    float verticalAngle = -.5f;
    // Initial Field of View
    float initialFoV = 60.0f;

    float slowslowSpeed = 0.5f;
    float slowSpeed = 5.0f;
    float maxSpeed = 25.0f;
    float speed = 25.0f; // 3 units / second
    float mouseSpeed = 0.05f;
    float deltaTime = .0f;
    int fpscount = 0;
    float currentFPS = 75.0f;

    const float SHADOW_MAP_RESOLUTION = 2048;

    //SHADOW MAPPING
    GLuint ShadowFrameBuffer = 0;
    glGenFramebuffers(1, &ShadowFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, ShadowFrameBuffer);

    GLuint ShadowRenderTexture = 0;
    glGenTextures(1, &ShadowRenderTexture);
    glBindTexture(GL_TEXTURE_2D, ShadowRenderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ShadowRenderTexture, 0);
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLuint DepthShaderID = util::Shader::CompileShader("simpleDepthShader");
    //GLuint depthMatrixID = glGetUniformLocation(DepthShaderID, "depthMVP");

    //Render Quad
    static const GLfloat g_quad_vertex_buffer_data[] = {
    -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    };

    GLuint quad_vertexbuffer;
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    GLuint quad_programID = util::Shader::CompileShader("passthrough");
    GLuint texID = glGetUniformLocation(quad_programID, "renderedTexture");

    do {
        double currentTime = glfwGetTime();
        double xpos, ypos;
        glfwGetCursorPos(this->window, &xpos, &ypos);
        glfwSetCursorPos(this->window, (float)width / 2, (float)height / 2);
        // Compute new orientation
        horizontalAngle -= mouseSpeed * deltaTime * float((float)width / 2 - xpos);
        verticalAngle -= mouseSpeed * deltaTime * float((float)height / 2 - ypos);
        glm::vec3 direction(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
        );
        // Right vector
        glm::vec3 right = glm::vec3(
            sin(horizontalAngle - 3.14f / 2.0f),
            0,
            cos(horizontalAngle - 3.14f / 2.0f)
        );
        // Up vector : perpendicular to both direction and right
        glm::vec3 up = glm::cross(right, direction);

        HandleMovement(window, position, direction, deltaTime, speed, right, up);

        float FoV = initialFoV;
        // Projection matrix : 45&deg; Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        Projection = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 500.0f);
        // Camera matrix
        View = glm::lookAt(
            position,           // Camera is here
            position + direction, // and looks here : at the same position, plus "direction"
            up                  // Head is up (set to 0,-1,0 to look upside-down)
        );

        


        // Compute the MVP matrix from the light's point of view
        const float dpmScale = 128;
        glm::mat4 depthProjectionMatrix = glm::ortho<float>(-dpmScale, dpmScale, -dpmScale, dpmScale, -10, 512);
        //glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 depthViewMatrix = glm::lookAt(
            glm::vec3(40, 60, -128),
            //glm::vec3(40, 60, 128) + glm::vec3(0.00139759236, -0.479425550, -0.877581477),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
        );
        glm::mat4 depthModelMatrix = glm::mat4(1.0);
        glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

        glm::mat4 biasMatrix(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        );
        glm::mat4 depthBiasMVP = biasMatrix * depthMVP;

        glBindFramebuffer(GL_FRAMEBUFFER, ShadowFrameBuffer);
        //glDrawBuffer(GL_FRONT);
        glViewport(0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(DepthShaderID);
        sceneManager.ShadowRender(depthMVP);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mvp = Projection * View * Model;
        //Update and Render Scene Objects
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, ShadowRenderTexture);
        glUniform1i(glGetUniformLocation(programID,"shadowMap"), 8);
        //glUniformMatrix4fv(glGetUniformLocation(programID, "LightBiasMVP"), 1, GL_FALSE, &depthBiasMVP[0][0]);
        sceneManager.Update();
        sceneManager.Render(mvp, depthBiasMVP);
        //sceneManager.ShadowRender(mvp);
        sceneManager.LateUpdate();

        // render the shadowmap (DEBUG)
        glViewport(0, 0, 256, 256);
        glUseProgram(quad_programID);
        glActiveTexture(GL_TEXTURE8);
        glUniform1i(texID, 8);
        glBindTexture(GL_TEXTURE_2D, ShadowRenderTexture);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
        glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
        glDisableVertexAttribArray(0);


        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        deltaTime = float(currentTime - glfwGetTime());
        if(fpscount < 100) {
            fpscount++;
            currentFPS = (currentFPS + (1.0f / -deltaTime)) / 2.0f;
        } else {
            std::string newWindowTitle = "3D Test: FPS " + std::to_string((int)currentFPS);
            glfwSetWindowTitle(window, newWindowTitle.c_str());
            fpscount = 0;
        }

        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            speed = maxSpeed;
        else if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
            speed = slowSpeed;
        else if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            speed = slowslowSpeed;

    } // Check if the ESC key was pressed or the window was closed
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0);
}

GLFWwindow* Engine::GetWindow(void) {
    return window;
}


void Engine::HandleMovement(GLFWwindow* window, glm::vec3& position, glm::vec3& direction, float deltaTime, float speed, glm::vec3& right, glm::vec3& up) {
    // Move forward
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position -= direction * deltaTime * speed;
    }
    // Move backward
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position += direction * deltaTime * speed;
    }
    // Strafe right
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position -= right * deltaTime * speed;
    }
    // Strafe left
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position += right * deltaTime * speed;
    }
    // Move Up
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        position += up * deltaTime * speed;
    }
    // Move Down
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        position -= up * deltaTime * speed;
    }
}

GLuint loadBMP(std::string imagePath) {
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int width, height;
    unsigned int imageSize;
    unsigned char* data;

    FILE* file;
    fopen_s(&file, imagePath.c_str(), "rb");

    if(!file) {
        printf("Image %s could not be loaded!\n", imagePath.c_str());
        return 0;
    }

    if(fread(header, 1, 54, file) != 54) { //Error loading the header, might not be a bmp file
        printf("Not a correct bmp file!\n");
        return 0;
    }

    if(header[0] != 'B' or header[1] != 'M') {
        printf("Not a correct bmp file!\n");
        return 0;
    }

    // Read ints from the byte array
    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if(imageSize == 0)    imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
    if(dataPos == 0)      dataPos = 54; // The BMP header is done that way

    data = new unsigned char[imageSize];
    fread(data, 1, imageSize, file);
    fclose(file);

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureID;
}
