#pragma once

#define COLOR_DEBUG "\x1b[33m"
#define COLOR_INFO "\x1b[1m"
#define COLOR_WARNING "\x1b[35m"
#define COLOR_ERROR "\x1b[31m"
#define COLOR_FATAL "\x1b[1m\x1b[31m"
#define COLOR_RESET "\x1b[0m"

typedef enum {
	LOG_DEBUG = 0,
	LOG_INFO = 1,
	LOG_WARNING = 2,
	LOG_ERROR = 3,
	LOG_FATAL = 4
} LogLevel;

void debug_start_program();

void debug_log(LogLevel level, const char* format, ...);

void debug_exit();


#ifdef DEBUG

	#define $init debug_start_program()
	#define $debug(fmt, ...) debug_log(LOG_DEBUG, fmt, ##__VA_ARGS__)
	#define $info(fmt, ...) debug_log(LOG_INFO, fmt, ##__VA_ARGS__)
	#define $warn(fmt, ...) debug_log(LOG_WARNING, fmt, ##__VA_ARGS__)
	#define $err(fmt, ...) debug_log(LOG_ERROR, fmt, ##__VA_ARGS__)
	#define $fatal(fmt, ...) debug_log(LOG_FATAL, fmt, ##__VA_ARGS__)

#else

	#define $init
	#define $debug(fmt, ...)
	#define $info(fmt, ...)
	#define $warn(fmt, ...)
	#define $err(fmt, ...)
	#define $fatal(fmt, ...)

#endif
