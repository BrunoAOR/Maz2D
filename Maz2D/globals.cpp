#include "globals.h"
#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>

const char* WINDOW_NAME = "Maz2D";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

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
