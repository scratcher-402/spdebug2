#define SPDEBUG_IMPLEMENTATION
#include "spdebug.h"

int main() {
	$init;

	int n = 6767;

	$d(n);

	int arr[67] = {1, 2, 3, 4, 5, 6, 7, 6, 7, 6, 7, 67, 67, 67, 67, 6767, 6767, 6767, 6767, 67, 67, 67, 67, 6, 7, 6, 7, 6, 7, -6, -7};

	$vd(arr, 67);
	
	const char* s = "Hello World\n and MIPT. \r\v Welcome to Gboard!   \x67\xb4\x0e\xda";

	$vs(s);

	return 0;
}
