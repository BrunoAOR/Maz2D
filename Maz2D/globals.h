#ifndef H_GLOBALS
#define H_GLOBALS

#define LOGGER(format, ...) logger(__FILE__, __LINE__, format, __VA_ARGS__)

#include <string>


extern const char* WINDOW_NAME;
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const char* DEFAULT_VERTEX_SHADER_PATH;
extern const char* DEFAULT_FRAGMENT_SHADER_PATH;


void logger(const char file[], int line, const char* format, ...);
bool LoadTextFile(const std::string& filePath, std::string& outputString);
bool SaveTextFile(const std::string& filePath, const std::string& content);


#endif // !H_GLOBALS
