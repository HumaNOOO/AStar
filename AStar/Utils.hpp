#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace astar::utils
{
	static float euclidDistance(const sf::Vector2f v1, const sf::Vector2f v2)
	{
		return std::sqrtf(std::powf(v1.x - v2.x, 2) + std::powf(v1.y - v2.y, 2));
	}

	static float getAngleDeg(const sf::Vector2f v1, const sf::Vector2f v2)
	{
		return std::atan2(v1.y - v2.y, v1.x - v2.x) * 57.30659025f;
	}
}