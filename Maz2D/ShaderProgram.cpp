#include "ShaderProgram.h"
#include <glad/glad.h>
#include "globals.h"


ShaderProgram::ShaderProgram()
{
}


ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(programId);
}


bool ShaderProgram::createShaderProgram(const char* vertexFilePath, const char* fragmentFilePath)
{
	bool success = true;
	std::string vertexSource;
	std::string fragmentSource;
	if (!LoadTextFile(vertexFilePath, vertexSource))
	{
		success = false;
		LOGGER("ERROR (ShaderProgram::ShaderProgram): Couldn't read vertex shader file!");
	}
	if (!LoadTextFile(fragmentFilePath, fragmentSource))
	{
		success = false;
		LOGGER("ERROR (ShaderProgram::ShaderProgram): Couldn't read fragment shader file!");
	}

	if (success)
	{
		unsigned int newProgramId = createShaderProgram(vertexSource, fragmentSource);
		if (newProgramId == 0)
		{
			success = false;
		}
		else
		{
			glDeleteProgram(programId);
			programId = newProgramId;
		}
	}

	return success;
}


unsigned int ShaderProgram::getProgramId()
{
	return programId;
}


int ShaderProgram::getUniformLocation(const char* uniformName)
{
	int location = -1;
	std::map<std::string, unsigned int>::iterator it;
	it = uniformLocations.find(uniformName);
	if (it != uniformLocations.end())
	{
		location = it->second;
	}
	else
	{
		// If not found, look for it in the shader Program and store it (if found)
		location = glGetUniformLocation(programId, uniformName);
		if (location != -1)
		{
			uniformLocations[uniformName] = location;
		}
	}
	return location;
}


void ShaderProgram::activate()
{
	glUseProgram(programId);
}


void ShaderProgram::deactivate()
{
	glUseProgram(GL_NONE);
}


unsigned int ShaderProgram::createShaderProgram(const std::string& vertexSource, const std::string& fragmentSource)
{
	unsigned int vertexId = compileShader(GL_VERTEX_SHADER, vertexSource.c_str());
	unsigned int fragmentId = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());

	unsigned int newProgramId = 0;
	if (vertexId != 0 && fragmentId != 0)
	{
		newProgramId = glCreateProgram();

		glAttachShader(newProgramId, vertexId);
		glAttachShader(newProgramId, fragmentId);
		glLinkProgram(newProgramId);

		int success;
		char errorLog[512];
		glGetProgramiv(newProgramId, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(newProgramId, 512, NULL, errorLog);
			LOGGER("ERROR (ShaderProgram): Shader program linking error: %s", errorLog);
			glDeleteProgram(newProgramId);
			newProgramId = 0;
		}
	}

	glDeleteShader(vertexId);
	glDeleteShader(fragmentId);
	return newProgramId;
}


unsigned int ShaderProgram::compileShader(GLenum shaderType, const char* source)
{
	unsigned int shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &source, 0);
	glCompileShader(shaderId);

	int success;
	char errorLog[512];
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		const char* shaderTypeName = shaderType == GL_VERTEX_SHADER ? "Vertex" : (shaderType == GL_FRAGMENT_SHADER ? "Fragment" : "Unknown shader type");
		glGetShaderInfoLog(shaderId, 512, NULL, errorLog);
		LOGGER("ERROR (ShaderProgram): Shader (%s) compilation error: %s", shaderTypeName, errorLog);
		glDeleteShader(shaderId);
		return 0;
	}
	else
	{
		return shaderId;
	}
}
