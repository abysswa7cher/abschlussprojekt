#include "Particle.hpp"

Particle::Particle()
{
	position = {};
	oldPosition = {};
	velocity = {};
	acceleration = {};
	id = -1;
	shape = std::make_unique<sf::CircleShape>();
}

Particle::Particle(sf::Vector2f initialPosition, sf::Vector2f initialVelocity,
	sf::Vector2f initialAcceleration, int id)
	: position(initialPosition)
	, velocity(initialVelocity)
	, acceleration(initialAcceleration)
	, id(id)
{
	shape = std::make_unique<sf::CircleShape>();
	shape->setRadius(Globals::getInstance().particleRadius);
	shape->setFillColor({ 255, 255, 255 });
}

void Particle::draw(sf::RenderWindow& window) const
{
	shape->setPosition(position);
	shape->setOrigin({ shape->getRadius(), shape->getRadius() });
	shape->setFillColor({ static_cast<uint8_t>(std::max(std::clamp(velocity.length(), 0.f, 255.f), 25.f)), static_cast<uint8_t>(std::max(std::clamp(velocity.length(), 0.f, 255.f), 25.f)), 128});
	window.draw(*shape);

}