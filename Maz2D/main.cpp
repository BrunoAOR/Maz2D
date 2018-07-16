#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

void onWindowSizeChanged(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

#define LOGGER(format, ...) Logger(__FILE__, __LINE__, format, __VA_ARGS__);
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

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.5f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

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
	LOGGER("Called: w: %i, h: %i", width, height);
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}