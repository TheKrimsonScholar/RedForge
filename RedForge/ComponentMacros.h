#pragma once

#include <unordered_map>
#include <typeindex>
#include <functional>
#include <string>

#include "Exports.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct ComponentVariableInfo
{
	std::type_index variableType;
	std::string variableName;

	std::function<std::istream&(std::istream&, void*)> readFromFile;
	std::function<std::ostream&(std::ostream&, void*)> writeToFile;
};
struct ComponentInfo
{
	ComponentInfo() = default;
	ComponentInfo(uint32_t componentID, std::string componentName, std::function<std::vector<std::pair<void*, ComponentVariableInfo>>(void*)> getVariables) :
		componentID(componentID), componentName(componentName), getVariables(getVariables) {};

	uint32_t componentID;
	std::string componentName;
	std::function<std::vector<std::pair<void*, ComponentVariableInfo>>(void*)> getVariables;
};

REDFORGE_API std::vector<std::type_index>& GetRegisteredComponentsList();
REDFORGE_API std::unordered_map<std::type_index, ComponentInfo>& GetRegisteredComponentInfoMap();

#define GET_COMPONENT_ID(componentTypeID) (uint32_t) GetRegisteredComponentInfoMap()[componentTypeID].componentID
#define GET_COMPONENT_NAME(componentTypeID) (std::string) GetRegisteredComponentInfoMap()[componentTypeID].componentName
// Returns a vector of void*-ComponentVariableInfo pairs indicating the location of variables and their type/name data
#define GET_COMPONENT_VARS(componentTypeID, componentPtr) (std::vector<std::pair<void*, ComponentVariableInfo>>) GetRegisteredComponentInfoMap()[componentTypeID].getVariables(componentPtr)

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
	return os << v.x << ' ' << v.y << ' ' << v.z;
}
inline std::istream& operator>>(std::istream& is, glm::vec3& v)
{
	return is >> v.x >> v.y >> v.z;
}

inline std::ostream& operator<<(std::ostream& os, const glm::quat& q)
{
	return os << q.x << ' ' << q.y << ' ' << q.z << ' ' << q.w;
}
inline std::istream& operator>>(std::istream& is, glm::quat& q)
{
	return is >> q.x >> q.y >> q.z >> q.w;
}

#define REGISTER_COMPONENT_BEGIN(Type)																													\
struct RegisterComponent_##Type																															\
{																																						\
	RegisterComponent_##Type()																															\
	{																																					\
		if(std::find(GetRegisteredComponentsList().begin(), GetRegisteredComponentsList().end(), typeid(Type)) != GetRegisteredComponentsList().end())	\
			return;																																		\
		if(GetRegisteredComponentInfoMap().find(typeid(Type)) != GetRegisteredComponentInfoMap().end())													\
			return;																																		\
																																						\
		GetRegisteredComponentsList().push_back(typeid(Type));																							\
		GetRegisteredComponentInfoMap().emplace(typeid(Type), ComponentInfo(																			\
			GetRegisteredComponentInfoMap().size(),																										\
			#Type,																																		\
			[](void* rawComponent) -> std::vector<std::pair<void*, ComponentVariableInfo>>																\
				{																																		\
					Type* component = static_cast<Type*>(rawComponent);																					\
																																						\
					return																																\
					{
#define COMPONENT_VAR(variableType, variableName)																										\
						{																																\
							&component->variableName,																									\
							{																															\
								typeid(variableType), #variableName,																					\
																																						\
								[](std::istream& is, void* variablePtr) -> std::istream&																\
									{																													\
										return is >> *static_cast<variableType*>(variablePtr);															\
									},																													\
								[](std::ostream& os, void* variablePtr) -> std::ostream&																\
									{																													\
										return os << *static_cast<variableType*>(variablePtr) << " ";															\
									}																													\
							}																															\
						},
#define REGISTER_COMPONENT_END(Type)																													\
					};																																	\
				} ));																																	\
	}																																					\
};																																						\
static RegisterComponent_##Type registerComponent_##Type;

//struct SerializeComponent_TransformComponent
//{																																						
//	SerializeComponent_TransformComponent()
//	{																																					
//		if(std::find(GetRegisteredComponentsList().begin(), GetRegisteredComponentsList().end(), typeid(TransformComponent)) != GetRegisteredComponentsList().end())
//			return;																																		
//		if(GetRegisteredComponentInfoMap().find(typeid(TransformComponent)) != GetRegisteredComponentInfoMap().end())
//			return;																																		
//																																						
//		GetRegisteredComponentsList().push_back(typeid(TransformComponent));
//		GetRegisteredComponentInfoMap().emplace(typeid(TransformComponent), ComponentInfo(
//			"TransformComponent",
//			[](void* rawComponent) -> std::vector<std::pair<void*, ComponentVariableInfo>>																
//				{																																		
//					TransformComponent* component = static_cast<TransformComponent*>(rawComponent);
//																																						
//					return																																
//					{
//						{
//							&component->location, 
//							{
//								typeid(glm::vec3), "location", 
//							
//								// Read from file
//								[](std::istream& is, void* variablePtr) -> std::istream&
//									{
//										//is >> static_cast<glm::vec3*>(variablePtr)->x;
//
//										return is >> static_cast<glm::vec3*>(variablePtr)->x;
//									},
//								// Write to file
//								[](std::ostream& os, void* variablePtr) -> std::ostream&
//									{
//										//os << static_cast<glm::vec3*>(variablePtr)->x;
//
//										return os << static_cast<glm::vec3*>(variablePtr)->x;
//									}
//							}
//						}
//					};																																	
//				} ));
//	}
//
//	friend std::ostream& operator<<(std::ostream& os, const TransformComponent& obj)
//	{
//		os << obj.location;
//		os << obj.rotation;
//		os << obj.scale;
//
//		return os;
//	}
//	friend std::istream& operator>>(std::istream& is, TransformComponent& obj)
//	{
//		is >> obj.value; // Read into the 'value' member
//		
//		return is;
//	}
//};																																						
//static SerializeComponent_TransformComponent serializeComponent_TransformComponent;

//struct SerializeComponent_TransformComponent
//{																																						
//	SerializeComponent_TransformComponent()
//	{																																					
//		if(std::find(GetRegisteredComponentsList().begin(), GetRegisteredComponentsList().end(), typeid(TransformComponent)) != GetRegisteredComponentsList().end())
//			return;																																		
//		if(GetRegisteredComponentInfoMap().find(typeid(TransformComponent)) != GetRegisteredComponentInfoMap().end())
//			return;																																		
//																																						
//		GetRegisteredComponentsList().push_back(typeid(TransformComponent));
//		GetRegisteredComponentInfoMap().emplace(typeid(TransformComponent), ComponentInfo(
//			"TransformComponent",
//			[](void* rawComponent) -> std::vector<std::pair<void*, ComponentVariableInfo>>																
//				{																																		
//					TransformComponent* component = static_cast<TransformComponent*>(rawComponent);
//																																						
//					return																																
//					{
//						{ &component->location, { typeid(glm::vec3), "location"}},
//					};																																	
//				},
//			[](void* rawComponent) -> std::string																
//				{																																		
//					TransformComponent* component = static_cast<TransformComponent*>(rawComponent);
//
//					os << component->location.x << component->location.y << component->location.z;
//																																						
//					return os;
//				} ));																																	
//	}
//
//	friend std::ostream& operator<<(std::ostream& os, const TransformComponent& obj)
//	{
//		os << obj.location;
//		os << obj.rotation;
//		os << obj.scale;
//
//		return os;
//	}
//	friend std::istream& operator>>(std::istream& is, TransformComponent& obj)
//	{
//		is >> obj.value; // Read into the 'value' member
//		
//		return is;
//	}
//};																																						
//static SerializeComponent_TransformComponent serializeComponent_TransformComponent;