#include "Editor.h"

#include "Engine.h"

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