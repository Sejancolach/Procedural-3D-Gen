#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Engine { 
private:
	int width, height;
	GLuint gBuffer;
	GLuint gPosition, gNormal, gColor;
	GLuint DepthRenderBuffer;
	GLuint ShadowFrameBuffer;
	GLuint ShadowRenderTexture;
	GLuint DeferredLightningPassID;
	GLuint VolumetricScatteringPass;
public:
	GLFWwindow* window;
	Engine(int width, int height);
	void HandleMovement(GLFWwindow* window, glm::vec3& position, glm::vec3& direction, float deltaTime, float speed, glm::vec3& right, glm::vec3& up);

	void MainLoop(void);

	GLFWwindow* GetWindow(void);
private:
	int InitOpenGL(void);
	void CreateGBufferRenderTextures(void);
	void CreateShadowRenderTexture(const int shadowResolution);
	void CreateRenderPasses(void);
	void DebugShowRenderTextures(void);
};

