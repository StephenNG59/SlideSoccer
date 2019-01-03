#include "pch.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <MyClass\Shader\shader.h>
#include <MyClass\model.h>
#include <stb_image.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <MyClass/camera.h>
#include <MyClass/Object/Object3D.h>
#include <MyClass/Object/Object3Dcube.h>
#include <MyClass/Object/Object3Dcylinder.h>
#include <MyClass/Object/Object3Dsphere.h>
#include <MyClass/Object/Collision.h>
#include <MyClass/Game/Game.h>
#include <MyClass/Skybox/Skybox.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);



// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float keySensitivity = 0.25f;
float scrollSensitivity = 0.35f;
float mouseSensitivity = 0.1f;
Camera camera(glm::vec3(0.0f, 0.0f, 30.0f), glm::vec3(0, -5.0f, 0));	// (eye coord), (center coord)


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;
float collideDelta = 0.0f;
const float collideCD = 0.02f;
bool collideCDing = false;


// Game
Game myGame(SCR_WIDTH, SCR_HEIGHT);


int main()
{
	// set up window creation hints
	// ----------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		// Uncomment this if Mac OS X

	// create a window
	// ---------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Slide Soccer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// initialize glad
	// ---------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// print out version number, must be after glad loading
	std::cout << glGetString(GL_VERSION) << std::endl;


	// register callback function
	// --------------------------
	glfwSetKeyCallback(window, key_callback);


	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	// game initialization
	// -------------------
	myGame.Init();

	//Model model("resources/objects/grass/grass!.obj");

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		//processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// #NOTE 如果不传指针，在函数内修改ball的速度对函数外的ball无影响
		//CollideSph2Sph(&ball, &ball2, true);


		myGame.ProcessInput(deltaTime);
		myGame.Update(deltaTime);
		//myGame.Render(myGame.GameShader);
		myGame.RenderWithShadow();


		//model.Draw(*(myGame.GameCamera), *(myGame.GameShader));


		// glfw: swap buffers and poll IO events
		// -------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clear all allocated resources
	// ----------------------------------------------
	glfwTerminate();
	return 0;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	myGame.GameCamera->OrientLeftByDegree(-xoffset * mouseSensitivity);
	myGame.GameCamera->OrientUpByDegree(yoffset * mouseSensitivity);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.GoForward(scrollSensitivity * yoffset);
	//camera.Fov += scrollSensitivity * yoffset;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			myGame.Keys[key] = GL_TRUE;
		else if (action == GLFW_RELEASE)
			myGame.Keys[key] = GL_FALSE;
	}
}