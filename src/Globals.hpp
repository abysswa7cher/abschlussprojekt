#pragma once
#include <compare>


// singleton helper class to store global constants
struct Globals {
	unsigned int WINDOW_WIDTH = 1920;
	unsigned int WINDOW_HEIGHT = 1080;
	float particleRadius = 10.f;

	static Globals& getInstance() {
		static Globals instance;
		return instance;
	}
	auto operator<=>(const Globals&) const = default;

private:
	Globals() = default;
	~Globals() = default;
	Globals(const Globals&) = delete;
	Globals& operator=(const Globals&) = delete;
};
