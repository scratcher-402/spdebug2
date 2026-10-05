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
