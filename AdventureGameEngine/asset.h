#pragma once

#include <SFML/Graphics.hpp>
#include "manager.h"

class CAsset
{
public:
	CAsset();

	virtual void Start(CManager *pManager, rapidxml::xml_node<>* pNode);
	virtual void Update(sf::RenderWindow* pWindow);
	virtual int GetName() const;
};

class CSprite : public CAsset
{
public:
	CSprite();

	virtual void Start(CManager *pManager, rapidxml::xml_node<>* pNode);
	virtual void Update(sf::RenderWindow* pWindow);
	virtual int GetName() const;
};