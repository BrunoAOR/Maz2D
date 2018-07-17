#include "globals.h"
#define WIN32_LEAN_AND_MEAN
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <windows.h>


const char* WINDOW_NAME = "Maz2D";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const char* DEFAULT_VERTEX_SHADER_PATH = "assets/shaders/defaultShader.vert";
const char* DEFAULT_FRAGMENT_SHADER_PATH = "assets/shaders/defaultShader.frag";


void logger(const char file[], int line, const char* format, ...)
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


bool LoadTextFile(const std::string& filePath, std::string& outputString)
{
	bool success;
	std::ifstream inFile;

	inFile.open(filePath);
	if (inFile.good())
	{
		success = true;
		std::ostringstream sStream;
		sStream << inFile.rdbuf();
		outputString = sStream.str();
	}
	else
	{
		success = false;
		outputString = "";
		LOGGER("ERROR: Could not open a file from \"%s\"", filePath.c_str());
	}
	inFile.close();

	return success;
}


bool SaveTextFile(const std::string& filePath, const std::string& content)
{
	bool success;
	std::ofstream outFile;

	outFile.open(filePath);
	if (outFile.good())
	{
		success = true;
		outFile << content;
	}
	else
	{
		success = false;
		LOGGER("ERROR: Could not save a file to \"%s\"", filePath.c_str());
	}
	outFile.close();

	return success;
}
