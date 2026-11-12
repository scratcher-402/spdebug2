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

#define COLOR_FILE_NAME "\x1b[1;36m"
#define COLOR_FUNC_NAME "\x1b[1;34m"
#define COLOR_LINE_NUMBER "\x1b[1;33m"

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

typedef enum {
	LOG_MODE_AUTO = 0,
	LOG_MODE_RAW = 1,
	LOG_MODE_RAW_PREP = 2
} LogMode;

typedef struct {
	const char* log_file_path;
	const char* program_name;
	LogLevel log_level;
} DebugConfig_t;

// Global vars

extern DebugConfig_t debug_config;

extern FILE* log_file;
extern int debug_saved_errno;
extern LogMode debug_log_mode;

#define SPDEBUG_MAX_STRING_LEN 2048

// Function prototypes

void debug_start_program(const char* name);

void debug_log(LogLevel level, const char* format, ...);

void debug_exit();

void debug_print_string(const char* file_name, const char* func_name, int line, const char* string_name, const char* s);

void debug_print_int_array(const char* file_name, const char* func_name, int line, const char* arr_name, const int* arr, size_t length);

void debug_print_number_array(void* arr, const char* arr_name, size_t arr_size, size_t arr_elem_size, void(*print_func)(const void*, int), const char* file_name, const char* func_name, int line_number);

void debug_print_int(const void* number, int width);
void debug_print_uint(const void* number, int width);
void debug_print_sizet(const void* number, int width);
void debug_print_char(const void* number, int width);

int debug_check_errno(const char* file_name, const char* func_name, int line, const char* print_reason);

void debug_hex_dump_object(uint8_t* object, size_t object_size, const char* object_name);

void debug_start_raw();

void debug_end_raw();

#ifndef SPNDEBUG

	#define $init debug_start_program(NULL)
	#define $initp(name) debug_start_program((name))
	
	#define $debug(fmt, ...) debug_log(LOG_DEBUG, fmt, ##__VA_ARGS__)
	#define $info(fmt, ...) debug_log(LOG_INFO, fmt, ##__VA_ARGS__)
	#define $warn(fmt, ...) debug_log(LOG_WARNING, fmt, ##__VA_ARGS__)
	#define $err(fmt, ...) debug_log(LOG_ERROR, fmt, ##__VA_ARGS__)
	#define $fatal(fmt, ...) debug_log(LOG_FATAL, fmt, ##__VA_ARGS__)

	#define $raw debug_start_raw()
	#define $endraw debug_end_raw()

	#define $vs(str) debug_print_string(__FILE__, __func__, __LINE__, #str, (str))
	#define $vd(arr, size) debug_print_number_array((arr), #arr, size, sizeof(int), debug_print_int, __FILE__, __func__, __LINE__)
	#define $vu(arr, size) debug_print_number_array((arr), #arr, size, sizeof(unsigned int), debug_print_uint, __FILE__, __func__, __LINE__)
	#define $vzu(arr, size) debug_print_number_array((arr), #arr, size, sizeof(size_t), debug_print_sizet, __FILE__, __func__, __LINE__)
	#define $vhhd(arr, size) debug_print_number_array((arr), #arr, size, sizeof(char), debug_print_char, __FILE__, __func__, __LINE__)

	#define DEBUG_LOCATOR_PREFIX "(In " COLOR_FILE_NAME "%s" COLOR_RESET ", function " COLOR_FUNC_NAME "%s" COLOR_RESET ", line " COLOR_LINE_NUMBER "%d" COLOR_RESET ") "
	#define $d(n) $debug(DEBUG_LOCATOR_PREFIX "(int) %s = %d", __FILE__, __func__, __LINE__, #n, (n))
	#define $u(n) $debug(DEBUG_LOCATOR_PREFIX "(unsigned int) %s = %u", __FILE__, __func__, __LINE__, #n, (n))
	#define $zu(n) $debug(DEBUG_LOCATOR_PREFIX "(size_t) %s = %zu", __FILE__, __func__, __LINE__, #n, (n))

	#define $xdi(item) debug_hex_dump_object((uint8_t*)&(item), sizeof(item), #item);
	#define $xdv(arr, size) debug_hex_dump_object((uint8_t*)(arr), (size), #arr);

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

	#define $raw
	#define $endraw

	#define $vs(str)
	#define $vd(arr, size)
	#define $vu(arr, size)
	#define $vzu(arr, size)
	#define $vhhd(arr, size)

	#define $d(n)
	#define $u(n)
	#define $zu(n)

	#define $xdi(item);
	#define $xdv(arr, size);

	#define $errno errno
	#define $cerrno(reason) errno

	#define $xerrno if (errno) exit(errno)
	#define $xcerrno(comment) if (errno) exit(errno)

#endif


#endif
