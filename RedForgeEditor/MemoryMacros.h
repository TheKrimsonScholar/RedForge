#ifdef _MSC_VER
	// These must be included once to enable the CRT tracking
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>

	// Define the custom DEBUG_NEW macro
	// Note: The new DEBUG_NEW substitution MUST be done in the .cpp file itself
	#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#endif