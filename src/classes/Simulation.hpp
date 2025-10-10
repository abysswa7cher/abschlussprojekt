#pragma once
#include "PhysicsSolver.hpp"
#include "Particle.hpp"
#include <SFML/Graphics.hpp>
#include <random>
#include "../Globals.hpp"

using Vector2d = sf::Vector2<double>;

class Simulation
{
	Globals& globals = Globals::getInstance();
	std::vector<Particle> bodies;
	int body_id = 0;

	float time_accumulator = 0.f;

	std::mt19937 rng;

public:
	bool running = false;
	int timeScale = 1;
	float physics_dt = 1.f / 60.f;

	bool addingParticleSpray = false;
	bool addParticleSprayActive = false;
	float sprayRadius = 100.f;

	PhysicsSolver physicsSolver;
	sf::Vector2i currentMousePosition;

	Simulation(const sf::Vector2u& windowSize);

	void update(float dt);
	void draw(sf::RenderWindow& window);
	void addParticle(sf::Vector2f initialPosition, sf::Vector2f initialVelocity, sf::Vector2f initialAcceleration);

	void addParticleSpray(sf::Vector2f mousePos);

	void togglePause();

	void clearSim();

	void handleMouseEvent(sf::RenderWindow& window, const sf::Event& event);
	void handleKeyboardPress(sf::RenderWindow& window, sf::Event::KeyPressed keyPressed);

	std::vector<Particle>& getBodies();

private:
	void cleanupBodies();
};