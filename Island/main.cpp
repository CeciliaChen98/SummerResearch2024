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
GLuint loadTexture(const char* path);

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

	vector<Texture> textures = {};
	// Shader for light cube
	Shader lightShader("light.vert", "light.frag");
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	// Create light mesh
	Mesh light(lightVerts, lightInd, textures);


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(-9.0f, 9.0f, -9.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	lightShader.use();
	lightShader.setMat4("model", lightModel);
	lightShader.setVec4("lightColor", lightColor);
	
	Shader islandShader("default.vert", "default.frag");

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
	objectModel = objectModel * rotation * scale;
	islandShader.setMat4("model", objectModel);

	
	Shader skyboxShader("skybox.vs", "skybox.fs");
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

	Shader lakeShader("lake.vs", "lake.fs");
	// Store mesh data in vectors for the lake mesh
	std::vector <Vertex> verts(lakeVertices, lakeVertices + sizeof(lakeVertices) / sizeof(Vertex));
	std::vector <GLuint> ind(lakeIndices, lakeIndices + sizeof(lakeIndices) / sizeof(GLuint));
	std::vector <Texture> tex;
	// Create lake mesh
	Mesh Lake(verts, ind, tex);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);


	// Create a depth map FBO
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	// Store the ID of the texture
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	// Set it as GL_DEPTH_COMPONENT
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Incorrectly generating framebuffer\n");
	}

	GLfloat near_plane = -10.0f, far_plane = 10.0f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(glm::vec3(-5.0f, 5.0f, -5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightProjection = lightProjection * lightView;

	Shader shadowShader("shadow.vert", "shadow.frag");
	shadowShader.use();
	shadowShader.setMat4("lightProjection", lightProjection);
	shadowShader.setMat4("model", objectModel);

	
	GLuint water_diffuse = loadTexture("Textures/water_color.jpg");
	GLuint water_specular = loadTexture("Textures/water_specular.jpg");
	GLuint grass_diffuse = loadTexture("Textures/grass/grass_diffuse.jpg");
	GLuint sand_diffuse = loadTexture("Textures/sand/sand_diffuse.jpg");
	GLuint tree_diffuse = loadTexture("Textures/tree/diffuse.jpg");
	GLuint leaf_diffuse = loadTexture("Textures/leaf/diffuse.jpg");
	GLuint stone_diffuse = loadTexture("Textures/stone/diffuse.jpg");
	GLuint wall_diffuse = loadTexture("Textures/house/wall_diffuse.jpg");
	GLuint roof_diffuse = loadTexture("Textures/house/roof_diffuse.jpg");

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

		shadowShader.use();
		island_big.Draw(shadowShader);
		island_small.Draw(shadowShader);
		leaves.Draw(shadowShader);
		tree.Draw(shadowShader);
		stone.Draw(shadowShader);
		wall.Draw(shadowShader);
		roof.Draw(shadowShader);
		bridge.Draw(shadowShader);

		// unbind frame buffer
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
		glActiveTexture(GL_TEXTURE1);
		islandShader.setInt("diffuseTexture", 1);
		glBindTexture(GL_TEXTURE_2D, sand_diffuse);
		island_big.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, grass_diffuse);
		island_small.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, leaf_diffuse);
		leaves.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tree_diffuse);
		tree.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, stone_diffuse);
		stone.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, wall_diffuse);
		wall.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, roof_diffuse);
		roof.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tree_diffuse);
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
		glActiveTexture(GL_TEXTURE1);
		islandShader.setInt("diffuseTexture", 1);
		glBindTexture(GL_TEXTURE_2D, sand_diffuse);
		island_big.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, grass_diffuse);
		island_small.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, leaf_diffuse);
		leaves.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tree_diffuse);
		tree.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, stone_diffuse);
		stone.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, wall_diffuse);
		wall.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, roof_diffuse);
		roof.Draw(islandShader);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tree_diffuse);
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
		glBindTexture(GL_TEXTURE_2D, water->id);
		glActiveTexture(GL_TEXTURE1);
		frameShader.setInt("texture_diffuse1", 1);
		glBindTexture(GL_TEXTURE_2D, water_diffuse);
		glActiveTexture(GL_TEXTURE2);
		frameShader.setInt("texture_specular1", 2);
		glBindTexture(GL_TEXTURE_2D, water_specular);

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

GLuint loadTexture(const char* path)
{
	// Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int widthImg, heightImg, numColCh;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);
	// Reads the image from a file and stores it in bytes
	unsigned char* bytes = stbi_load(path, &widthImg, &heightImg, &numColCh, 0);

	// Generates an OpenGL texture object
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Check what type of color channels the texture has and load it accordingly
	if (numColCh == 4)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else if (numColCh == 3)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else if (numColCh == 1)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			bytes
		);
	// Generates MipMaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;

}