#pragma once

#include <string>
#include <filesystem>

struct MaterialRef
{
	std::filesystem::path identifier;

	MaterialRef(const std::wstring& identifier = L"default") : identifier(identifier) {}

	friend std::ostream& operator<<(std::ostream& os, const MaterialRef& material)
	{
		return os << material.identifier;
	}
	friend std::istream& operator>>(std::istream& is, MaterialRef& material)
	{
		return is >> material.identifier;
	}
};