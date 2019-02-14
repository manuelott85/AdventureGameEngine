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
	sf::RenderWindow window(sf::VideoMode(1786, 992), "XML: TitleNode not readable");
	if (pTitle != NULL)
		window.setTitle(pTitle->value());

	CManager::instance().start(pRootNode);	// initiate the manager
	//window.setMouseCursorVisible(false);	// hide the windows mouse cursor

	// main Game loop
	while (window.isOpen())
	{
		// Events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();	// close the programm, when window is closed

			// Check for a mouse button
			if (event.type == sf::Event::MouseButtonPressed)
			{
				// check for left mouse button
				if (event.mouseButton.button == sf::Mouse::Button::Left)
				{
					// move the player's character to that location
					CManager::instance().m_pActiveScene->m_playerMoveToTarget->m_v2fPosition = (sf::Vector2f)sf::Mouse::getPosition(window);
				}
			}
		}

		// Draw Calls
		window.clear(sf::Color::Black);	// remove the last frame
		CManager::instance().update(&window);	// call the update function which includes drawing
		window.display();	// show the current frame
	}
	return 0;
}