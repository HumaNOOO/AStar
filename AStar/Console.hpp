#pragma once

#include "Graph.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <optional>

namespace
{
	using Command = std::tuple<std::string, std::function<void(std::optional<std::vector<std::string>>)>, bool>;
}

namespace astar
{
	struct Console
	{
		void operator=(const Console&) = delete;
		Console(const Console&) = delete;
		static Console& getInstance();
		void handleInput(const sf::Keyboard::Key key);
		void toggle();
		void addChar(const char c);
		bool isOpen() const;
		void resetCarriage(const sf::Vector2f& carriagePos);
		void draw(sf::RenderTarget& rt);
		void moveCarriage(const bool left);
	private:
		Console();
		constexpr static int carriageOffset_{ 10 };
		void executeCommand(std::string& command);
		bool consoleOpen_;
		size_t carriagePos_;
		sf::RectangleShape carriage_;
		std::string currentText_;
		std::vector<Command> callbacks_;
		std::vector<std::string> history_;
		sf::Font font_;
		sf::Text text_;
	};
}
