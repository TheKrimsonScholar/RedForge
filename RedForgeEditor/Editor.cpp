#include "Editor.h"

#include "Engine.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

Engine* Editor::CreateEngineInstance()
{
	if(engine)
		return engine;

	engine = new Engine();
	engine->CreateVulkanInstance();

	return engine;
}
void Editor::DestroyEngineInstance()
{
	if(!engine)
		return;

	engine->DestroyVulkanInstance();
	delete engine;
}

Engine* Editor::GetEngine()
{
	return engine;
}
World& Editor::GetWorld()
{
	return engine->GetWorld();
}
EntityManager& Editor::GetEntityManager()
{
	return engine->GetWorld().GetEntityManager();
}