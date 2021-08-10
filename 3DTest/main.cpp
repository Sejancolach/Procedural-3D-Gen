#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include "MeshRender.hpp"
#include "Transform.hpp"
#include "Shader.hpp"
#include "Engine.hpp"

class Transform;
class util::Shader;
class Engine;

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

void HandleMovement(GLFWwindow* window, glm::vec3& position, glm::vec3& direction, float deltaTime, float speed, glm::vec3& right, glm::vec3& up);

int initGLFW(GLFWwindow*& window, const GLint& width, const GLint& height, bool& retflag);

int main(int argc, char* argv[]) {

	GLint width = 1280;
	GLint height = 720;
    GLFWwindow* window;

    Engine engine(width, height);
    window = engine.GetWindow();
    engine.MainLoop();
    return EXIT_SUCCESS;

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    static const GLfloat g_vertex_buffer_data[] = {
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
    };
    
    static const GLfloat g_color_buffer_data[] = {
    0.583f,  0.771f,  0.014f,
    0.609f,  0.115f,  0.436f,
    0.327f,  0.483f,  0.844f,
    0.822f,  0.569f,  0.201f,
    0.435f,  0.602f,  0.223f,
    0.310f,  0.747f,  0.185f,
    0.597f,  0.770f,  0.761f,
    0.559f,  0.436f,  0.730f,
    0.359f,  0.583f,  0.152f,
    0.483f,  0.596f,  0.789f,
    0.559f,  0.861f,  0.639f,
    0.195f,  0.548f,  0.859f,
    0.014f,  0.184f,  0.576f,
    0.771f,  0.328f,  0.970f,
    0.406f,  0.615f,  0.116f,
    0.676f,  0.977f,  0.133f,
    0.971f,  0.572f,  0.833f,
    0.140f,  0.616f,  0.489f,
    0.997f,  0.513f,  0.064f,
    0.945f,  0.719f,  0.592f,
    0.543f,  0.021f,  0.978f,
    0.279f,  0.317f,  0.505f,
    0.167f,  0.620f,  0.077f,
    0.347f,  0.857f,  0.137f,
    0.055f,  0.953f,  0.042f,
    0.714f,  0.505f,  0.345f,
    0.783f,  0.290f,  0.734f,
    0.722f,  0.645f,  0.174f,
    0.302f,  0.455f,  0.848f,
    0.225f,  0.587f,  0.040f,
    0.517f,  0.713f,  0.338f,
    0.053f,  0.959f,  0.120f,
    0.393f,  0.621f,  0.362f,
    0.673f,  0.211f,  0.457f,
    0.820f,  0.883f,  0.371f,
    0.982f,  0.099f,  0.879f
    };
    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    util::Shader::Init();
	GLuint programID = LoadShaders("simple.vert", "simple.frag");
    GLuint shaderID = util::Shader::GetDefaultShader();

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	glm::mat4 Projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.01f, 100.0f);
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, -3), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around
	// Get a handle for our "MVP" uniform
	// Only during the initialisation
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    Component::Transform transform;
    Component::MeshRender mRender;
    Mesh mesh;    
    std::vector<GLfloat> v(g_vertex_buffer_data, g_vertex_buffer_data + sizeof(g_vertex_buffer_data) / sizeof(g_vertex_buffer_data[0]));
    mesh.SetVertices(v);
    mRender.mesh = &mesh;
    mRender.ShaderID = programID;

    transform.SetPosition(glm::vec3(1.0f, 2.0f, 0.0f));


    // position
    glm::vec3 position = glm::vec3(0, 0, 5);
    // horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
    // vertical angle : 0, look at the horizon
    float verticalAngle = 0.0f;
    // Initial Field of View
    float initialFoV = 60.0f;

    float speed = 3.0f; // 3 units / second
    float mouseSpeed = 0.05f;
    float deltaTime = .0f;
    int fpscount = 0;
    float currentFPS = 75.0f;
    do {
        // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        double currentTime = glfwGetTime();
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glfwSetCursorPos(window, (float)width / 2, (float)height / 2);
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
        Projection = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
        // Camera matrix
        View = glm::lookAt(
            position,           // Camera is here
            position + direction, // and looks here : at the same position, plus "direction"
            up                  // Head is up (set to 0,-1,0 to look upside-down)
        );
        glm::mat4 mvp = Projection * View * Model;

        mRender.Render(mvp);

        mvp = mvp * transform.getMatrix();

        static float sinOf = 0;
        sinOf += deltaTime;
        transform.AddPosition(glm::vec3(std::sin(sinOf) * deltaTime * 10, 0, 0));

		glUseProgram(programID);
        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 12*3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
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
    } // Check if the ESC key was pressed or the window was closed
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0);
}

int initGLFW(GLFWwindow*& window, const GLint& width, const GLint& height, bool& retflag) {
    retflag = true;
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
    retflag = false;
    return { };
}

void HandleMovement(GLFWwindow* window, glm::vec3& position, glm::vec3& direction, float deltaTime, float speed, glm::vec3& right, glm::vec3& up) {
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

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	} else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		(void)getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if(InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if(InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if(InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}