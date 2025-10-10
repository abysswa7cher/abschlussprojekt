#include "Particle.hpp"

struct CollisionManifold
{
	Particle& bodyA;
	Particle& bodyB;
	sf::Vector2f normal;
	float penetrationDepth;

	CollisionManifold(Particle& bodyA, Particle& bodyB, sf::Vector2f normal, float penetrationDepth)
		: bodyA(bodyA)
		, bodyB(bodyB)
		, normal(normal)
		, penetrationDepth(penetrationDepth) {
	};
};