#pragma once

#include <SFML/Graphics.hpp>
#include "rapidxml.hpp"

#include "manager.h"

class CManager;
class CAnimationComponent;

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

class CSpriteMapImageAsset : public CSpriteAsset
{
protected:
	sf::Texture* m_pTexture = NULL;
	sf::IntRect m_Rect;

public:
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode, CSpriteMap* pSpriteMap);
	virtual void update(sf::RenderWindow* pWindow);
};

// ----------------------------------------------------------------------------

class CSpriteMapAnimationAsset : public CSpriteMapImageAsset
{
public:
	int	m_nSteps;
	int	m_nTime;
	int	m_nStepIndex;
	int	m_nStepIndexFirstRow;
	sf::Clock m_clockTiming;
	CAnimationComponent* m_pParentGameObject = NULL;

public:
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode, CSpriteMap* pSpriteMap);
	virtual void update(sf::RenderWindow* pWindow);
};