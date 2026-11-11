#include "AlgorithmUtils.h"

size_t CombineHashes(size_t h1, size_t h2)
{
	return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2)); // Boost algorithm (0x9e3779b9 = inverse golden ratio)
}