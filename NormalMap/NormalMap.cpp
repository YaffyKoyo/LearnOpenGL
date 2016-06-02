// NormalMap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
GLFWwindow* window;

//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void computeTangentBasis(std::vector<GLfloat>& vertices_vector,
	std::vector<glm::vec3>& tangents,
	std::vector<glm::vec3>& bitangents);
GLuint loadTexture(GLchar* path);

GLuint loadCubemap(std::vector<const GLchar*> faces);

bool firstMouse = true;
const GLuint WIDTH = 1280, HEIGHT = 720;

// Camera class
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;
bool keys[1024];

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

glm::vec3 lightPos(1.2f, 1.0f, 1.0f);

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
	glDepthFunc(GL_LESS);

	// Build and compile our shader program
	Shader lampShader("../Common/Shaders/Lamp.vs", "../Common/Shaders/Lamp.fs");
	Shader normalShader("../Common/Shaders/normal_mapping.vs", "../Common/Shaders/normal_mapping.fs");
	Shader skyboxShader("../Common/Shaders/skybox.vs", "../Common/Shaders/skybox.fs");
	Shader cubemapShader("../Common/Shaders/cubemap.vs", "../Common/Shaders/cubemap.fs");


	GLuint diffuseMap = loadTexture("../Common/images/chessboard_pattern.png");
	GLuint normalMap = loadTexture("../Common/images/height_2.jpg");

	normalShader.Use();
	glUniform1i(glGetUniformLocation(normalShader.Program, "diffuseMap"), 0);
	glUniform1i(glGetUniformLocation(normalShader.Program, "normalMap"), 1);

	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	GLfloat vertices[] = {
		// Positions          // Normals           // Texture Coords
		//back
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	std::vector<GLfloat> vertices_vector( vertices,vertices+sizeof vertices/sizeof vertices[0] );

	// World space positions of our cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// Positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(1.7f,  1.2f,  1.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};


	std::vector<glm::vec3> tangents, bitangents,testVector;
	computeTangentBasis(vertices_vector, tangents, bitangents);

	for (int i=0;i<sizeof vertices/sizeof *vertices;i+=8)
	{
		testVector.push_back(glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]));
	}

	GLfloat *testArray;
	testArray = new GLfloat[testVector.size() * 3];

	for (int i=0;i<testVector.size();i+=3)
	{
		testArray[i] = testVector[i].x;
		testArray[i+1] = testVector[i].y;
		testArray[i+2] = testVector[i].z;
	}



	GLuint VBO, containerVAO, tangentBuffer, bitangentBuffer;
	glGenVertexArrays(1, &containerVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &tangentBuffer);
	glGenBuffers(1, &bitangentBuffer);

	glBindVertexArray(containerVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	
	
	glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
	glBufferData(GL_ARRAY_BUFFER, tangents.size()*sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0*sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(3);
	
	glBindBuffer(GL_ARRAY_BUFFER, bitangentBuffer);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size()*sizeof glm::vec3, &bitangents[0], GL_STATIC_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);

	// Setup skybox VAO
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);


	// Cubemap (Skybox)
	std::vector<const GLchar*> faces;	  
	faces.push_back("../Common/images/skybox/right.jpg");
	faces.push_back("../Common/images/skybox/left.jpg");
	faces.push_back("../Common/images/skybox/top.jpg");
	faces.push_back("../Common/images/skybox/bottom.jpg");
	faces.push_back("../Common/images/skybox/back.jpg");
	faces.push_back("../Common/images/skybox/front.jpg");
	GLuint cubemapTexture = loadCubemap(faces);



	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	std::vector<float> angle(10,0.0f);
	std::vector<float> rotationRate;

	srand(20);

	for (int i=0;i<10;i++)
	{
		
		rotationRate.push_back((float)std::rand() / (RAND_MAX)*6.28-3.14);
	}



	/* Loop until the user closes the window */

	do {
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Do_Movement();
		camera.Do_Movement();


		// Draw skybox first
		glDepthMask(GL_FALSE);// Remember to turn depth writing off
		skyboxShader.Use();
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);


		///////////////////////////////////////////////////////////////////////////
		//reflection
		///////////////////////////////////////////////////////////////////////////

		//cubemapShader.Use();
		//glm::mat4 modelMatrix;
		//glm::mat4 viewMatrix = camera.GetViewMatrix();
		//glm::mat4 projectionMatrix = glm::perspective(camera.Zoom, (GLfloat)WIDTH / HEIGHT, 0.1f, 100.0f);

		//GLint modelLoc = glGetUniformLocation(cubemapShader.Program, "model");
		//GLint viewLoc = glGetUniformLocation(cubemapShader.Program, "view");
		//GLint projLoc = glGetUniformLocation(cubemapShader.Program, "projection");
		//GLint campLoc = glGetUniformLocation(cubemapShader.Program, "cameraPos");

		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		//glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		//glUniform3f(campLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		//glBindVertexArray(containerVAO);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);


		//for (GLuint i = 0; i < 10; i++)
		//{
		//	modelMatrix = glm::mat4();
		//	modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
		//	//GLfloat angle = 20.0f*i;
		//	modelMatrix = glm::rotate(modelMatrix, angle[i] += rotationRate[i] * deltaTime, glm::vec3(1.0f, 0.3f, 0.5f));

		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		//	glDrawArrays(GL_TRIANGLES, 0, 36);
		//}

		////////////////////////////////////////////////////////////////////////////
		//normalMap
		////////////////////////////////////////////////////////////////////////////

		normalShader.Use();
			//creat transformations
		glm::mat4  viewMatrix, projectionMatrix;

		/*Class*/
		viewMatrix = camera.GetViewMatrix();
		projectionMatrix = glm::perspective(camera.Zoom, (GLfloat)WIDTH / HEIGHT, 0.1f, 100.0f);

		GLint modelLoc = glGetUniformLocation(normalShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(normalShader.Program, "view");
		GLint projLoc = glGetUniformLocation(normalShader.Program, "projection");

		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniform3fv(glGetUniformLocation(normalShader.Program, "lightPos"), 1, glm::value_ptr(pointLightPositions[0]));
		glUniform3fv(glGetUniformLocation(normalShader.Program, "viewPos"), 1, &camera.Position[0]);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);

		glm::mat4 modelMatrix;
		glBindVertexArray(containerVAO);

		

		for (GLuint i = 0; i < 1; i++)
		{
			modelMatrix = glm::mat4();
			modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
			//GLfloat angle = 20.0f*i;
			//modelMatrix = glm::rotate(modelMatrix, angle[i]+=rotationRate[i]*deltaTime, glm::vec3(1.0f, 0.3f, 0.5f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}



		glBindVertexArray(0);


		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(lightVAO);

		for (GLuint i = 0; i < 1; i++)
		{
			modelMatrix = glm::mat4();
			modelMatrix = glm::translate(modelMatrix, pointLightPositions[i]);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDeleteVertexArrays(1, &containerVAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}


GLuint loadTexture(GLchar* path)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}

GLuint loadCubemap(std::vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
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

void computeTangentBasis(std::vector<GLfloat>& vertices, std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents)
{
	int counting = 0;
	for (int i=0;i<vertices.size();i+=3*8)
	{
		//shortcuts of vertices
		glm::vec3 & v0 = glm::vec3(vertices[i+0],vertices[i+1],vertices[i+2]);
		glm::vec3 & v1 = glm::vec3(vertices[i+8], vertices[i+8+1], vertices[i+8+2]);
		glm::vec3 & v2 = glm::vec3(vertices[i+2*8], vertices[i + 2 * 8+1], vertices[i + 2 * 8+2]);

		//shortcuts for UVs
		glm::vec2 & uv0 = glm::vec2(vertices[i+6], vertices[i+7]);
		glm::vec2 & uv1 = glm::vec2(vertices[i+8+6], vertices[i+8+7]);
		glm::vec2 & uv2 = glm::vec2(vertices[i+2*8+6], vertices[i+2*8+7]);

		//edges of the triangle: position delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		//uv delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;
		
		//compute tangent and bitangent:
		float r = 1.0f / (deltaUV1.x*deltaUV2.y - deltaUV1.y*deltaUV2.x);
		glm::vec3 tangent = (deltaPos1*deltaUV2.y - deltaPos2*deltaUV1.y)*r;
		glm::vec3 bitangent = (deltaPos2*deltaUV1.x - deltaPos1*deltaUV2.x)*r;

		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);

		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		counting++;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


