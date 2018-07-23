#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "Texture.h"
#include "globals.h"


void onWindowSizeChanged(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

unsigned int getTriangleVao();
unsigned int getTopLeftQuadVao();
unsigned int getTopRightQuadVao();
unsigned int getQuadVao(float vertices[], unsigned int verticesSize);
unsigned int getBottomTriangleLine();


int main()
{
	LOGGER("INFO: Starting Maz2D");
	if (!glfwInit())
	{
		LOGGER("ERROR: Failed to initialize GLFW");
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// For Mac, the following is required:
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
	if (window == nullptr)
	{
		LOGGER("ERROR: Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, onWindowSizeChanged);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOGGER("ERROR: Failed to initialize GLAD");
		return -1;
	}

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// Setup DevIL
	Texture::InitDevIL();

	// Setup testing VAOs
	unsigned int triangle = getTriangleVao();
	unsigned int tlQuad = getTopLeftQuadVao();
	unsigned int trQuad = getTopRightQuadVao();
	unsigned int trisLine = getBottomTriangleLine();

	Texture testTexture;
	testTexture.loadTexture("assets/images/testTexture.png");
	
	ShaderProgram shaderProgram;
	shaderProgram.createShaderProgram(DEFAULT_VERTEX_SHADER_PATH, DEFAULT_FRAGMENT_SHADER_PATH);
	int shaderColorLocation = shaderProgram.getUniformLocation("inColor");
	int useTextureLocation = shaderProgram.getUniformLocation("useTexture");

	// Setup matrices
	glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT, 0.1f, 10.0f);
	//projectionMatrix = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -5.0f));
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix= glm::translate(modelMatrix, glm::vec3(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f, 1.0f));
	
	int projectionLocation = shaderProgram.getUniformLocation("projectionMatrix");
	int viewLocation = shaderProgram.getUniformLocation("viewMatrix");
	int modelLocation = shaderProgram.getUniformLocation("modelMatrix");

	shaderProgram.activate();
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	shaderProgram.deactivate();


	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.5f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.activate();
		testTexture.bind();

		float time = (float)glfwGetTime();
		float changingValue = (sin(time) / 2.0f) + 0.5f;
		glUniform4f(shaderColorLocation, 0.0f, changingValue, 0.0f, 1.0f);

		glUniform1i(useTextureLocation, true);
		glBindVertexArray(tlQuad);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(GL_NONE);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(trQuad);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(GL_NONE);

		glUniform1i(useTextureLocation, false);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(triangle);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(GL_NONE);

		glBindVertexArray(trisLine);
		glDrawElements(GL_TRIANGLES, 15, GL_UNSIGNED_INT, 0);
		glBindVertexArray(GL_NONE);

		testTexture.unbind();
		shaderProgram.deactivate();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	LOGGER("INFO: Closing Maz2D");
	return 0;
}


int CALLBACK WinMain(
	HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow
)
{
	return main();
}


void onWindowSizeChanged(GLFWwindow* window, int width, int height)
{
	LOGGER("INFO: Window resized to: w: %i, h: %i", width, height);
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}


unsigned int getTriangleVao()
{
	// Note CCW order
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,	// bottom-left
		 0.5f, -0.5f, 0.0f, // bottom-right
		 0.0f,  0.5f, 0.0f	// top-center
	};

	unsigned int vao;
	unsigned int vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	
	glBindVertexArray(GL_NONE);

	return vao;
}


unsigned int getTopLeftQuadVao()
{
	// x, y, z, u, v 
	float vertices[] = {
		-0.8f, 0.2f, 0.0f, 0.0f, 0.0f,	// bottom-left
		-0.2f, 0.2f, 0.0f, 1.0f, 0.0f,	// bottom-right
		-0.2f, 0.8f, 0.0f, 1.0f, 1.0f,	// top-right
		-0.8f, 0.8f, 0.0f, 0.0f, 1.0f	// top-left
	};

	return getQuadVao(vertices, sizeof(vertices));
}


unsigned int getTopRightQuadVao()
{
	float vertices[] = {
		0.2f, 0.2f, 0.0f, 0.0f, 0.0f,	// bottom-left
		0.8f, 0.2f, 0.0f, 1.0f, 0.0f,	// bottom-right
		0.8f, 0.8f, 0.0f, 1.0f, 1.0f,	// top-right
		0.2f, 0.8f, 0.0f, 0.0f, 1.0f	// top-left
	};

	return getQuadVao(vertices, sizeof(vertices));
}


unsigned int getQuadVao(float vertices[], unsigned int verticesSize)
{
	// Note CCW order
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	// Note that the EBO must be un-bound AFTER the VAO is unbound.
	// This is done because the VAO registers the EBO that is bound while it (the VAO) is bound
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

	return vao;
}


unsigned int getBottomTriangleLine()
{
	float vertices[] = {
		-1.0f, -1.0f, 0.0f,	// BL
		-0.6f, -1.0f, 0.0f,	// B
		-0.8f, -0.5f, 0.0f,	// T
		-0.2f, -1.0f, 0.0f,	// B
		-0.4f, -0.5f, 0.0f,	// T
		 0.2f, -1.0f, 0.0f,	// B
		 0.0f, -0.5f, 0.0f,	// T
		 0.6f, -1.0f, 0.0f,	// B
		 0.4f, -0.5f, 0.0f,	// T
		 1.0f, -1.0f, 0.0f,	// B
		 0.8f, -0.5f, 0.0f,	// T
	};

	unsigned int indices[] = {
		0, 1, 2,
		1, 3, 4,
		3, 5, 6,
		5, 7, 8,
		7, 9, 10
	};

	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	// Note that the EBO must be un-bound AFTER the VAO is unbound.
	// This is done because the VAO registers the EBO that is bound while it (the VAO) is bound
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

	return vao;
}
