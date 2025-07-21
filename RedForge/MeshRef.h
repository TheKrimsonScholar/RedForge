#pragma once

#include <string>
#include <filesystem>

struct MeshRef
{
	std::filesystem::path identifier;

	MeshRef(const std::wstring& identifier = L"default.obj") : identifier(identifier) {}

	friend std::ostream& operator<<(std::ostream& os, const MeshRef& mesh)
	{
		return os << mesh.identifier;
	}
	friend std::istream& operator>>(std::istream& is, MeshRef& mesh)
	{
		return is >> mesh.identifier;
	}
};