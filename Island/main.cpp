#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include"Shader.h"
#include"Camera.h"
#include"Mesh.h"
#include"Model.h"
#include"WaterFrameBuffer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int width = 800;
const unsigned int height = 600;

// camera
Camera camera(glm::vec3(1.0f, 3.0f, 1.0f));
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;



float skyboxVertices[] =
{
	//   Coordinates
	-100.0f, -100.0f,  100.0f,//        7--------6
	 100.0f, -100.0f,  100.0f,//       /|       /|
	 100.0f, -100.0f, -100.0f,//      4--------5 |
	-100.0f, -100.0f, -100.0f,//      | |      | |
	-100.0f,  100.0f,  100.0f,//      | 3------|-2
	 100.0f,  100.0f,  100.0f,//      |/       |/
	 100.0f,  100.0f, -100.0f,//      0--------1
	-100.0f,  100.0f, -100.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};



Vertex lakeVertices[] =
{
	Vertex{glm::vec3(-100.0f, 1.0f,  100.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-100.0f, 1.0f, -100.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 100.0f)},
	Vertex{glm::vec3(100.0f, 1.0f, -100.0f),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(100.0f, 100.0f)},
	Vertex{glm::vec3(100.0f, 1.0f,  100.0f),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(100.0f, 0.0f)}
};




GLuint lakeIndices[] =
{
	0, 1, 2,
	0, 2, 3
};


Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-1.0f, -1.0f,  1.0f)},
	Vertex{glm::vec3(-1.0f, -1.0f, -1.0f)},
	Vertex{glm::vec3(1.0f, -1.0f, -1.0f)},
	Vertex{glm::vec3(1.0f, -1.0f,  1.0f)},
	Vertex{glm::vec3(-1.0f,  1.0f,  1.0f)},
	Vertex{glm::vec3(-1.0f,  1.0f, -1.0f)},
	Vertex{glm::vec3(1.0f,  1.0f, -1.0f)},
	Vertex{glm::vec3(1.0f,  1.0f,  1.0f)}
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};


int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Create a GLFWwindow object of 800 by 600 pixels
	GLFWwindow* window = glfwCreateWindow(width, height, "Island", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	//Load GLAD so it configures OpenGL
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// Specify the viewport of OpenGL in the Window
	glViewport(0, 0, width, height);

		// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Generates Shader object using shaders default.vert and default.frag
	Shader skyboxShader("skybox.vs", "skybox.fs");


	vector<Texture> textures = {
	};
	// Shader for light cube
	Shader lightShader("light.vert", "light.frag");
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	// Create light mesh
	Mesh light(lightVerts, lightInd, textures);


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(98.0f, 99.0f, -98.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	lightShader.use();
	lightShader.setMat4("model", lightModel);
	lightShader.setVec4("lightColor", lightColor);
	
	Shader islandShader("color.vert", "color.frag");

	Model island_big("Models/sand.ply");
	Model island_small("Models/grass.ply");
	Model leaves("Models/leaf.ply");
	Model tree("Models/tree.ply");
	Model bridge("Models/bridge.ply");
	Model stone("Models/stone.ply");
	Model roof("Models/roof.ply");
	Model wall("Models/wall.ply");

	glm::vec4 objectColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 objectPos = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 objectModel = glm::mat4(1.0f);
	objectModel = glm::translate(objectModel, objectPos);

	islandShader.use();
	glm::mat4 scale = glm::mat4(1.0f);
	glm::vec3 sca = glm::vec3(0.2f, 0.2f, 0.2f);
	scale = glm::scale(scale, sca);
	glm::quat rotat = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotation = glm::mat4_cast(rotat);
	islandShader.setMat4("rotation", rotation);
	islandShader.setMat4("model", objectModel);
	islandShader.setMat4("scale", scale);


	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// All the faces of the cubemap (make sure they are in this exact order)
	std::string facesCubemap[6] =
	{
		"Textures/skybox/right.jpg",
		"Textures/skybox/left.jpg", 
		"Textures/skybox/top.jpg",
		"Textures/cubemap/bottom.jpg",
		"Textures/skybox/front.jpg",
		"Textures/skybox/behind.jpg"
	};

	// Creates the cubemap texture object
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	WaterFrameBuffer lakeFBO;
	Texture* water = lakeFBO.GetReflectionTexture();

	Shader frameShader("Texture.vert", "Texture.frag");
	frameShader.use();
	frameShader.setInt("frameBuffer", water->id);


	// Texture data
	Texture lakeTextures[]
	{
		{ 0, "texture_diffuse", "Textures/water_color.jpg"},
		{ 1, "texture_specular", "Textures/water_specular.jpg"},
		{ cubemapTexture, "cubemap",""}
		//Texture((parentDir + texPath + "planksSpec.png").c_str(), "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};


	Shader lakeShader("lake.vs", "lake.fs");
	// Store mesh data in vectors for the lake mesh
	std::vector <Vertex> verts(lakeVertices, lakeVertices + sizeof(lakeVertices) / sizeof(Vertex));
	std::vector <GLuint> ind(lakeIndices, lakeIndices + sizeof(lakeIndices) / sizeof(GLuint));
	std::vector <Texture> tex(lakeTextures, lakeTextures + sizeof(lakeTextures) / sizeof(Texture));
	// Create lake mesh
	Mesh Lake(verts, ind, tex);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	lakeShader.use();
	lakeShader.setInt("skybox", 1);

	// Create a depth map FBO
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	// Store the ID of the texture
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	// Set it as GL_DEPTH_COMPONENT
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	GLfloat near_plane = -10.0f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightProjection = lightProjection * lightView;

	Shader shadowShader("shadow.vert", "shadow.frag");
	shadowShader.use();
	shadowShader.setMat4("lightProjection", lightProjection);
	glm::mat4 model = objectModel * rotation * scale;
	shadowShader.setMat4("model",model);


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		glEnable(GL_DEPTH_TEST);
		// Specify the color of the background
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
		glm::vec4 planeVec = glm::vec4(0, 1, 0, -1);

		island_big.Draw(shadowShader);
		island_small.Draw(shadowShader);
		leaves.Draw(shadowShader);
		tree.Draw(shadowShader);
		stone.Draw(shadowShader);
		wall.Draw(shadowShader);
		roof.Draw(shadowShader);
		bridge.Draw(shadowShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		glViewport(0, 0, 800, 600);

		glEnable(GL_CLIP_DISTANCE0);
		// Clean the back buffer and depth buffer

		lakeFBO.BindReflectionBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glEnable(GL_DEPTH_TEST);

		// 1.0f is the height of the lake
		float distance = 2 * (camera.Position.y - 1.0f);
		camera.Position.y -= distance;
		camera.invertPitch();

		lightShader.use();
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		light.Draw(lightShader);

		islandShader.use();
		islandShader.setMat4("view", view);
		islandShader.setMat4("projection", projection);
		planeVec = glm::vec4(0, 1, 0, -1);
		islandShader.setVec4("plane", planeVec);
		islandShader.setMat4("lightProjection", lightProjection);
		islandShader.setVec3("camPos", camera.Position);
		islandShader.setVec3("lightPos", lightPos);
		glActiveTexture(GL_TEXTURE0);
		islandShader.setInt("shadowMap", 0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		objectColor = glm::vec4(0.83f, 0.62f, 0.13f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		island_big.Draw(islandShader);
		objectColor = glm::vec4(0.71f, 0.96f, 0.46f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		island_small.Draw(islandShader);
		leaves.Draw(islandShader);
		objectColor = glm::vec4(0.55f, 0.35f, 0.15f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		tree.Draw(islandShader);
		objectColor = glm::vec4(.5f, 0.4f, 0.3f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		stone.Draw(islandShader);
		objectColor = glm::vec4(0.88f, 0.79f, 0.70f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		wall.Draw(islandShader);
		objectColor = glm::vec4(0.9f, 0.35f, 0.35f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		roof.Draw(islandShader);
		objectColor = glm::vec4(.5f, 0.4f, 0.3f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		bridge.Draw(islandShader);

		
		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);

		skyboxShader.use();
		view = camera.GetViewMatrix();
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);
		
		camera.Position.y += distance;
		camera.invertPitch();


		lakeFBO.UnbindBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CLIP_DISTANCE0);

		lightShader.use();
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		light.Draw(lightShader);

		islandShader.use();
		islandShader.setMat4("view", view);
		islandShader.setMat4("projection", projection);
		planeVec = glm::vec4(0, 1, 0, -1);
		islandShader.setVec4("plane", planeVec);
		islandShader.setMat4("lightProjection", lightProjection);
		islandShader.setVec3("camPos", camera.Position);
		islandShader.setVec3("lightPos", lightPos);
		glActiveTexture(GL_TEXTURE0);
		islandShader.setInt("shadowMap", 0);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		objectColor = glm::vec4(0.83f, 0.62f, 0.13f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		island_big.Draw(islandShader);
		objectColor = glm::vec4(0.71f, 0.96f, 0.46f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		island_small.Draw(islandShader);
		leaves.Draw(islandShader);
		objectColor = glm::vec4(0.55f, 0.35f, 0.15f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		tree.Draw(islandShader);
		objectColor = glm::vec4(.5f, 0.4f, 0.3f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		stone.Draw(islandShader);
		objectColor = glm::vec4(0.88f, 0.79f, 0.70f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		wall.Draw(islandShader);
		objectColor = glm::vec4(0.9f, 0.35f, 0.35f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		roof.Draw(islandShader);
		objectColor = glm::vec4(.5f, 0.4f, 0.3f, 1.0f);
		islandShader.setVec4("objectColor", objectColor);
		bridge.Draw(islandShader);

		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);

		skyboxShader.use();
		view = camera.GetViewMatrix();
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);

		frameShader.use();

		glm::mat4 model = glm::mat4(1.0f);
		frameShader.setMat4("model", model);
		frameShader.setMat4("view", view);
		frameShader.setMat4("projection", projection);
		frameShader.setVec3("camPos", camera.Position);
		glActiveTexture(GL_TEXTURE0);
		frameShader.setInt("frameBuffer",0);
		glBindTexture(GL_TEXTURE_2D, 2);
		glActiveTexture(GL_TEXTURE1);
		frameShader.setInt("texture_diffuse1", 1);
		glBindTexture(GL_TEXTURE_2D, 5);
		glActiveTexture(GL_TEXTURE2);
		frameShader.setInt("texture_specular1", 2);
		glBindTexture(GL_TEXTURE_2D, 6);

		Lake.Draw(frameShader);


		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	//lakeFBO.~WaterFrameBuffer();

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

