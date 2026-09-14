#include "debug.h"
#include <sys/ioctl.h>
#include <asm/termbits.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_STRING_LEN 2048

typedef struct {
	size_t y_size;
	size_t x_size;
} TerminalSize;

typedef unsigned int uint;

TerminalSize debug_get_terminal_size() {
	TerminalSize size = {24, 80};
	
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

inline uint _ceil_div(uint a, uint b) {
	if (a % b == 0) return a/b;
	else return a/b + 1;
}

inline void _console_print_char(const char c) {
	char buf[8] = {0};
	if (c == 0) {
		// Null-terminator
		buf[0] = '\\';
		buf[1] = '0';
		
		fprintf(stderr, COLOR_SYM_NULL "%5s" COLOR_RESET, &buf);
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

		fprintf(stderr, COLOR_SYM_NAMED "%5s" COLOR_RESET, &buf);
	} else if (c == ' ') {
		// Space
		buf[0] = '\'';
		buf[1] = ' ';
		buf[2] = '\'';
		
		fprintf(stderr, COLOR_SYM_SPACE "%5s" COLOR_RESET, &buf);
	} else if (c > 32 && c < 127) {
		// Printable chars
		buf[0] = c;
		
		fprintf(stderr, COLOR_SYM_NORMAL "%5s" COLOR_RESET, &buf);
	} else {
		fprintf(stderr, COLOR_SYM_UNNAMED " \\x%02hhx" COLOR_RESET, c);
	}
}

void debug_print_string(const char* string_name, const char* s) {
	if (string_name == NULL) {
		$warn("[debug_print_string] String name is null. Falling back to default (\"s\")");
		string_name = "s";
	}

	if (s == NULL) {
		$err("[debug_print_string] Pointer to a string %s is null", string_name);
		return;
	}

	size_t length = strnlen(s, MAX_STRING_LEN);
	if (length == MAX_STRING_LEN) {
		$err("[debug_print_string] String %s is longer than max printing size (%zu). String may be corrupted.", string_name, MAX_STRING_LEN);
		return;
	}
	length++; // to print the null-terminator

	$debug("[debug_print_string] Start of string \"%s\"", string_name);
	
	// Print string to a terminal

	TerminalSize ts = debug_get_terminal_size();
	ts.x_size -= 1;

	uint elements_per_row = ts.x_size / 5;
	uint row_count = _ceil_div(length, elements_per_row);

	for (uint row = 0; row < row_count; row++) {
		// Print indexes
		for (uint col = 0; col < elements_per_row; col++) {
			fprintf(stderr, COLOR_INDEX "%5d" COLOR_RESET, row * elements_per_row + col);
		}
		fprintf(stderr, "\n");
		
		// Print values
		for (uint col = 0; col < elements_per_row; col++) {
			uint index = row * elements_per_row + col;

			if (index == length) break;

			_console_print_char(s[index]);
		}
		fprintf(stderr, "\n");
	}

	$debug("[debug_print_string] End of string \"%s\"", string_name);
}

void debug_print_int_array(const char* arr_name, const int* arr, size_t length) {
	if (arr_name == NULL) {
		$warn("[debug_print_int_array] Array name is null. Falling back to default name (\"arr\")");
		arr_name = "arr";
	}

	if (arr == NULL) {
		$err("[debug_print_int_array] Pointer to array %s is null.", arr_name);
		return;	
	}

	$debug("[debug_print_int_array] Start of array %s", arr_name);

	// Print array to a terminal

	TerminalSize ts = debug_get_terminal_size();
	ts.x_size -= 1;

	uint elements_per_row = ts.x_size / 10;
	uint row_count = _ceil_div(length, elements_per_row);

	for (uint row = 0; row < row_count; row++) {
		// Print indexes
		for (uint col = 0; col < elements_per_row; col++) {
			fprintf(stderr, COLOR_INDEX "%10d" COLOR_RESET, row * elements_per_row + col);
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
	
	$debug("[debug_print_int_array] End of array %s", arr_name);
}
