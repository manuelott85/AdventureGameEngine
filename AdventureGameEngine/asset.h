#pragma once

#include <SFML/Graphics.hpp>
#include "rapidxml.hpp"

#include "manager.h"

class CManager;

// ----------------------------------------------------------------------------

class CAsset
{
protected: 
	std::string m_name;

public:
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode);
	virtual void update(sf::RenderWindow* pWindow);
	std::string getName() const;
	virtual sf::Sprite* getSprite();
};

// ----------------------------------------------------------------------------

class CSpriteAsset : public CAsset
{
protected:
	sf::Texture m_Texture;
	sf::Sprite m_Sprite;

public:
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode);
	virtual void update(sf::RenderWindow* pWindow);
	virtual sf::Sprite* getSprite();
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

class CSpriteMapImageAsset : public CAsset
{
protected:
	sf::Texture* m_pTexture = NULL;
	sf::Sprite m_Sprite;
	sf::IntRect m_Rect;
	virtual sf::Sprite* getSprite();

public:
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode, CSpriteMap* pSpriteMap);
	virtual void update(sf::RenderWindow* pWindow);
};