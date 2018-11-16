#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include <list>
#include <fstream>

#include "rapidxml.hpp"
#include "rapidXMLAdditions.h"

#include "manager.h"

int main()
{
	// rapidXML init
	std::ifstream tempFile("data.xml", std::ios::binary);
	std::vector<char> buffer((std::istreambuf_iterator<char>(tempFile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');
	tempFile.close();

	rapidxml::xml_document<> doc;
	doc.parse<0>(&buffer[0]);

	rapidxml::xml_node<>* pRootNode = doc.first_node();

	// Render Window
	rapidxml::xml_node<>* pTitle;
	pTitle = CRapidXMLAdditions::findChildNode(pRootNode, "title");
	sf::RenderWindow window(sf::VideoMode(1280, 720), "XML: TitleNode not readable");
	if (pTitle != NULL)
		window.setTitle(pTitle->value());

	// initiate the manager
	CManager::instance().start(pRootNode);

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
		CManager::instance().update(&window);
		window.display();
	}
	return 0;
}
