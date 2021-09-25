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

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar* message,
                const void* userParam) {
    fprintf(stderr, "  => GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}


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
    glEnable(GL_ARB_framebuffer_object);
    glfwSwapInterval(1); // enable vsync
    
    // ENABLE DEBUG OUTPUT

    //glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(MessageCallback, 0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe Render
    return 0;
}

Engine::Engine(int width, int height) { 
	this->width = width;
	this->height = height;
    this->InitOpenGL();
    util::Shader::Init();
    Component::MeshRender::lastUsedShader = 0xFFFF;
    Component::MeshRender::DepthShaderID = util::Shader::CompileShader("simpleDepthShader");
}

void Engine::MainLoop(void) { 
    SceneManager sceneManager;
    

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = util::Shader::GetDefaultShader();
    programID = util::Shader::CompileShader("gBuffer");

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
    float slowSpeed = 10.0f;
    float maxSpeed = 100.0f;
    float speed = 100.0f;
    float mouseSpeed = 0.05f;
    float deltaTime = .0f;
    int fpscount = 0;
    float currentFPS = 75.0f;

    //              ------------------------
    //              === DEFERRED SHADING ===
    //              ------------------------

    GLuint gBuffer = 0;
    glGenFramebuffers(1, &gBuffer);
    glBindBuffer(GL_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    GLuint gPosition, gNormal, gColor;
    // - position
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // - normal
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // - color
    glGenTextures(1, &gColor);
    glBindTexture(GL_TEXTURE_2D, gColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB10, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColor, 0);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    GLuint DepthRenderBuffer = 0;
    glGenRenderbuffers(1, &DepthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRenderBuffer);

    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer not complete!!!!!\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //SHADOW MAPPING
    const float SHADOW_MAP_RESOLUTION = 16384;
    GLuint ShadowFrameBuffer = 0;
    glGenFramebuffers(1, &ShadowFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, ShadowFrameBuffer);

    GLuint ShadowRenderTexture = 0;
    glGenTextures(1, &ShadowRenderTexture);
    glBindTexture(GL_TEXTURE_2D, ShadowRenderTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
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
    glUniform1i(texID, 0);

    // -- LIGHTNING --
    const uint32_t NR_LIGHTS = 127;
    std::vector<glm::vec3> lightPos;
    std::vector<glm::vec3> lightCol;

    for(uint16_t i = 0; i < NR_LIGHTS; i++) {
        const uint16_t seed = 0x4571;
        float x = Mathf::Noise1DF(i,seed) * 256+64;
        float z = Mathf::Noise1DF(i * 13,seed) * 256+64;
        float y = (Mathf::SmoothOctaveNoise2D(x * 0.03125f * 0.125f, z * 0.03125f * 0.125f, 0x154, 32, 1.214f, .855f) * 320) + 4;
        lightPos.push_back(glm::vec3(x, y, z));
        x = (Mathf::Noise1DF(i * 7, seed)+1) * .25f + .5f;
        y = (Mathf::Noise1DF(i * 7+1, seed)+1) * .25f + .5f;
        z = (Mathf::Noise1DF(i * 7+2, seed)+1) * .25f + .5f;
        lightCol.push_back(glm::vec3(x, y, z));
    }

    GLuint DeferredLightningPassID = util::Shader::CompileShader("DeferredLightningPass");
    glUseProgram(DeferredLightningPassID);
    glUniform1i(glGetUniformLocation(DeferredLightningPassID,"gPosition"), 0);
    glUniform1i(glGetUniformLocation(DeferredLightningPassID,"gNormal"), 1);
    glUniform1i(glGetUniformLocation(DeferredLightningPassID,"gColor"), 2);
    glUniform1i(glGetUniformLocation(DeferredLightningPassID,"ShadowMap"), 7);

    glUseProgram(programID);
    GLuint gBufferShadowMapLocation = glGetUniformLocation(programID, "shadowMap");
    glUniform1i(gBufferShadowMapLocation, 7);

    GLuint VolumetricScatteringPass = util::Shader::CompileShader("VolumetricScattering");
    glUseProgram(VolumetricScatteringPass);
    glUniform1i(glGetUniformLocation(VolumetricScatteringPass, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(VolumetricScatteringPass, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(VolumetricScatteringPass, "gColor"), 2);
    glUniform1i(glGetUniformLocation(VolumetricScatteringPass, "ShadowMap"), 7);

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
        Projection = glm::perspective(glm::radians(FoV), 16.0f / 9.0f, 0.001f, 1024.0f);
        // Camera matrix
        View = glm::lookAt(
            position,           // Camera is here
            position + direction, // and looks here : at the same position, plus "direction"
            up                  // Head is up (set to 0,-1,0 to look upside-down)
        );


        // Compute the MVP matrix from the light's point of view
        const float dpmScale = 384;
        glm::mat4 depthProjectionMatrix = glm::ortho<float>(-dpmScale, dpmScale, -dpmScale, dpmScale, 0, 1536);
        //glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::vec3 sunPos = position + glm::vec3(0, 192, -512);
        glm::vec3 sunViewDir = position + glm::vec3(0, 0, 0);
        sunViewDir = glm::floor(sunViewDir);
        sunViewDir.y = 0;
        sunPos = glm::floor(sunPos);
        sunPos.y = 192;
        glm::mat4 depthViewMatrix = glm::lookAt(
            sunPos,
            sunViewDir,
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

        glm::vec3 depthViewRight = glm::vec3(1,0,0);
        glm::vec3 depthViewUp = glm::vec3(depthViewMatrix[2][0], depthViewMatrix[2][1], depthViewMatrix[2][2]);
        //   -----------------
        //   === RENDERING ===
        //   -----------------
        glDepthFunc(GL_LEQUAL);

        //   -- SHADOW MAP RENDER -- 
        glBindFramebuffer(GL_FRAMEBUFFER, ShadowFrameBuffer);
        glViewport(0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(DepthShaderID);
        sceneManager.ShadowRender(depthMVP);

        //   -- MAIN CAMERA RENDER --         
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 mvp = Projection * View * Model;

        sceneManager.Update();
        sceneManager.LateUpdate();
        glUseProgram(programID);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, ShadowRenderTexture);
        sceneManager.Render(mvp);

        //   -- Deferred Lightning Pass --
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(DeferredLightningPassID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gColor);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, ShadowRenderTexture);

        for(uint16_t i = 0; i < NR_LIGHTS; i++) {
            glUniform3fv(glGetUniformLocation(DeferredLightningPassID, ("lights[" + std::to_string(i) + "].Position").c_str()), 1, &lightPos[i][0]);
            glUniform3fv(glGetUniformLocation(DeferredLightningPassID, ("lights[" + std::to_string(i) + "].Color").c_str()), 1, &lightCol[i][0]);
            const float linear = .0025f;
            const float quadratic = .005f;
            glUniform1f(glGetUniformLocation(DeferredLightningPassID, ("lights[" + std::to_string(i) + "].Linear").c_str()), linear);
            glUniform1f(glGetUniformLocation(DeferredLightningPassID, ("lights[" + std::to_string(i) + "].Quadratic").c_str()), quadratic);
        }
        glUniform3fv(glGetUniformLocation(DeferredLightningPassID, "viewPos"), 1, &position[0]);
        //glUniform3fv(glGetUniformLocation(DeferredLightningPassID, "sunViewDir"), 1, &glm::vec3(0,-.5f,1.f)[0]);
        glUniform3fv(glGetUniformLocation(DeferredLightningPassID, "sunViewDir"), 1, &depthBiasMVP[1][0]);
        //glUniform3fv(glGetUniformLocation(DeferredLightningPassID, "sunPos"), 1, &glm::vec3(0,64,128)[0]);
        //glUniform3fv(glGetUniformLocation(DeferredLightningPassID, "sunUp"), 1, &depthViewUp[0]);
        //glUniform3fv(glGetUniformLocation(DeferredLightningPassID, "sunRight"), 1, &depthViewRight[0]);
        glUniformMatrix4fv(glGetUniformLocation(DeferredLightningPassID, "ShadowMapMVP"), 1, GL_FALSE, &depthBiasMVP[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(DeferredLightningPassID, "MVP"), 1, GL_FALSE, &mvp[0][0]);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDisableVertexAttribArray(0);

        // Write the Depth Buffer into the Framebuffer for Forward Rendering
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // -- Forward Rendering Pass --

        // -- Post Processing --
        //glUseProgram(VolumetricScatteringPass);


        // -- Show the RenderTextures (DEBUG) --
        glDepthFunc(GL_ALWAYS);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);

        glUseProgram(quad_programID);
        glUniformMatrix4fv(glGetUniformLocation(quad_programID, "MVP"), 1, GL_FALSE, &glm::mat4(1.0f)[0][0]);
        glActiveTexture(GL_TEXTURE0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


        //glViewport(0, 0, 256, 256);
        //glBindTexture(GL_TEXTURE_2D, gPosition);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //glViewport(256, 0, 256, 256);
        //glBindTexture(GL_TEXTURE_2D, gNormal);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //glViewport(512, 0, 256, 256);
        //glBindTexture(GL_TEXTURE_2D, gColor);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //glViewport(768, 0, 256, 256);
        //glBindTexture(GL_TEXTURE_2D, ShadowRenderTexture);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //glDepthFunc(GL_LESS);

        //glViewport(0, 0, width, height);
        //glBindTexture(GL_TEXTURE_2D, textureID);
        //for(uint16_t i = 0; i < NR_LIGHTS; i++) {
        //    glm::mat4 xModel = glm::mat4(1.0f);
        //    xModel = glm::translate(xModel, lightPos[i]);
        //    xModel = glm::scale(xModel, glm::vec3(2.0f));
        //    glm::mat4 xMVP = Projection * View * xModel;
        //    glUniformMatrix4fv(glGetUniformLocation(quad_programID, "MVP"), 1, GL_FALSE, &xMVP[0][0]);
        //    glDrawArrays(GL_TRIANGLES, 0, 6);
        //}
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
