#ifndef H_SHADER_PROGRAM
#define H_SHADER_PROGRAM

#include <map>
#include <string>


class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	ShaderProgram(const ShaderProgram& shaderProgram) = delete;
	ShaderProgram(ShaderProgram&& shaderProgram) = delete;
	ShaderProgram& operator= (const ShaderProgram& shaderProgram) = delete;
	ShaderProgram& operator= (ShaderProgram&& shaderProgram) = delete;

	bool createShaderProgram(const char* vertexFilePath, const char* fragmentFilePath);
	unsigned int getProgramId();
	int getUniformLocation(const char* uniformName);
	void activate();
	void deactivate();

private:
	typedef unsigned int GLenum;
	unsigned int createShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
	unsigned int compileShader(GLenum shaderType, const char* source);
	
	unsigned int programId = 0;
	std::map<std::string, unsigned int> uniformLocations;
};


#endif // !H_SHADER_PROGRAM
