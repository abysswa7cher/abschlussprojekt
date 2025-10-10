#pragma once
#include <SFML/Graphics.hpp>
#include <compare>
#include "../Globals.hpp"

class Particle
{
public:
	sf::Vector2f position;
	sf::Vector2f oldPosition;
	sf::Vector2f velocity;
	sf::Vector2f acceleration;

	int id;

	bool active = true;

	std::unique_ptr<sf::CircleShape> shape;

	Particle();
	Particle(sf::Vector2f initialPosition, sf::Vector2f initialVelocity,
		sf::Vector2f initialAcceleration, int id);

	void draw(sf::RenderWindow& window) const;
	auto operator<=>(const Particle&) const = default;
	auto operator==(const Particle& other) const 
	{
		return id == other.id;
	};
};