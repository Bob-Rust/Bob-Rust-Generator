#pragma once

#include <iostream>
#include <cstdarg>

int LogLevel = 100;

void v(const char* format, ...) {
	if(LogLevel < 1) return;

	va_list args;
	va_start(args, format);
	vprintf(format, args);
}

void vv(const char* format, ...) {
	if(LogLevel < 2) return;

	va_list args;
	va_start(args, format);
	printf("  ");
	vprintf(format, args);
}

void vvv(const char* format, ...) {
	if(LogLevel < 3) return;

	va_list args;
	va_start(args, format);
	printf("    ");
	vprintf(format, args);
}