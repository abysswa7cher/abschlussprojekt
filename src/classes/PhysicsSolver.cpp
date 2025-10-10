#include "PhysicsSolver.hpp"
#include "Particle.hpp"
#include <SFML/System/Vector2.hpp>
#include <iterator>
#include <vector>
#include <ranges>
#include <set>
#include <iostream>

PhysicsSolver::PhysicsSolver()
{
}

std::optional<CollisionManifold> PhysicsSolver::detectCollision(Particle& p1, Particle& p2)
{
	sf::Vector2f V = p2.position - p1.position;
	float d = V.length();
	float combinedRadius = p1.shape.get()->getRadius() + p2.shape.get()->getRadius();

	if (d <= combinedRadius) {
		return CollisionManifold
		{ p1, p2, V.length() > 0 ? V.normalized() : sf::Vector2f{}, combinedRadius - d };
	}

	return std::nullopt;
}

void PhysicsSolver::preupdatePositions(std::vector<Particle>& bodies, float dt) const
{
	for (auto& body : bodies)
	{
		if (!body.active) continue;
		// Apply gravity and update velocity
		body.velocity += Ga * dt;
		body.velocity += windowAcceleration * dt;
		// Store the current position before predicting the next one
		body.oldPosition = body.position;
		// Predict the next position
		body.position += body.velocity * dt;
	}
}

void PhysicsSolver::updatePositions(std::vector<Particle>& bodies, float dt) const
{
	for (auto& body : bodies)
	{
		if (!body.active) continue;
		body.velocity = (body.position - body.oldPosition) / dt;

		// Optional: Add damping
		body.velocity *= 0.999f;
	}
}

void PhysicsSolver::resolveCollisions(std::vector<Particle>& bodies, float dt)
{

	for (int i{ solverIterations }; i > 0; --i)
	{

		// =====================================================================
		// 1. Initialize and Populate Spatial Grid
		// =====================================================================
		const int gridWidth = globals.WINDOW_WIDTH / simGridResolution + 1;
		const int gridHeight = globals.WINDOW_HEIGHT / simGridResolution + 1;

		std::vector<std::vector<std::vector<Particle*>>> grid(gridWidth, std::vector<std::vector<Particle*>>(gridHeight));
		grid.resize(gridWidth);

		for (auto& row : grid) {
			for (auto& cell : row) {
				cell.clear();
			}
		}

		// Populate the grid by iterating through particles once
		for (Particle& body : bodies)
		{
			if (!body.active) continue;

			// Correctly calculate which cell the particle is in by dividing by cell SIZE
			int cx = static_cast<int>(body.position.x / simGridResolution);
			int cy = static_cast<int>(body.position.y / simGridResolution);

			// Add a pointer to the particle in the correct cell, with bounds checking
			if (cx >= 0 && cx < gridWidth && cy >= 0 && cy < gridHeight) {
				grid[cx][cy].push_back(&body);
			}
		}

		// =====================================================================
		// 2. Resolve Particle-Particle Collisions Using the Grid
		// =====================================================================

		for (int i = 0; i < gridWidth; ++i)
		{
			for (int j = 0; j < gridHeight; ++j)
			{
				std::vector<Particle*>& current_cell = grid[i][j];

				// A lambda function to handle the collision logic for any two particles
				auto checkAndResolvePair = [&](Particle* bodyA, Particle* bodyB)
					{
						// Order the pair by pointer address to prevent duplicates like (A,B) and (B,A)
						if (bodyA > bodyB) std::swap(bodyA, bodyB);

						auto manifoldOpt = detectCollision(*bodyA, *bodyB);
						if (manifoldOpt)
						{
							CollisionManifold& manifold = manifoldOpt.value();

							sf::Vector2f vRel = manifold.bodyB.velocity - manifold.bodyA.velocity;
							float vRelDotNormal = vRel.dot(manifold.normal);

							if (vRelDotNormal < 0)
							{
								// impact force
								// consider mass being 1
								float Jmag = (-(1 + Cr) * vRelDotNormal) / 2.f;
								sf::Vector2f Jn = Jmag * manifold.normal;
								auto collisionForce = Jn / dt;

								manifold.bodyA.velocity -= Jn;
								manifold.bodyB.velocity += Jn;

								// friction
								vRel = manifold.bodyB.velocity - manifold.bodyA.velocity;
								sf::Vector2f tangent =
									vRel - (vRel.dot(manifold.normal)) * manifold.normal;

								if (tangent.length() > 0.f)
								{
									tangent = tangent.normalized();
									float Jt_mag = -vRel.dot(tangent);
									float frictionJmag = std::min(std::abs(Jt_mag), Jmag * u_friction);
									sf::Vector2f frictionImpulse = frictionJmag * tangent;

									manifold.bodyA.velocity -= frictionImpulse;
									manifold.bodyB.velocity += frictionImpulse;
								}
							}

							// correction to avoid sinking
							const float percent = 0.2f;
							const float slop = 0.01f;
							sf::Vector2f correction = manifold.normal * (std::max(manifold.penetrationDepth - slop, 0.0f) * 0.5f * percent);

							manifold.bodyA.position -= correction;
							manifold.bodyB.position += correction;
						}
					};

				// --- A) Check for collisions WITHIN the current cell ---
				for (size_t p1_idx = 0; p1_idx < current_cell.size(); ++p1_idx)
				{
					for (size_t p2_idx = p1_idx + 1; p2_idx < current_cell.size(); ++p2_idx)
					{
						checkAndResolvePair(current_cell[p1_idx], current_cell[p2_idx]);
					}
				}

				// --- B) Check for collisions with NEIGHBORING cells ---
				// (Only check in directions that haven't been checked already to avoid duplicates)
				int neighbor_offsets[4][2] = {
					{1, 0},   // Right
					{0, 1},   // Down
					{-1, 1},  // Down-Left
					{1, 1}    // Down-Right
				};

				for (const auto& offset : neighbor_offsets)
				{
					int ni = i + offset[0];
					int nj = j + offset[1];

					if (ni >= 0 && ni < gridWidth && nj >= 0 && nj < gridHeight)
					{
						std::vector<Particle*>& neighbor_cell = grid[ni][nj];
						for (Particle* p1 : current_cell)
						{
							for (Particle* p2 : neighbor_cell)
							{
								checkAndResolvePair(p1, p2);
							}
						}
					}
				}
			}
		}

		// =====================================================================
		// 3. Screen Boundary Collisions
		// =====================================================================
		for (auto& body : bodies)
		{
			if (body.active)
			{
				const float radius = body.shape->getRadius();
				if (body.position.x - radius < 0)
				{
					body.position.x = radius;
					body.velocity.x = -Cr * body.velocity.x;
				}
				else if (body.position.x + radius > globals.WINDOW_WIDTH)
				{
					body.position.x = globals.WINDOW_WIDTH - radius;
					body.velocity.x = -Cr * body.velocity.x;
				}
				if (body.position.y - radius < 0)
				{
					body.position.y = radius;
					body.velocity.y = -Cr * body.velocity.y;
				}
				else if (body.position.y + radius > globals.WINDOW_HEIGHT)
				{
					body.position.y = globals.WINDOW_HEIGHT - radius;
					body.velocity.y = -Cr * body.velocity.y;
				}
			}
		}
	}
}