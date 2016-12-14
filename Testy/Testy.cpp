// First.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
GLFWwindow* window;

//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
int main(void);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
bool firstMouse = true;
const GLuint WIDTH = 1280, HEIGHT = 720;

// Camera class
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;
bool keys[1024];


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main(void)
{
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WIDTH, HEIGHT, "First", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	//Set the required callback function
	//glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);


	// Build and compile our shader program
	Shader ourShader("../Common/Shaders/First.vs", "../Common/Shaders/First.fs");

	// Build and compile our shader program
	Shader lightingShader("../Common/Shaders/Lighting.vs", "../Common/Shaders/Lighting.fs");
	Shader lampShader("../Common/Shaders/Lamp.vs", "../Common/Shaders/Lamp.fs");


	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	// World space positions of our cubes
	

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO


						  // Load and create a texture 
	GLuint texture1;
	GLuint texture2;
	// ====================
	// Texture 1
	// ====================
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
											// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	int width, height;
	unsigned char* image = SOIL_load_image("../Common/images/chessboard_pattern.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
									 // ===================
									 // Texture 2
									 // ===================
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	image = SOIL_load_image("awesomeface.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLfloat standard_Angles[] = { 0,0,90,0,0,0 };

	GLfloat arm_Angles[] = { 45*3.14/180,0,10*3.14/180,0,0,0 };
	glm::vec3 arm_Axis[] = {
		glm::vec3(0,1,0),
		glm::vec3(1,0,0),
		glm::vec3(1,0,0),
		glm::vec3(0,1,0),
		glm::vec3(1,0,0),
		glm::vec3(0,1,0)
	};
	glm::vec3 arm_Shape[] = {
		glm::vec3(1.0,0.3,1.0),
		glm::vec3(0.8,0.8,0.8),
		glm::vec3(0.7,0.5,0.7),
		glm::vec3(0.6,1.0,0.6),
		glm::vec3(0.5,1.0,0.5),
		glm::vec3(0.4,0.4,0.4)
	};

	glm::vec3 BasePosition = glm::vec3(0, 0.15, 0);

	float counter = 0;

	/* Loop until the user closes the window */

	do {
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		counter += deltaTime;
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Do_Movement();
		camera.Do_Movement();

		//bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);

		//Active shader
		ourShader.Use();

		//creat transformations
		glm::mat4 viewMatrix, projectionMatrix;

		/*Class*/
		viewMatrix = camera.GetViewMatrix();

		projectionMatrix = glm::perspective(camera.Zoom, (GLfloat)WIDTH / HEIGHT, 0.1f, 100.0f);

		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projlLoc = glGetUniformLocation(ourShader.Program, "projection");

		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projlLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(VAO);

		//Arms Animation
		glm::mat4 modelMatrix;
		glm::mat4 base_global;
		base_global = glm::translate(glm::mat4(1), BasePosition);
		base_global = glm::rotate(glm::mat4(1), arm_Angles[0], arm_Axis[0])*base_global;
		modelMatrix = base_global*glm::scale(modelMatrix, arm_Shape[0]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glm::mat4 A2_global;
		glm::mat4 A2_Local;
		glm::mat4 A2_withScale;
		//modelMatrix1 = glm::scale(modelMatrix1, arm_Shape[1]);
		A2_Local = glm::translate(A2_Local, glm::vec3(0, 0.15, 0));
		A2_Local = glm::translate(glm::rotate(A2_Local, arm_Angles[1], arm_Axis[1]),glm::vec3(0,0.4,0));
		A2_global = base_global*A2_Local;
		A2_withScale = A2_global*glm::scale(glm::mat4(1), arm_Shape[1]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(A2_withScale));
		glDrawArrays(GL_TRIANGLES, 0, 36);


		glm::mat4 A3_global;
		glm::mat4 A3_Local;
		glm::mat4 A3_withScale;
		//modelMatrix1 = glm::scale(modelMatrix1, arm_Shape[1]);
		A3_Local = glm::translate(A3_Local, glm::vec3(0, 0.45, 0));
		A3_Local = glm::translate(glm::rotate(A3_Local, arm_Angles[2], arm_Axis[2]), glm::vec3(0, 0.25, 0));
		A3_global = A2_global*A3_Local;
		A3_withScale = A3_global*glm::scale(glm::mat4(1), arm_Shape[2]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(A3_withScale));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glm::mat4 A4_global;
		glm::mat4 A4_Local;
		glm::mat4 A4_withScale;
		//modelMatrix1 = glm::scale(modelMatrix1, arm_Shape[1]);
		A4_Local = glm::translate(A4_Local, glm::vec3(0, 0.45, 0));
		A4_Local = glm::translate(glm::rotate(A4_Local, arm_Angles[3], arm_Axis[3]), glm::vec3(0, 0.4, 0));
		A4_global = A3_global*A4_Local;
		A4_withScale = A4_global*glm::scale(glm::mat4(1), arm_Shape[3]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(A4_withScale));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glm::mat4 A5_global;
		glm::mat4 A5_Local;
		glm::mat4 A5_withScale;
		//modelMatrix1 = glm::scale(modelMatrix1, arm_Shape[1]);
		A5_Local = glm::translate(A5_Local, glm::vec3(0, 0.65, 0));
		A5_Local = glm::translate(glm::rotate(A5_Local, arm_Angles[4], arm_Axis[4]), glm::vec3(0, 0.4, 0));
		A5_global = A4_global*A5_Local;
		A5_withScale = A5_global*glm::scale(glm::mat4(1), arm_Shape[4]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(A5_withScale));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glm::mat4 A6_global;
		glm::mat4 A6_Local;
		glm::mat4 A6_withScale;
		//modelMatrix1 = glm::scale(modelMatrix1, arm_Shape[1]);
		A6_Local = glm::translate(A6_Local, glm::vec3(0, 0.15, 0));
		A6_Local = glm::translate(glm::rotate(A6_Local, arm_Angles[5], arm_Axis[5]), glm::vec3(0, 0.4, 0));
		A6_global = A5_global*A6_Local;
		A6_withScale = A6_global*glm::scale(glm::mat4(1), arm_Shape[5]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(A6_withScale));
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

/*mouse control preprocess*/

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

