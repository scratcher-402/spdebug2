#ifndef SPDEBUG_H
#define SPDEBUG_H

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Colors

#define COLOR_DEBUG "\x1b[33m"
#define COLOR_INFO "\x1b[1m"
#define COLOR_WARNING "\x1b[35m"
#define COLOR_ERROR "\x1b[31m"
#define COLOR_FATAL "\x1b[1;31m"

#define COLOR_INDEX "\x1b[36m"
#define COLOR_VALUE ""

#define COLOR_LIB_NAME "\x1b[1;36m"

#define COLOR_SYM_NORMAL ""
#define COLOR_SYM_NULL "\x1b[1;35m"
#define COLOR_SYM_NAMED "\x1b[1;33m"
#define COLOR_SYM_SPACE "\x1b[33m"
#define COLOR_SYM_UNNAMED "\x1b[31m"

#define COLOR_RESET "\x1b[0m"

typedef enum {
	LOG_DEBUG = 0,
	LOG_INFO = 1,
	LOG_WARNING = 2,
	LOG_ERROR = 3,
	LOG_FATAL = 4
} LogLevel;

// Global vars

FILE* log_file;
int debug_saved_errno;

#define SPDEBUG_MAX_STRING_LEN 2048

// Function prototypes

void debug_start_program(const char* name);

void debug_log(LogLevel level, const char* format, ...);

void debug_exit();

void debug_print_string(const char* file_name, const char* func_name, int line, const char* string_name, const char* s);

void debug_print_int_array(const char* file_name, const char* func_name, int line, const char* arr_name, const int* arr, size_t length);

int debug_check_errno(const char* file_name, const char* func_name, int line, const char* print_reason);

void debug_hex_dump_object(uint8_t* object, size_t object_size);

#ifndef SPNDEBUG

	#define $init debug_start_program(NULL)
	#define $initp(name) debug_start_program((name))

	#ifdef SPDEBUG_LOG_LIB_NAME
		#define SPDEBUG_LIB_NAME_C COLOR_LIB_NAME "[" SPDEBUG_LOG_LIB_NAME "]" COLOR_RESET " "
		
		#define $debug(fmt, ...) debug_log(LOG_DEBUG, SPDEBUG_LIB_NAME_C fmt, ##__VA_ARGS__)
		#define $info(fmt, ...) debug_log(LOG_INFO, SPDEBUG_LIB_NAME_C fmt, ##__VA_ARGS__)
		#define $warn(fmt, ...) debug_log(LOG_WARNING, SPDEBUG_LIB_NAME_C fmt, ##__VA_ARGS__)
		#define $err(fmt, ...) debug_log(LOG_ERROR, SPDEBUG_LIB_NAME_C fmt, ##__VA_ARGS__)
		#define $fatal(fmt, ...) debug_log(LOG_FATAL, SPDEBUG_LIB_NAME_C fmt, ##__VA_ARGS__)
	#else
		#define $debug(fmt, ...) debug_log(LOG_DEBUG, fmt, ##__VA_ARGS__)
		#define $info(fmt, ...) debug_log(LOG_INFO, fmt, ##__VA_ARGS__)
		#define $warn(fmt, ...) debug_log(LOG_WARNING, fmt, ##__VA_ARGS__)
		#define $err(fmt, ...) debug_log(LOG_ERROR, fmt, ##__VA_ARGS__)
		#define $fatal(fmt, ...) debug_log(LOG_FATAL, fmt, ##__VA_ARGS__)
	#endif

	#define $vs(str) debug_print_string(__FILE__, __func__, __LINE__, #str, (str))
	#define $vd(arr, size) debug_print_int_array(__FILE__, __func__, __LINE__, #arr, (arr), size)
	
	#define $d(n) $debug("(In %s, function %s, line %d) (int) %s = %d", __FILE__, __func__, __LINE__, #n, (n))
	#define $u(n) $debug("(In %s, function %s, line %d) (unsigned int) %s = %u", __FILE__, __func__, __LINE__, #n, (n))
	#define $zu(n) $debug("(In %s, function %s, line %d) (size_t) %s = %zu", __FILE__, __func__, __LINE__, #n, (n))

	#define $xdi(item) debug_hex_dump_object((uint8_t*)&(item), sizeof(item));
	#define $xdv(arr, size) debug_hex_dump_object((uint8_t*)(arr), (size));

	#define $errno debug_check_errno(__FILE__, __func__, __LINE__, NULL)
	#define $cerrno(comment) debug_check_errno(__FILE__, __func__, __LINE__, comment)

	#define $xerrno if ($errno) exit(debug_saved_errno)
	#define $xcerrno(comment) if ($cerrno(comment)) exit(debug_saved_errno)

#else

	#define $init
	#define $initp(name)
	#define $debug(fmt, ...)
	#define $info(fmt, ...)
	#define $warn(fmt, ...)
	#define $err(fmt, ...)
	#define $fatal(fmt, ...)

	#define $vs(str)
	#define $vd(arr, size)

	#define $d(n)
	#define $u(n)
	#define $zu(n)

	#define $errno errno
	#define $cerrno(reason) errno

	#define $xerrno if (errno) exit(errno)
	#define $xcerrno(comment) if (errno) exit(errno)

#endif


#endif
