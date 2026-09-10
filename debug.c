#include "debug.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>

FILE* log_file = NULL;

const char* level_tags[] = {
	"DEBUG",
	"INFO",
	"WARNING",
	"ERROR",
	"FATAL"
};

const char* console_level_tags[] = {
	COLOR_DEBUG "DEBUG" COLOR_RESET,
	COLOR_INFO "INFO" COLOR_RESET,
	COLOR_WARNING "WARNING" COLOR_RESET,
	COLOR_ERROR "ERROR" COLOR_RESET,
	COLOR_FATAL "FATAL" COLOR_RESET
};

inline void _print_tag(LogLevel level) {
	time_t now = time(NULL);
	char* str = ctime(&now);

	fprintf(stderr, "[%s %s] ", console_level_tags[level], str);

	if (log_file) fprintf(log_file, "[%s %s] ", level_tags[level], str);
	
	return;
}

inline void _print_log_text(const char* format, va_list ap) {
	vfprintf(stderr, format, ap);

	if (log_file) vfprintf(log_file, format, ap);

	return;
}

inline void _print_log_end() {
	fprintf(stderr, "\n");

	if (log_file) {
		fprintf(log_file, "\n");
		fflush(log_file);
	}

	return;
}

void debug_log(LogLevel level, const char* format, ...) {
	// Initialize argument list
	va_list ap;
	va_start(ap, format);

	_print_tag(level);
	_print_log_text(format, ap);
	_print_log_end();

	va_end(ap);
	return;
}

void debug_start_program() {
	if (!log_file) {
		log_file = fopen("debug.log", "a");
		if (!log_file) {
			$err("Failed to open log file. Error: %s (errno %d)", strerror(errno), errno);
		}
	}

	atexit(debug_exit);
}

void debug_exit() {
	if (log_file) {
		fflush(log_file);
		fclose(log_file);
	}
}
