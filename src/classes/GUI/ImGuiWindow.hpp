#pragma once
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "../Simulation.hpp"

class ImGuiWindow
{
	Simulation& sim;
	sf::RenderWindow& window;
	bool showBodyInfoPopup = false;
	bool showSimulationSettings = false;

public:
	float windowDragSpeedMult = 0.5f;
	bool shouldShowWarning = false;

	ImGuiWindow(sf::RenderWindow& window, Simulation& sim)
		:sim(sim)
		, window(window)
	{
	};

	void ShowGUI();
};
