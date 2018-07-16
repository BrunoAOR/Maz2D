#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define LOGGER(format, ...) Logger(__FILE__, __LINE__, format, __VA_ARGS__);

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

void onWindowSizeChanged(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void Logger(const char file[], int line, const char* format, ...);

unsigned int getTriangleVao();
const char* getVertexShaderSource();
const char* getFragmentShaderSource();
unsigned int compileShader(GLenum shaderType, const char* source);
unsigned int getShaderProgramId();


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

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Maz2D", NULL, NULL);
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

	unsigned int shader = getShaderProgramId();
	unsigned int triangle = getTriangleVao();

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.5f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		glBindVertexArray(triangle);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glUseProgram(GL_NONE);

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

void Logger(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list ap;

	/* Construct the string from variable arguments */
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "%s - in %s(%i)\n", tmp_string, file, line);
	OutputDebugString(tmp_string2);
}

void onWindowSizeChanged(GLFWwindow* window, int width, int height)
{
	LOGGER("INFO: Called: w: %i, h: %i", width, height);
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

const char* getVertexShaderSource()
{
	return
		"#version 330 core\n"
		"layout (location = 0) in vec3 pos;"
		""
		"void main()"
		"{"
		"	gl_Position = vec4(pos, 1.0);"
		"}"
		"";
}


const char* getFragmentShaderSource()
{
	return
		"#version 330 core\n"
		"out vec4 outColor;"
		"void main()"
		""
		"{"
		"	outColor = vec4(0.5f, 1.0f, 0.2f, 1.0f);"
		"}"
		"";
}

unsigned int getShaderProgramId()
{
	unsigned int vertexId = compileShader(GL_VERTEX_SHADER, getVertexShaderSource());
	unsigned int fragmentId = compileShader(GL_FRAGMENT_SHADER, getFragmentShaderSource());

	unsigned int shaderProgram = 0;
	if (vertexId != 0 && fragmentId != 0)
	{
		shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexId);
		glAttachShader(shaderProgram, fragmentId);
		glLinkProgram(shaderProgram);

		int success;
		char errorLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, errorLog);
			LOGGER("ERROR: Shader program linking error: %s", errorLog);
			glDeleteProgram(shaderProgram);
			shaderProgram = 0;
		}
	}
	
	glDeleteShader(vertexId);
	glDeleteShader(fragmentId);
	return shaderProgram;
}

unsigned int compileShader(GLenum shaderType, const char* source)
{
	static const char* vertexCstr = "Vertex Shader";
	static const char* fragmentCstr = "Fragment Shader";

	unsigned int shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &source, 0);
	glCompileShader(shaderId);

	int success;
	char errorLog[512];
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		const char* shaderText = shaderType == GL_VERTEX_SHADER ? vertexCstr : (shaderType == GL_FRAGMENT_SHADER ? fragmentCstr : "Unknown shader type");
		glGetShaderInfoLog(shaderId, 512, NULL, errorLog);
		LOGGER("ERROR: Shader compilation (%s) error: %s", shaderText, errorLog);
		glDeleteShader(shaderId);
		return 0;
	}
	else
	{
		return shaderId;
	}
}
