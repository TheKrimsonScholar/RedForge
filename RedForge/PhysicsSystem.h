#pragma once

#include <unordered_map>
#include <vector>

#include "EntityManager.h"
#include "TransformComponent.h"
#include "ColliderComponent.h"
#include "PhysicsComponent.h"

#include <glm/glm.hpp>

struct CollisionPair
{
private:
	const ColliderComponent* a;
	const ColliderComponent* b;

public:
	CollisionPair(const ColliderComponent* a = nullptr, const ColliderComponent* b = nullptr) : a(a), b(b) {}

	bool operator==(const CollisionPair& other) const
	{
		// Order-agnostic equality
		return (&a == &other.a && &b == &other.b)
			|| (&a == &other.b && &b == &other.a);
	}

	const ColliderComponent* GetFirst() const { return a; };
	const ColliderComponent* GetSecond() const { return b; };

	friend std::hash<CollisionPair>;
};
namespace std
{
	template<>
	struct hash<glm::vec3>
	{
		std::size_t operator()(const glm::vec3& v) const
		{
			/* Hash and combine x, y, z */

			std::hash<float> hasher;

			return hasher(v.x)
				^ (hasher(v.y) << 1)
				^ (hasher(v.z) << 2);
		}
	};

	template<>
	struct hash<CollisionPair>
	{
		std::size_t operator()(CollisionPair collisionPair) const
		{
			/* Hash and combine pointers */

			uintptr_t hashA = reinterpret_cast<uintptr_t>(&collisionPair.a);
			uintptr_t hashB = reinterpret_cast<uintptr_t>(&collisionPair.b);

			// Sort for order-independence
			if(hashA > hashB)
				std::swap(hashA, hashB);

			return hashA ^ (hashB << 1);
		}
	};
}

struct ContactPoint
{
	glm::vec3 location;
	glm::vec3 normal;
	float penetrationDepth;
};

struct CollisionData
{
	Entity entityA;
	Entity entityB;
	CollisionPair colliderPair;
	std::vector<ContactPoint> contacts;
};

class PhysicsSystem
{
private:
	static inline PhysicsSystem* Instance;

	std::unordered_map<CollisionPair, std::vector<ContactPoint>> cachedContacts;

	bool tickPhysics = true;

public:
	PhysicsSystem() {};
	~PhysicsSystem() {};

	void Startup();
	void Shutdown();

	REDFORGE_API void Update();

private:
	void UpdatePhysics(TransformComponent& transform, PhysicsComponent& rigidbody, float deltaTime);

	void ApplyImpulse(const TransformComponent& transform, const ColliderComponent& collider, PhysicsComponent& rigidbody, const glm::vec3& impulse, const glm::vec3& location);

	glm::mat4 GetInertiaTensor(const TransformComponent& transform, const ColliderComponent& collider, const PhysicsComponent& rigidbody);

	bool GJK(const TransformComponent& transformA, const ColliderComponent& a, const TransformComponent& transformB, const ColliderComponent& b, std::vector<ContactPoint>& out_contactPoints);
	bool UpdateSimplex(std::deque<glm::vec3>& simplex, glm::vec3& direction);

	bool UpdateSimplex_LineCase(std::deque<glm::vec3>& simplex, glm::vec3& direction);
	bool UpdateSimplex_TriangleCase(std::deque<glm::vec3>& simplex, glm::vec3& direction);
	bool UpdateSimplex_TetrahedronCase(std::deque<glm::vec3>& simplex, glm::vec3& direction);

	std::vector<ContactPoint> EPA(std::deque<glm::vec3> simplex, std::unordered_map<glm::vec3, std::pair<glm::vec3, glm::vec3>> supportPoints, const TransformComponent& transformA, const ColliderComponent& a, const TransformComponent& transformB, const ColliderComponent& b);
	void EPA_GetPolytopeNormals(std::vector<glm::vec3> polytope, std::vector<unsigned int> faces, std::vector<std::pair<glm::vec3, float>>& out_normals, unsigned int& out_minNormalIndex);
	void EPA_AddIfUniqueEdge(std::vector<std::pair<unsigned int, unsigned int>>& edges, const std::vector<unsigned int>& faces, unsigned int a, unsigned int b);

	std::vector<glm::vec3> GetContactPoints(std::vector<glm::vec3> incidentFace, std::vector<glm::vec3> referenceFace, glm::vec3 referenceNormal);
	void ClipPolygonAgainstPlane(std::vector<glm::vec3>& points, glm::vec3 planeNormal, float planeOffset);

	bool AreContactsValidInCache(CollisionPair colliderPair, std::vector<ContactPoint> newContacts);

	/* Component helpers */

	// Convex colliders
	glm::vec3 GJK_Support(const TransformComponent& transform, const ColliderComponent& collider, const glm::vec3& direction) const;
	std::vector<glm::vec3> EPA_GetAlignedFace(const TransformComponent& transform, const ColliderComponent& collider, const glm::vec3& direction, glm::vec3& out_faceNormal) const;
};