#pragma once

#include <string>
#include <SFML/Graphics.hpp>

#include "asset.h"

class CAsset;
class CSpriteAsset;
class CSpriteMapAnimationAsset;
class CComponent;

// ----------------------------------------------------------------------------

class CGameObject
{
public:
	std::string m_name;
	sf::Vector2f m_v2fPosition;
	sf::Vector2f m_v2fScale;
	float m_nRotation;
	bool m_bEnabled = true;
	std::list<CComponent*> m_components;

public:
	void update(sf::RenderWindow* pWindow);
};

// ----------------------------------------------------------------------------

class CComponent
{
public:
	sf::Vector2f m_v2fPosition;
	sf::Vector2f m_v2fScale;
	sf::Vector2f m_v2fOrigin;
	float m_nRotation;
	CGameObject* m_pParentGameObject = NULL;
	bool m_bEnabled = true;

public:
	virtual void update(sf::RenderWindow* pWindow);
};

// ----------------------------------------------------------------------------

class CSpriteComponent : public CComponent
{
public:
	CSpriteAsset* m_pAsset = NULL;

public:
	virtual void update(sf::RenderWindow* pWindow);
};

// ----------------------------------------------------------------------------

class CAnimationComponent : public CSpriteComponent
{
public:
	CSpriteMapAnimationAsset* m_pAsset = NULL;

public:
	bool	m_bReversePlay;
	bool	m_bMirrorX;
	bool	m_bMirrorY;

public:
	virtual void update(sf::RenderWindow* pWindow);
};