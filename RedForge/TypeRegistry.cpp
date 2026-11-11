#include "TypeRegistry.h"

#include "ComponentRegistration.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

REGISTER_TYPE(IComponent)
REGISTER_TYPE(IReplicatedComponent, IComponent)

REGISTER_TYPE(IResource)

REGISTER_TYPE(IEvent)

TypeRegistry* TypeRegistry::instance;

TypeRegistry::TypeRegistryDestructor TypeRegistry::destructor;

TypeRegistry::TypeRegistryDestructor::TypeRegistryDestructor()
{
	
}
TypeRegistry::TypeRegistryDestructor::~TypeRegistryDestructor()
{
	TypeRegistry::DestroyInstance();
}

void TypeRegistry::RegisterAllTypes()
{
	std::vector<std::type_index> typeOrder = typeIndexHierarchy.GetTopologicallySortedListBFS(
		[this](const std::type_index& t1, const std::type_index& t2) -> bool
		{
			return RegisterTypeBase::Compare(*registrars.at(t1), *registrars.at(t2));
		});
	// Use the sorted type list to establish the types map
	for(size_t i = 0; i < typeOrder.size(); i++)
	{
		TypeID typeID = { i };

		// Add the type to the TypeID hierarchy and add connections to all of its parent types
		typeHierarchy.AddNode(typeID);
		for(const std::type_index& parentTypeIndex : typeIndexHierarchy.GetInValuesOfNode(typeOrder[i]))
			typeHierarchy.AddConnection(registeredTypesMap.at(parentTypeIndex), typeID);

		// Associate this type index with its corresponding TypeID (index in the registered types list)
		registeredTypesMap.emplace(typeOrder[i], typeID);
	}

	for(const std::type_index& typeIndex : typeOrder)
		registrars.at(typeIndex)->Register();
}

void TypeRegistry::DestroyInstance()
{
	if(instance)
		delete instance;

	instance = nullptr;
}

TypeRegistry& TypeRegistry::GetInstance()
{
	if(!instance)
		instance = new TypeRegistry();

	return *instance;
}