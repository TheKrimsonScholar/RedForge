#include "TypeRegistration.h"

TypeRegistry& RegisterTypeBase::GetRegistryInstance()
{
	return TypeRegistry::GetInstance();
}