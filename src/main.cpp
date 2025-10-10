#include "Globals.hpp"
#include "classes/Simulation.hpp"
#include "classes/GUI/ImGuiWindow.hpp"
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

const sf::Vector2u windowSize{ Globals::getInstance().WINDOW_WIDTH, Globals::getInstance().WINDOW_HEIGHT };

int dragAreaSize = 50;

const sf::IntRect dragAreas[4] = {
	{{0, 0}, {static_cast<int>(windowSize.x), dragAreaSize }}, // Top
	{{0, dragAreaSize + 1}, {dragAreaSize, static_cast<int>(windowSize.y) -  2 * dragAreaSize - 2}}, // Left
	{{static_cast<int>(windowSize.x) - dragAreaSize, dragAreaSize}, {dragAreaSize, static_cast<int>(windowSize.y) - 2 * dragAreaSize - 2}}, // Right
	{{0, static_cast<int>(windowSize.y) - dragAreaSize }, {static_cast<int>(windowSize.x), static_cast<int>(windowSize.y)} } // Bottom
};

bool windowDrag = false;

int main()
{
	auto window = sf::RenderWindow(sf::VideoMode(windowSize), "Gravity Simulation");
	window.setFramerateLimit(60);

	if (!ImGui::SFML::Init(window))
		return -1;

	Simulation sim(windowSize);
	ImGuiWindow guiWindow(window, sim);


	sf::Clock clock;
	sf::Time dt{};

	auto mousePos = sf::Mouse::getPosition(window);

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			ImGuiIO& io = ImGui::GetIO();

			ImGui::SFML::ProcessEvent(window, *event);
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}

			if (event->is<sf::Event::Resized>())
			{
				auto eventValue = event->getIf<sf::Event::Resized>();
				window.setSize({ eventValue->size.x, eventValue->size.y });
			}

			if (event->is<sf::Event::MouseButtonPressed>())
			{
				auto mousePressed = event->getIf<sf::Event::MouseButtonPressed>();

				if (!io.WantCaptureMouse)
				{
					if (!windowDrag) sim.handleMouseEvent(window, *event);
				}

				if (mousePressed->button == sf::Mouse::Button::Right)
				{
					for (auto area : dragAreas)
					{
						if (area.contains(mousePressed->position))
						{
							windowDrag = true;
						}
					}
				}
			}

			if (event->is<sf::Event::MouseButtonReleased>())
			{
				auto mouseReleased = event->getIf<sf::Event::MouseButtonReleased>();

				if (mouseReleased->button == sf::Mouse::Button::Right)
				{
					windowDrag = false;
				}

				if (!io.WantCaptureMouse)
				{
					sim.handleMouseEvent(window, *event);
				}
			}

			if (windowDrag)
			{
				auto mouseDelta = sf::Mouse::getPosition(window) - mousePos;
				sim.physicsSolver.windowAcceleration = sf::Vector2f({ mouseDelta.x * sim.physicsSolver.Ga.y * guiWindow.windowDragSpeedMult,
																	  mouseDelta.y * sim.physicsSolver.Ga.y * guiWindow.windowDragSpeedMult });
			}

			if (event->is<sf::Event::KeyPressed>())
			{
				const auto keyPressed = event->getIf<sf::Event::KeyPressed>();
				sim.handleKeyboardPress(window, *keyPressed);

				if (keyPressed->code == sf::Keyboard::Key::Escape)
				{
					windowDrag = false;
				}
			}
		}

		mousePos = sf::Mouse::getPosition(window);

		dt = clock.restart();

		if (1.f / dt.asSeconds() < 30.f)
		{
			guiWindow.shouldShowWarning = true;
			sim.timeScale = 1;
		}
		else 
		{
			guiWindow.shouldShowWarning = false;
		}

		ImGui::SFML::Update(window, dt);
		guiWindow.ShowGUI();

		sim.update(dt.asSeconds());
		sim.physicsSolver.windowAcceleration = sf::Vector2f{};

		window.clear();
		sim.draw(window);

		for (auto area : dragAreas)
		{
			sf::RectangleShape rect{ static_cast<sf::Vector2f>(area.size) };
			rect.setPosition(static_cast<sf::Vector2f>(area.position));
			rect.setFillColor(sf::Color(0, 0, 0, 0));
			rect.setOutlineThickness(1);
			rect.setOutlineColor({ 200, 0, 0, 128 });
			window.draw(rect);
		}

		ImGui::SFML::Render(window);

		window.display();
	}

	ImGui::SFML::Shutdown();
	return 0;
}