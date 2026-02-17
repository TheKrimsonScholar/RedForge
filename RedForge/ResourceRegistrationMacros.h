#pragma once

#include "ResourceMacros.h"

struct RegisterResourceBase
{
protected:
	template<typename T>
	T* CreateResource()
	{
		static_assert(std::is_base_of_v<IResource, T>);

		return new T();
	}
};

#define REGISTER_RESOURCE(Type)	\
REGISTER_RESOURCE_BEGIN(Type)		\
REGISTER_RESOURCE_END(Type)
#define REGISTER_RESOURCE_BEGIN(Type)																													\
struct RegisterResource_##Type : public RegisterResourceBase																							\
{																																						\
	RegisterResource_##Type()																															\
	{																																					\
		std::type_index typeID = typeid(Type);																											\
																																						\
		if(std::find(GetRegisteredResourcesList().begin(), GetRegisteredResourcesList().end(), typeID) != GetRegisteredResourcesList().end())		\
			return;																																		\
		if(GetRegisteredResourceInfoMap().find(typeID) != GetRegisteredResourceInfoMap().end())														\
			return;																																		\
																																						\
		auto castResourcePtr = [](void* rawResource) -> Type*																							\
			{																																			\
				return static_cast<Type*>(rawResource);																								\
			};																																			\
																																						\
		GetRegisteredResourcesList().push_back(typeID);																								\
																																						\
		ResourceInfo& resourceInfo = GetRegisteredResourceInfoMap()[typeID];																			\
		resourceInfo.createResource = [this]() { return CreateResource<Type>(); };															\
		resourceInfo.resourceID = GetRegisteredResourceInfoMap().size();																				\
		resourceInfo.resourceName = #Type;
#define RESOURCE_DEPENDS_ON(resourceType)																												\
		resourceInfo.dependencies.push_back(typeid(resourceType));																					\
		GetRegisteredResourcesInfoMap()[typeid(resourceType)].dependents.push_back(typeID);
#define RESOURCE_VARS_BEGIN																															\
		resourceInfo.getVariables = [castResourcePtr](void* rawResource) -> std::vector<std::pair<void*, ResourceVariableInfo>>						\
			{																																			\
				auto resource = castResourcePtr(rawResource);																						\
																																						\
				return																																	\
				{
#define RESOURCE_VAR(variableType, variableName)																										\
					{																																	\
						&resource->variableName,																										\
						{																																\
							typeid(variableType), #variableName,																						\
																																						\
							[](std::istream& is, void* variablePtr) -> std::istream&																	\
								{																														\
									return is >> *static_cast<variableType*>(variablePtr);																\
								},																														\
							[](std::ostream& os, void* variablePtr) -> std::ostream&																	\
								{																														\
									return os << *static_cast<variableType*>(variablePtr) << " ";														\
								}																														\
						}																																\
					},
#define RESOURCE_VARS_END																																\
				};																																		\
			};
#define REGISTER_RESOURCE_END(Type)																													\
	}																																					\
};																																						\
static inline RegisterResource_##Type registerResource_##Type;