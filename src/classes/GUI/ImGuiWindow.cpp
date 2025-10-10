#include "ImGuiWindow.hpp"

void ImGuiWindow::ShowGUI()
{
	ImGui::Begin("Simulation Controls");
	ImGui::Separator();

	ImGui::Text(sim.running ? "Running" : "Paused");
	if (ImGui::Button("Pause/Resume"))
	{
		sim.togglePause();
	}
	ImGui::SliderInt("Time Scale", &sim.timeScale, 1, 100);
	ImGui::Separator();

	ImGui::Text(sim.addingParticleSpray ? "Spray: Active" : "Spray: Inactive");
	if (ImGui::Button("Spray Bodies"))
	{
		sim.addingParticleSpray = !sim.addingParticleSpray;
	}
	ImGui::SliderFloat("Spray Radius", &sim.sprayRadius, 10.f, 500.f);
	ImGui::SliderFloat("Particle Radius", &Globals::getInstance().particleRadius, 1.f, 50.f);
	ImGui::Separator();

	if (ImGui::Button("Clear Simulation"))
	{
		sim.clearSim();
	}
	ImGui::Separator();
	if (ImGui::Button("Show Body Info"))
	{
		showBodyInfoPopup = !showBodyInfoPopup;
	}

	ImGui::Separator();

	ImGui::SliderFloat("Window Drag Speed", &windowDragSpeedMult, 0.1f, 1.f);

	if (ImGui::Button("Simulation Settings"))
	{
		showSimulationSettings = !showSimulationSettings;
	}

	ImGui::Separator();
	ImGui::Text("Anzahl der Teilchen: %d", sim.getBodies().size());

	ImGui::End();

	if (showBodyInfoPopup)
	{
		ImGui::Begin("Bodies:");
		for (auto& b : sim.getBodies())
		{
			ImGui::Text("Body %d", b.id);
			ImGui::Text("Position: (%.1f, %.1f)", b.position.x, b.position.y);
		}
		ImGui::End();
	}

	if (showSimulationSettings)
	{
		ImGui::Begin("Simulation Settings", &showSimulationSettings, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Adjust global simulation settings here.");
		ImGui::Separator();

		ImGui::SliderInt("Resolution of the collision grid", &sim.physicsSolver.simGridResolution, 10, 200);
		ImGui::SliderInt("Solver Iterations / Frame", &sim.physicsSolver.solverIterations, 1, 10);
		ImGui::Separator();

		ImGui::SliderFloat("Gravity", &sim.physicsSolver.Ga.y, -100.f, 100.f);
		ImGui::SliderFloat("Restitution Coefficient", &sim.physicsSolver.Cr, 0.f, 1.f);
		ImGui::SliderFloat("Friction Coefficient", &sim.physicsSolver.u_friction, 0.f, 1.f);
		ImGui::SliderFloat("Time Substep (physics_dt)", &sim.physics_dt, 0.1f, 0.01f, "%.3f");

		ImGui::End();
	}

	if (shouldShowWarning)
	{
		ImGui::Begin("Warning");
		ImGui::Text("Low FPS detected! Time Scale reset to 1.");
		ImGui::End();
	}
}