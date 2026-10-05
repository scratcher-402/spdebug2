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
			if (str[str_pos] == '\x1b') continue;
		}
		
		clean_str[clean_str_pos] = str[str_pos];
		clean_str_pos++;
		str_pos++;
	}

	assert(clean_str_pos <= str_pos);
	clean_str[clean_str_pos] = '\0';

	return clean_str;
}

static inline void _print_tag(LogLevel level) {
	time_t now = time(NULL);
	struct tm *lt = localtime(&now);
	char buf[64];
	strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", lt);

	fprintf(stderr, "[%s %s] ", console_level_tags[level], buf);

	if (log_file) fprintf(log_file, "[%s %s] ", level_tags[level], buf);
	
	return;
}

static inline int _check_if_clean_needed(const char* format) {
	size_t i = 0;
	
	while (format[i] != '\x1b' && format[i] != '\0') i++;

	return format[i] == '\x1b';
}

static inline void _print_log_text(const char* format, va_list ap) {
	va_list ap_copy;
	va_copy(ap_copy, ap);
	
	vfprintf(stderr, format, ap_copy);

	va_end(ap_copy);

	if (log_file) {
		if (_check_if_clean_needed(format)) {
			char* clean_format = debug_clean_string(format);
			vfprintf(log_file, clean_format, ap);
			free(clean_format);
		} else {
			vfprintf(log_file, format, ap);
		}
	}

	return;
}

static inline void _print_log_end() {
	fprintf(stderr, "\n");

	if (log_file) {
		fprintf(log_file, "\n");
		fflush(log_file);
	}

	return;
}

/*
	LOG MODES

 1. "Auto" mode
In this mode, every string passed to debug_log is written as a separate log message, 
printed on a new line with a tag. Log file buffer is flushed every time automatically. 
 2. "Raw" mode
In raw mode, debug_log acts almost like printf function, but the data is written to logging outputs.
At the first call of debug_log after raw mode is enabled, the tag is printed.
To enable raw mode, use debug_start_raw() or $raw macro.
You must quit the raw mode after a task that requires it is finished. To do this, use 
debug_end_raw() or $endraw macro.
While the raw mode is enabled, you should use only one log level. Using multiple log levels
is UB in this version of spdebug.
*/

void debug_log(LogLevel level, const char* format, ...) {
	if (level < debug_config.log_level) return;
	
	// Initialize argument list
	va_list ap;
	va_start(ap, format);

	if (debug_log_mode == LOG_MODE_AUTO) _print_tag(level);
	if (debug_log_mode == LOG_MODE_RAW_PREP) {
		_print_tag(level);
		debug_log_mode = LOG_MODE_RAW;
	}
	
	_print_log_text(format, ap);
	
	if (debug_log_mode == LOG_MODE_AUTO) _print_log_end();

	va_end(ap);
	return;
}

void debug_start_raw() {
	debug_log_mode = LOG_MODE_RAW_PREP;
}

void debug_end_raw() {
	if (debug_log_mode == LOG_MODE_RAW) _print_log_end();
	debug_log_mode = LOG_MODE_AUTO;
}

void debug_start_program(const char* name) {
	debug_config.program_name = name;
	
	if (!log_file) {
		log_file = fopen(debug_config.log_file_path, "a");
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
	if (debug_config.program_name) {
		$info("Program %s finished.", debug_config.program_name);
	} else {
		$info("Program finished.");
	}
	
	if (log_file) {
		fclose(log_file);
	}
}
