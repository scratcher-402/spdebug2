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
	// Function is deprecated
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

void debug_print_number_array(void* arr, const char* arr_name, size_t arr_size, size_t arr_elem_size, void(*print_func)(const void*, int), const char* file_name, const char* func_name, int line_number) {
	// TODO: Automatic element width
	assert(arr_name != NULL);
	assert(file_name != NULL);
	assert(func_name != NULL);
	assert(print_func != NULL);

	$raw;
	
	$debug("[debug_print_number_array] (in %s, function %s, line %d) Array %s, length %zu\n", file_name, func_name, line_number, arr_name, arr_size);

	// Handle empty array
	if (arr_size == 0) {
		$debug(COLOR_INDEX "< empty array >" COLOR_RESET "\n");
	}

	// Handle null array pointer
	if (arr == NULL) {
		$debug(COLOR_ERROR "< array pointer is null >" COLOR_RESET "\n");
	}

	// Print array
	
	DebugTerminalSize ts = debug_get_terminal_size();
	ts.x_size -= 1;

	uint elements_per_row = ts.x_size / 10;
	uint row_count = _debug_ceil_div(arr_size, elements_per_row);

	for (uint row = 0; row < row_count; row++) {
		// Print indexes
		for (uint col = 0; col < elements_per_row; col++) {
			uint index = row * elements_per_row + col;

			if (index == arr_size) break;
			
			$debug(COLOR_INDEX "%10u" COLOR_RESET, index);
		}
		$debug("\n");
		
		// Print values
		for (uint col = 0; col < elements_per_row; col++) {
			uint index = row * elements_per_row + col;

			if (index == arr_size) break;

			print_func(arr + index * arr_elem_size, 10);
		}
		fprintf(stderr, "\n");
	}

	$endraw;

	return;
}

void debug_print_int(const void* number, int width) {
	$debug("%*d", width, *(int*)number);
}

void debug_print_uint(const void* number, int width) {
	$debug("%*u", width, *(uint*)number);
}

void debug_print_sizet(const void* number, int width) {
	$debug("%*zu", width, *(size_t*)number);
}

void debug_print_char(const void* number, int width) {
	$debug("%*hhd", width, *(char*)number);
}
