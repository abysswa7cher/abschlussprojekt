#include "Simulation.hpp"
#include "PhysicsSolver.hpp"

Simulation::Simulation(const sf::Vector2u& windowSize)
{
	std::random_device dev;
	rng = std::mt19937(dev());
}

void Simulation::addParticle(sf::Vector2f initialPosition, sf::Vector2f initialVelocity,
	sf::Vector2f initialAcceleration)
{
	bodies.push_back({ initialPosition, initialVelocity, initialAcceleration, body_id++ });
}

void Simulation::update(float dt)
{
	if (running)
	{
		currentMousePosition = sf::Mouse::getPosition();
		if (addingParticleSpray && addParticleSprayActive)
		{
			addParticleSpray(static_cast<sf::Vector2f>(currentMousePosition));
		}

		time_accumulator += dt * timeScale;
		while (time_accumulator >= physics_dt)
		{
			// Step 1: Predict next position, ignoring collisions for now
			physicsSolver.preupdatePositions(bodies, physics_dt);

			// Step 2: Run the collision solver iterations
			physicsSolver.resolveCollisions(bodies, physics_dt);

			// Step 3: Update final velocity based on the corrected position
			physicsSolver.updatePositions(bodies, physics_dt);

			// Step 4: Sync the final, corrected position for drawing
			for (auto& body : bodies)
			{
				body.shape->setPosition(static_cast<sf::Vector2f>(body.position));
			}

			cleanupBodies();
			time_accumulator -= physics_dt;
		}
	}
}

void Simulation::draw(sf::RenderWindow& window)
{
	for (int i = 0; i < globals.WINDOW_WIDTH / physicsSolver.simGridResolution + 1; i++)
	{
		float x = i * static_cast<float>(physicsSolver.simGridResolution);
		sf::Vertex line_v[] =
		{
			sf::Vertex(sf::Vector2f(x, 0), sf::Color(50, 50, 50)),
			sf::Vertex(sf::Vector2f(x, static_cast<float>(globals.WINDOW_HEIGHT)), sf::Color(50, 50, 50))
		};
		window.draw(line_v, 2, sf::PrimitiveType::Lines);
	}

	for (int i = 0; i < globals.WINDOW_HEIGHT / physicsSolver.simGridResolution + 1; i++)
	{
		float y = i * static_cast<float>(physicsSolver.simGridResolution);
		sf::Vertex line_h[] =
		{
			sf::Vertex(sf::Vector2f(0, y), sf::Color(50, 50, 50)),
			sf::Vertex(sf::Vector2f(static_cast<float>(globals.WINDOW_WIDTH), y), sf::Color(50, 50, 50))
		};
		window.draw(line_h, 2, sf::PrimitiveType::Lines);
	}

	for (const auto& b : bodies)
	{
		b.draw(window);
	}

	if (addingParticleSpray)
	{
		auto mousePos = sf::Mouse::getPosition(window);

		sf::CircleShape cursorCircle = sf::CircleShape{ sprayRadius, 16 };
		cursorCircle.setFillColor({ 0, 0, 0, 0 });
		cursorCircle.setOutlineThickness(1);
		cursorCircle.setOutlineColor({ 128, 128, 0, 128 });
		cursorCircle.setOrigin({ sprayRadius, sprayRadius });
		cursorCircle.setPosition(static_cast<sf::Vector2f>(mousePos));

		window.draw(cursorCircle);
	}
}

void Simulation::clearSim()
{
	bodies.clear();
	body_id = 0;
}

void Simulation::cleanupBodies()
{
	bodies.erase(std::remove_if(bodies.begin(), bodies.end(), [](const Particle& b) { return !b.active; }),
		bodies.end());
}

std::vector<Particle>& Simulation::getBodies()
{
	return bodies;
}

void Simulation::handleMouseEvent(sf::RenderWindow& window, const sf::Event& event)
{
	if (auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
	{
		auto mousePos = mousePressed->position;
		auto mouseBtn = mousePressed->button;

		switch (mouseBtn)
		{
		case sf::Mouse::Button::Left:
			addParticleSprayActive = true;
			break;
		}
	}

	if (auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>())
	{
		auto mousePos = mouseReleased->position;
		auto mouseBtn = mouseReleased->button;

		switch (mouseBtn)
		{
		case sf::Mouse::Button::Left:
			addParticleSprayActive = false;
			break;
		}
	}

}

void Simulation::handleKeyboardPress(sf::RenderWindow& window, sf::Event::KeyPressed keyPressed)
{
	switch (keyPressed.code)
	{
	case sf::Keyboard::Key::Space:
		togglePause();
		break;
	case sf::Keyboard::Key::Escape:
		addingParticleSpray = false;
		break;
	default:
		break;
	}
}

void Simulation::togglePause()
{
	running = !running;
}

void Simulation::addParticleSpray(sf::Vector2f mouseWorldPos)
{
	auto intDist = std::uniform_int_distribution<int>(1, static_cast<int>(sprayRadius) / 10);
	auto floatDist = std::uniform_real_distribution<float>(-sprayRadius, sprayRadius);

	int n = intDist(rng);

	bodies.reserve(bodies.size() + n);

	for (int i = 0; i < n; i++)
	{
		sf::Vector2f  pos = mouseWorldPos + sf::Vector2f{ floatDist(rng), floatDist(rng) };
		sf::Vector2f  vel = { floatDist(rng), floatDist(rng) };
		bodies.emplace_back(Particle(pos, vel, {}, body_id++));
	}
}