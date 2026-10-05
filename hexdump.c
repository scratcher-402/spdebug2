#ifndef SPDEBUG_H
#include "debug.h"
#endif

void debug_hex_dump_object(uint8_t* object, size_t object_size, const char* object_name) {
	$debug("Hexadecimal dump of %s:", object_name);
	
	for (size_t i = 0; i < object_size; i += 8) {
		for (size_t j = 0; j < 8 && (i + j) < object_size; j += 1) {
			fprintf(stderr, "%02hhx ", object[i + j]);
		}
		fprintf(stderr, "\n");
	}

	$debug("End of a hexadecimal dump.");
	return;
}
