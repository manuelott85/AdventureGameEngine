#include <SFML/Graphics.hpp>

#include <iostream>
#include <list>
#include <fstream>
#include "rapidxml.hpp"
#include "rapidXMLAdditions.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

int main()
{
	// rapidXML init
	std::ifstream MyFile("data.xml", std::ios::binary);
	std::vector<char> buffer((std::istreambuf_iterator<char>(MyFile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');
	MyFile.close();

	rapidxml::xml_document<> Doc;
	Doc.parse<0>(&buffer[0]);

	rapidxml::xml_node<>* pRootNode = Doc.first_node();

	// Render Window
	rapidxml::xml_node<>* pTitle;
	pTitle = FindChildNode(pRootNode, "title");
	sf::RenderWindow window(sf::VideoMode(1280, 720), "XML: TitleNode not readable");
	if (pTitle != NULL)
		window.setTitle(pTitle->value());

	// main Game loop
	while (window.isOpen())
	{
		// Events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
		}

		// Draw Calls
		window.clear(sf::Color::Black);
		window.display();
	}
	return 0;
}
