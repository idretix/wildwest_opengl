//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp" //core glm functionality
#include "glm/gtc/matrix_transform.hpp" //glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp" //glm extension for computing inverse matrices
#include "glm/gtc/type_ptr.hpp" //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>
int i = 0;
int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
glm::mat4 model2;
glm::mat4 modelB;
glm::mat4 modelF;

GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

glm::vec3 lightDir2;
GLuint lightDirLoc2;
glm::vec3 lightColor2;
GLuint lightColorLoc2;

glm::vec3 lightDir3;
GLuint lightDirLoc3;
glm::vec3 lightColor3;
GLuint lightColorLoc3;
int fog = 0;
GLuint fogLoc;
int sandstorm = 0;
GLuint sandstormLoc;
int transparent = 1.0f;
GLuint transparentLoc;

gps::Camera myCamera(
	glm::vec3(9.2531f, 1.7103f, 10.652f),
	glm::vec3(8.6472f, 1.4979f, 9.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.1f;
bool pressedKeys[1024];

float angleY = 0.0f;
float angleWheel = 0.0f;
float angleTail = -30.0f;
float anglePoster = -0.164f;
float angleDoor = 32.7f;
float forward = 0.0f;
float angleBack = 0.0f;

GLfloat lightAngle;

gps::Model3D wildwest2;
gps::Model3D windmill_wheel;
gps::Model3D windmill_tail;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D lamp;
gps::Model3D portal;
gps::Model3D horse;
gps::Model3D poster;
gps::Model3D door;
gps::Model3D train;
gps::Model3D back;
gps::Model3D back2;
gps::Model3D front;
gps::Model3D front2;

gps::Shader myCustomShader;
gps::Shader myWheelShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader lightLamp;
gps::Shader portalShader;

gps::Shader skybox;
std::vector<const GLchar*> faces;
gps::SkyBox mySkyBox;

std::vector<const GLchar*> facesNight;
gps::SkyBox mySkyBoxNight;

GLuint shadowMapFBO;
GLuint depthMapTexture;
GLuint textureID;

bool showDepthMap;
bool wireframe = 0;
bool direction = 0;
bool posterD = 1;
bool doorD = 0;
bool day = 1;
bool directions[10] = { 0 };


GLenum glCheckError_(const char *file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

double px = -1.0;
double py = -1.0;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (px < 0) {
		px = xpos;
		py = ypos;
	}
	else {
		double dx = xpos - px;
		double dy = ypos - py;
		myCamera.rotate(-dy * 0.001, dx * 0.001);
		px = xpos;
		py = ypos;
	}
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;		
	}
	if (pressedKeys[GLFW_KEY_R]) {
	//	angleWheel += 0.1f;
	}
	if (pressedKeys[GLFW_KEY_F]) {
		if (fog == 0) {
			myCustomShader.useShaderProgram();
			fogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fog");
			glUniform1i(fogLoc, fog);
			fog = 1;
		}
		else {
			myCustomShader.useShaderProgram();
			fogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fog");
			glUniform1i(fogLoc, fog);
			
			fog = 0;
		}
	}
	if (pressedKeys[GLFW_KEY_T]) {
		if (sandstorm == 0) {
			myCustomShader.useShaderProgram();

			fogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "sandstorm");
			glUniform1i(fogLoc, sandstorm);
			sandstorm = 1;
		}
		else {
			myCustomShader.useShaderProgram();
			fogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "sandstorm");
			glUniform1i(fogLoc, sandstorm);

			sandstorm = 0;
		}
	}
	if (pressedKeys[GLFW_KEY_N]) {
		if (day == 0) {
			myCustomShader.useShaderProgram();
			lightColor = glm::vec3(0.1f, 0.1f, 0.1f); //white light
			lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
			glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

			lightColor2 = glm::vec3(1.5f, 1.5f, 1.0f); //white light
			lightColorLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor2");
			glUniform3fv(lightColorLoc2, 1, glm::value_ptr(lightColor2));


			faces.push_back("skybox/day/front.tga");
			faces.push_back("skybox/day/back.tga");

			faces.push_back("skybox/day/top.tga");
			faces.push_back("skybox/day/bottom.tga");

			faces.push_back("skybox/day/right.tga");
			faces.push_back("skybox/day/left.tga");

			mySkyBox.Load(faces);


			skybox.useShaderProgram();
			glUniformMatrix4fv(glGetUniformLocation(skybox.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			day = 1;
		}
		else {
			myCustomShader.useShaderProgram();
			lightColor = glm::vec3(1.3f, 1.0f, 0.9f); //white light
			lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
			glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
			lightColor2 = glm::vec3(0.0f, 0.0f, 0.0f); //white light
			lightColorLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor2");
			glUniform3fv(lightColorLoc2, 1, glm::value_ptr(lightColor2));



			facesNight.push_back("skybox/night/front.png");
			facesNight.push_back("skybox/night/back.png");

			facesNight.push_back("skybox/night/top.png");
			facesNight.push_back("skybox/night/bottom.png");

			facesNight.push_back("skybox/night/right.png");
			facesNight.push_back("skybox/night/left.png");

			mySkyBox.Load(faces);

			skybox.useShaderProgram();
			glUniformMatrix4fv(glGetUniformLocation(skybox.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			day = 0;
		}
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;		
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);	
		myCustomShader.useShaderProgram();
		model2 = glm::mat4(1.0f);
		model2 = glm::rotate(model2, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
		model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, cameraSpeed));
		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


		horse.Draw(myCustomShader);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);		
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_UP]) {
		myCamera.move(gps::MOVE_UP, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_DOWN]) {
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);
	}
	if (pressedKeys[GLFW_KEY_V]) {
		if (wireframe == 0) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireframe = 1;
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframe = 0;
		}
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
   wildwest2.LoadModel("models/wildwest/wildwest0.obj");
    windmill_wheel.LoadModel("models/windmill_wheel/windmill_wheel.obj");
	windmill_tail.LoadModel("models/windmill_tail/windmill_tail.obj");
	portal.LoadModel("models/portal/portal.obj");
	horse.LoadModel("models/horse/horse.obj");
	lamp.LoadModel("models/lamp_glass/lamp_glass.obj");

	lightCube.LoadModel("models/cube/cube.obj");

	screenQuad.LoadModel("models/quad/quad.obj");
	door.LoadModel("models/door/door.obj");
	poster.LoadModel("models/poster/poster.obj");

	train.LoadModel("models/train/train.obj");
	back.LoadModel("models/back/back.obj");
	back2.LoadModel("models/back/back2.obj");

	front.LoadModel("models/front/front.obj");
	front2.LoadModel("models/front/front2.obj");



	faces.push_back("skybox/day/front.tga");
	faces.push_back("skybox/day/back.tga");

	faces.push_back("skybox/day/top.tga");
	faces.push_back("skybox/day/bottom.tga");

	faces.push_back("skybox/day/right.tga");
	faces.push_back("skybox/day/left.tga");

	mySkyBox.Load(faces);
}

void initShaders() {
	myCustomShader.loadShader(
		"shaders/basic.vert",
		"shaders/basic.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	lightLamp.loadShader("shaders/lightLamp.vert", "shaders/lightLamp.frag");
	lightLamp.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
	depthMapShader.useShaderProgram();
	portalShader.loadShader("shaders/portal.vert", "shaders/portal.frag");
	portalShader.useShaderProgram();
	skybox.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skybox.useShaderProgram();

}

void initUniforms() {
	myCustomShader.useShaderProgram();

	transparentLoc = glGetUniformLocation(myCustomShader.shaderProgram, "transparent");
	glUniform1i(transparentLoc, transparent);


	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(9.0f, 30.0f, -11.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");	
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.3f, 1.0f, 0.9f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	myCustomShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
/////////////////////////////
		//set the light direction (direction towards the light)
	lightDir2 = glm::vec3(19.908f, 0.31856f, -6.171f);
	//lightRotation2 = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir2");
	glUniform3fv(lightDirLoc2, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view )) * lightDir2));

	//set light color
	lightColor2 = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor2");
	glUniform3fv(lightColorLoc2, 1, glm::value_ptr(lightColor2));

	//set light color
	lightColor3 = glm::vec3(1.0f, 0.0f, 1.0f); //purple light
	lightColorLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor3");
	glUniform3fv(lightColorLoc3, 1, glm::value_ptr(lightColor3));



	lightLamp.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	skybox.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(skybox.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


	//portalShader.useShaderProgram();
	//portalColor= glm::vec3(1.0f, 0.0f, 1.0f); //purple light
	//portalColorLoc = glGetUniformLocation(portalShader.shaderProgram, "portalColor");
	//glUniform3fv(portalColorLoc, 1, glm::value_ptr(portalColor));

}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	glGenFramebuffers(1, &shadowMapFBO);
	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
		0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
	//TODO - Return the light-space transformation matrix
	glm::mat4 lightView = glm::lookAt(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 5.0f, far_plane = 100;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
	return lightSpaceTrMatrix;
}

void drawObjects(gps::Shader shader, bool depthPass) {
		
	shader.useShaderProgram();
	
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	
	wildwest2.Draw(shader);
model = glm::mat4(1.0f);
glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	//model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
//model = glm::scale(model, glm::vec3(0.5f));
	//model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));

	//glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));


}

void drawWheel(gps::Shader shader, bool depthPass){

	shader.useShaderProgram();
	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(model2, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	model2 = glm::translate(model2, glm::vec3(23.915f, 5.8688f, -0.98897f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));
	

						angleWheel += 0.5f;

		model2 = glm::rotate(model2, glm::radians(angleWheel), glm::vec3(1.0f, 0.0, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

		
	model2 = glm::translate(model2, glm::vec3(-23.915f,- 5.8688f, 0.98897f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));
	


	windmill_wheel.Draw(shader);
	////////////////
	shader.useShaderProgram();
	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(model2, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	model2 = glm::translate(model2, glm::vec3(24.926f, 5.8705f, -0.9727f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

	if (direction == 0){
		angleTail += 0.25f;
		if (angleTail > 30.f)direction = 1;
	}
	else {
		angleTail -= 0.25f;
		if (angleTail <-30.f)direction = 0;
	}
	
	model2 = glm::rotate(model2, glm::radians(angleTail), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

	model2 = glm::translate(model2, glm::vec3(-24.926f, -5.8705f, 0.9727f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));
	windmill_tail.Draw(shader);


	shader.useShaderProgram();
	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(model2, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	horse.Draw(shader);
	//
	shader.useShaderProgram();
	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(model2, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	model2 = glm::translate(model2, glm::vec3(20.352f, 1.0018f, -6.4112f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

	if (posterD == 0) {
		anglePoster += 0.05f;
		if (anglePoster > 12.f)posterD = 1;
	}
	else {
		anglePoster -= 0.05f;
		if (anglePoster < -0.164f)posterD = 0;
	}

	model2 = glm::rotate(model2, glm::radians(anglePoster), glm::vec3(0.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

	model2 = glm::translate(model2, glm::vec3(-20.352f, -1.0018f, 6.4112f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));
	poster.Draw(shader);
	////
	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(model2, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	model2 = glm::translate(model2, glm::vec3(-0.78344f, 0.88192, -1.9184f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

	if (doorD == 0) {
		angleDoor += 0.1f;
		if (angleDoor > 30.f)doorD = 1;
	}
	else {
		angleDoor -= 0.1f;
		if (angleDoor < -25.f)doorD = 0;
	}

	model2 = glm::rotate(model2, glm::radians(angleDoor), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

	model2 = glm::translate(model2, glm::vec3(0.78344f, -0.88192, 1.9184f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));
	door.Draw(shader);
	///////////////////////train
	//train
	shader.useShaderProgram();
	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(model2, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));
	////////////////////////
	if(forward>=-50.0f)
	forward -= 0.005f;
	model2 = glm::translate(model2, glm::vec3(forward,0.0f,0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));
	train.Draw(shader);
////backwheel

	shader.useShaderProgram();
	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(model2, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	//
	model2 = glm::translate(model2, glm::vec3(forward, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

	model2 = glm::translate(model2, glm::vec3(28.127f, 1.3292f, -64.095f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

	if (forward >= -50.0f)
	angleBack += 0.5f;

	model2 = glm::rotate(model2, glm::radians(angleBack), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	model2 = glm::translate(model2, glm::vec3((-1.0f) * (28.127f), -1.3292f, 64.095f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	back.Draw(shader);

	shader.useShaderProgram();
	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(model2, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	//
	model2 = glm::translate(model2, glm::vec3(forward, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

	model2 = glm::translate(model2, glm::vec3(28.127f, 1.3292f, -65.897f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	//angleBack += 0.5f;

	model2 = glm::rotate(model2, glm::radians(angleBack), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	model2 = glm::translate(model2, glm::vec3((-1.0f) * (28.127), -1.3292f, 65.897f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	back2.Draw(shader);

	//
	shader.useShaderProgram();
	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(model2, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	//
	model2 = glm::translate(model2, glm::vec3( forward,0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

	model2 = glm::translate(model2, glm::vec3(24.299f ,0.93521f, -64.095f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	//angleBack += 0.5f;

	model2 = glm::rotate(model2, glm::radians(angleBack), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	model2 = glm::translate(model2, glm::vec3((-1.0f) * (24.299f ), -0.93521f, 64.095f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	front.Draw(shader);

	shader.useShaderProgram();
	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(model2, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	//
	model2 = glm::translate(model2, glm::vec3(forward, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

	model2 = glm::translate(model2, glm::vec3(24.299f, 0.93521f, -65.897f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	//angleBack += 0.5f;

	model2 = glm::rotate(model2, glm::radians(angleBack), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	model2 = glm::translate(model2, glm::vec3((-1.0f) * (24.299f), -0.93521f, 65.897f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));


	front2.Draw(shader);
}

void renderScene() {

	// depth maps creation pass
	//TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//processMovement();
	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawObjects(depthMapShader, false);
	drawWheel(depthMapShader, false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	

	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
				
		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));
		drawObjects(myCustomShader, false);
		drawWheel(myCustomShader, false);

		
	
		

		//draw a sun around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);

		//draw a lamp glass around the light

		myCustomShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model2 = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));

		//model2 = glm::translate(model2, 1.0f * lightDir);
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

		lamp.Draw(myCustomShader);
		
		//draw a portal around the light
		glEnable(GL_BLEND);
		myCustomShader.useShaderProgram();
		

		glDepthMask(false); //disable z-testing
//Render all transparent objects*

		model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		transparent = 0.5f;
		transparentLoc = glGetUniformLocation(myCustomShader.shaderProgram, "transparent");
		glUniform1i(transparentLoc, transparent);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		portal.Draw(myCustomShader);
		glDepthMask(true); //enable z-testing (for the next frame)

		

		skybox.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(skybox.shaderProgram, "view"), 1, GL_FALSE,
			glm::value_ptr(view));

		projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
		glUniformMatrix4fv(glGetUniformLocation(skybox.shaderProgram, "projection"), 1, GL_FALSE,
			glm::value_ptr(projection));

		mySkyBox.Draw(skybox, view, projection);

	}
}

void cleanup() {
	glDeleteTextures(1,& depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char * argv[]) {


	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
				//angleWheel += 1.0f;
	
		processMovement();
		renderScene();		

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}
	cleanup();

	return 0;
}