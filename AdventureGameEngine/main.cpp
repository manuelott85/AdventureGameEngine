#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include <list>
#include <fstream>

#include "rapidxml.hpp"
#include "rapidXMLAdditions.h"
//#include "interpolation.h"

#include "manager.h"

//void interTemp(sf::Vector2f startVector, sf::Vector2f endVector)
//{
//	//magnitude
//	sf::Vector2f direction = (endVector - startVector);
//	float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);
//
//	std::vector<double> xData = { startVector.x, endVector.x };
//	std::vector<double> yData = { startVector.y, endVector.y };
//	float speed = 2;
//
//	sf::Clock clock;
//	clock.getElapsedTime();
//	
//	//xData.
//
//	// Set up some points for interpolation in xVals
//	const int NPTS = xData.at(1);
//	std::vector<double> xVals, yVals;
//	for (float i = xData.at(0); i <= NPTS; i = i + speed)
//		xVals.push_back((double)i);
//
//	// Interpolate
//	for (double x : xVals)
//	{
//		double y = interpolate(xData, yData, x, true);
//		yVals.push_back(y);
//	}
//
//	// Output
//#define SP << std::fixed << setw( 15 ) << setprecision( 6 ) <<
//	std::cout << "Original data:\n";
//	for (int i = 0; i < xData.size(); i++) std::cout SP xData[i] SP yData[i] << '\n';
//	std::cout << "\nInterpolated data:\n";
//	for (int i = 0; i < xVals.size(); i++) std::cout SP xVals[i] SP yVals[i] << '\n';
//}

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

	//interTemp({ 10,10 }, { 100, 400 });

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