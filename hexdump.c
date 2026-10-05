#ifndef SPDEBUG_H
#include "debug.h"
#endif

void debug_hex_dump_object(uint8_t* object, size_t object_size, const char* object_name) {
	assert(object != NULL);

	$raw;
	
	$debug("Hexadecimal dump of %s (at %p, size %zu): \n", object_name, object, object_size);

	// Print indexes
	$debug("     ");
	
	for (size_t j = 0; j < 8; j += 1) {
		$debug(COLOR_INDEX "%2zu " COLOR_RESET, j);
	}

	$debug("\n");

	// Print bytes
	
	for (size_t i = 0; i < object_size; i += 8) {
		$debug(COLOR_INDEX "%4zu " COLOR_RESET, i);
		
		for (size_t j = 0; j < 8 && (i + j) < object_size; j += 1) {
			$debug("%02hhx ", object[i + j]);
		}
		$debug("\n");
	}

	$debug("End of a hexadecimal dump.");

	$endraw;
	return;
}
