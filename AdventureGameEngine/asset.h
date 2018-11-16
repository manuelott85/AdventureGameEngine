#pragma once

#include <SFML/Graphics.hpp>
#include "rapidxml.hpp"

#include "manager.h"

class CManager;

class CAsset
{
protected: 
	std::string m_name;

public:
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode);
	virtual void update(sf::RenderWindow* pWindow);
	std::string getName() const;
};

// ----------------------------------------------------------------------------

class CSprite : public CAsset
{
protected:
	sf::Texture m_Texture;
	sf::Sprite m_Sprite;

public:
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode);
	virtual void update(sf::RenderWindow* pWindow);
};

// ----------------------------------------------------------------------------

class CSpriteMap
{
protected:
	sf::Texture m_Texture;

public:
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode);
	sf::Texture* getTexture();
};

// ----------------------------------------------------------------------------

class CSpriteMapImage : public CAsset
{
protected:
	sf::Texture* m_pTexture;
	sf::Sprite m_Sprite;
	sf::IntRect m_Rect;

public:
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode, CSpriteMap* pSpriteMap);
	virtual void update(sf::RenderWindow* pWindow);
};