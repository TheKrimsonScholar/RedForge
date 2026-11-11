#include "TypeRegistration.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

TypeRegistry& RegisterTypeBase::GetRegistryInstance()
{
	return TypeRegistry::GetInstance();
}