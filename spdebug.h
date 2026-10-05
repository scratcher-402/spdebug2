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
#ifdef SPDEBUG_IMPLEMENTATION

#ifndef SPDEBUG_IMPLEMENTATION_IG
#define SPDEBUG_IMPLEMENTATION_IG

#ifndef SPDEBUG_H
#include "debug.h"
#endif

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

FILE* log_file = NULL;
const char* program_name = NULL;

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

char* debug_clean_string(const char* str) {
	// Removes ANSI color codes from string. Returns new string allocated in dynamic memory.
	char* clean_str = strdup(str);

	size_t str_pos = 0, clean_str_pos = 0;
	while (str[str_pos] != '\0') {
		if (str[str_pos] == '\x1b') {
			do {
				str_pos++;
			} while (str[str_pos] != '\0' && str[str_pos] != 'm');
			
			if (str[str_pos] == '\0') break;

			str_pos++;
		}
		
		clean_str[clean_str_pos] = str[str_pos];
		clean_str_pos++;
		str_pos++;
	}

	assert(clean_str_pos <= str_pos);
	clean_str[clean_str_pos] = '\0';

	return clean_str;
}

static void _print_tag(LogLevel level) {
	time_t now = time(NULL);
	struct tm *lt = localtime(&now);
	char buf[64];
	strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", lt);

	fprintf(stderr, "[%s %s] ", console_level_tags[level], buf);

	if (log_file) fprintf(log_file, "[%s %s] ", level_tags[level], buf);
	
	return;
}

static void _print_log_text(const char* format, va_list ap) {
	va_list ap_copy;
	va_copy(ap_copy, ap);
	
	vfprintf(stderr, format, ap_copy);

	va_end(ap_copy);

	if (log_file) {
		char* clean_format = debug_clean_string(format);
		vfprintf(log_file, clean_format, ap);
		free(clean_format);
	}

	return;
}

static void _print_log_end() {
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

void debug_start_program(const char* name) {
	program_name = name;
	
	if (!log_file) {
		log_file = fopen("debug.log", "a");
		if (!log_file) {
			$err("Failed to open log file. Error: %s (errno %d)", strerror(errno), errno);
		}
		fputs("\n\n\n", log_file);
	}

	atexit(debug_exit);

	if (name) {
		$info("Program %s started.", name);
	} else {
		$info("Program started.");
	}
}

void debug_exit() {
	if (program_name) {
		$info("Program %s finished.", program_name);
	} else {
		$info("Program finished.");
	}
	
	if (log_file) {
		fclose(log_file);
	}
}
#ifndef SPDEBUG_H
#include "debug.h"
#endif

#include <sys/ioctl.h>
#include <asm/termbits.h>
#include <stdio.h>
#include <unistd.h>

// To do:
// - debug_print_inline_string
// - debug_print_<type>_array for uint, long, size_t

int debug_saved_errno = 0;

typedef struct {
	size_t y_size;
	size_t x_size;
} DebugTerminalSize;

typedef unsigned int uint;

DebugTerminalSize debug_get_terminal_size() {
	DebugTerminalSize size = {24, 80};
	
	struct winsize ws;
	int result = ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	$errno;
	
	if (result) {
		$warn("[debug_get_terminal_size] Error while getting terminal size. Result code: %d. Falling back to default value (80x24)", result);
	} else {
		size.y_size = ws.ws_row;
		size.x_size = ws.ws_col;
	}

	return size;
}

static inline uint _debug_ceil_div(uint a, uint b) {
	if (a % b == 0) return a/b;
	else return a/b + 1;
}

static inline void _console_print_char(const char c, uint width) {
	assert(width >= 4);

	char buf[8] = {0};
	if (c == 0) {
		// Null-terminator
		buf[0] = '\\';
		buf[1] = '0';
		
		fprintf(stderr, COLOR_SYM_NULL "%*s" COLOR_RESET, width, (char*)&buf);
	} else if (c >= 7 && c <= 13) {
		// Named sequences
		buf[0] = '\\';
		
		switch (c) {
			case 7:
				buf[1] = 'a';
				break;
			case 8:
				buf[1] = 'b';
				break;
			case 9:
				buf[1] = 't';
				break;
			case 10:
				buf[1] = 'n';
				break;
			case 11:
				buf[1] = 'v';
				break;
			case 12:
				buf[1] = 'f';
				break;
			case 13:
				buf[1] = 'r';
				break;
		}

		fprintf(stderr, COLOR_SYM_NAMED "%*s" COLOR_RESET, width, (char*)&buf);
	} else if (c == ' ') {
		// Space
		buf[0] = '\'';
		buf[1] = ' ';
		buf[2] = '\'';
		
		fprintf(stderr, COLOR_SYM_SPACE "%*s" COLOR_RESET, width, (char*)&buf);
	} else if (c > 32 && c < 127) {
		// Printable chars
		buf[0] = c;
		
		fprintf(stderr, COLOR_SYM_NORMAL "%*s" COLOR_RESET, width, (char*)&buf);
	} else {
		fprintf(stderr, COLOR_SYM_UNNAMED "%*s\\x%02hhx" COLOR_RESET, width - 4, "", c);
	}
}

static inline void _print_char(FILE* file, const char c, uint width) {
	char buf[8] = {0};
	if (c == 0) {
		// Null-terminator
		buf[0] = '\\';
		buf[1] = '0';
		
		fprintf(file, "%*s", width, (char*)&buf);
	} else if (c >= 7 && c <= 13) {
		// Named sequences
		buf[0] = '\\';
		
		switch (c) {
			case 7:
				buf[1] = 'a';
				break;
			case 8:
				buf[1] = 'b';
				break;
			case 9:
				buf[1] = 't';
				break;
			case 10:
				buf[1] = 'n';
				break;
			case 11:
				buf[1] = 'v';
				break;
			case 12:
				buf[1] = 'f';
				break;
			case 13:
				buf[1] = 'r';
				break;
		}

		fprintf(file, "%*s", width, (char*)&buf);
	} else if (c == ' ') {
		// Space
		buf[0] = '\'';
		buf[1] = ' ';
		buf[2] = '\'';
		
		fprintf(file, "%*s", width, (char*)&buf);
	} else if (c > 32 && c < 127) {
		// Printable chars
		buf[0] = c;
		
		fprintf(file, "%*s", width, (char*)&buf);
	} else {
		if (width >= 4) fprintf(file, "%*s\\x%02hhx", width - 4, "", c);
		else fprintf(file, "\\x%02hhx", c);
	}
}

void debug_print_string(const char* file_name, const char* func_name, int line, const char* string_name, const char* s) {
	// To do:
	// - Write string to log file
	// - Add file, function and line information
	
	if (string_name == NULL) {
		$warn("[debug_print_string] String name is null. Falling back to default (\"s\")");
		string_name = "s";
	}

	if (s == NULL) {
		$err("[debug_print_string] Pointer to a string %s is null", string_name);
		return;
	}

	size_t length = strnlen(s, SPDEBUG_MAX_STRING_LEN);
	if (length == SPDEBUG_MAX_STRING_LEN) {
		$err("[debug_print_string] String %s is longer than max printing size (%zu). String may be corrupted.", string_name, SPDEBUG_MAX_STRING_LEN);
		return;
	}
	length++; // to print the null-terminator

	$debug("[debug_print_string] (in %s, function %s, line %d) String \"%s\", length %zu", file_name, func_name, line, string_name, length);
	
	// Print string to a terminal

	DebugTerminalSize ts = debug_get_terminal_size();
	ts.x_size -= 1;

	uint elements_per_row = ts.x_size / 5;
	uint row_count = _debug_ceil_div(length, elements_per_row);

	for (uint row = 0; row < row_count; row++) {
		// Print indexes
		for (uint col = 0; col < elements_per_row; col++) {
			uint index = row * elements_per_row + col;

			if (index == length) break;
			
			fprintf(stderr, COLOR_INDEX "%5u" COLOR_RESET, index);
		}
		fprintf(stderr, "\n");
		
		// Print values
		for (uint col = 0; col < elements_per_row; col++) {
			uint index = row * elements_per_row + col;

			if (index == length) break;

			_console_print_char(s[index], 5);
		}
		fprintf(stderr, "\n");
	}

	// Print it to a log file

	if (log_file) {
		// Print indexes
		for (uint index = 0; index < length; index++) fprintf(log_file, "%5u", index);
		fprintf(log_file, "\n");

		// Print values
		for (uint index = 0; index < length; index++) _print_char(log_file, s[index], 5);
		fprintf(log_file, "\n");

		fflush(log_file);
	}

	return;
}

void debug_print_int_array(const char* file_name, const char* func_name, int line, const char* arr_name, const int* arr, size_t length) {
	if (arr_name == NULL) {
		$warn("[debug_print_int_array] Array name is null. Falling back to default name (\"arr\")");
		arr_name = "arr";
	}

	$debug("[debug_print_int_array] (in %s, function %s, line %d) Array %s, length %zu", file_name, func_name, line, arr_name, length);

	// Handle empty array

	if (length == 0) {
		fprintf(stderr, COLOR_INDEX "	< empty array >" COLOR_RESET "\n");
		if (log_file) fprintf(log_file, "	< empty array >\n");
		return;
	}

	// Handle null array poiner

	if (arr == NULL) {
		$err("[debug_print_int_array] Pointer to array %s is null.", arr_name);
		return;	
	}

	// Print array to a terminal

	DebugTerminalSize ts = debug_get_terminal_size();
	ts.x_size -= 1;

	uint elements_per_row = ts.x_size / 10;
	uint row_count = _debug_ceil_div(length, elements_per_row);

	for (uint row = 0; row < row_count; row++) {
		// Print indexes
		for (uint col = 0; col < elements_per_row; col++) {
			uint index = row * elements_per_row + col;

			if (index == length) break;
			
			fprintf(stderr, COLOR_INDEX "%10u" COLOR_RESET, index);
		}
		fprintf(stderr, "\n");
		
		// Print values
		for (uint col = 0; col < elements_per_row; col++) {
			uint index = row * elements_per_row + col;

			if (index == length) break;

			fprintf(stderr, COLOR_VALUE "%10d" COLOR_RESET, arr[index]);
		}
		fprintf(stderr, "\n");
	}

	// Print array to a log file

	if (log_file) {
		for (uint index = 0; index < length; index++) fprintf(log_file, "%10u", index);
		fprintf(log_file, "\n");

		for (uint index = 0; index < length; index++) fprintf(log_file, "%10d", arr[index]);
		fprintf(log_file, "\n");

		fflush(log_file);
	}

	return;
}

int debug_check_errno(const char* file_name, const char* func_name, int line, const char* print_reason) {
	debug_saved_errno = errno;
	if (errno) {
		if (print_reason) {
			$err("(in %s, function %s, line %d) %s: %s (Errno %d)", file_name, func_name, line, print_reason, strerror(errno), errno);
		} else {
			$err("(in %s, function %s, line %d) Error in standard library: %s (Errno %d)", file_name, func_name, line, strerror(errno), errno);
		}
	}
	return debug_saved_errno;
}
#ifndef SPDEBUG_H
#include "debug.h"
#endif

void debug_hex_dump_object(uint8_t* object, size_t object_size) {
	for (size_t i = 0; i < object_size; i += 8) {
		for (size_t j = 0; j < 8 && (i + j) < object_size; j++) {
			fprintf(stderr, "%02hhx ", object[i + j]);
		}
		fprintf(stderr, "\n");
	}
}
#endif
#endif
