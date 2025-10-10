#pragma once
#include "CollisionManifold.hpp"
#include "Particle.hpp"
#include "../Globals.hpp"

class PhysicsSolver
{
	Globals& globals = Globals::getInstance();

public:
	PhysicsSolver();

	float Cr = 0.f;// 0.3f;
	float u_friction = 0.8f;
	sf::Vector2f Ga = { 0, 98.07f };
	sf::Vector2f windowAcceleration{};
	int simGridResolution = 120;
	int solverIterations = 3;

	void resolveCollisions(std::vector<Particle>& bodies, float dt);
	void preupdatePositions(std::vector<Particle>& bodies, float dt) const;
	void updatePositions(std::vector<Particle>& bodies, float dt) const;

private:
	std::optional<CollisionManifold> detectCollision(Particle& b1, Particle& b2);
};