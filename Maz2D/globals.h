#ifndef H_GLOBALS
#define H_GLOBALS

#define LOGGER(format, ...) logger(__FILE__, __LINE__, format, __VA_ARGS__);

extern const char* WINDOW_NAME;
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

void logger(const char file[], int line, const char* format, ...);


#endif // !H_GLOBALS
