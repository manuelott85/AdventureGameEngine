#pragma once

#include <string>
#include <SFML/Graphics.hpp>

#include "asset.h"

class CAsset;
class CSpriteAsset;
class CSpriteMapAnimationAsset;
class CComponent;
class CCursorComponent;

// ----------------------------------------------------------------------------

// The GameObject is just a container for any kind of components
// Itself only has a name, a transform and knows if it is active or not, and ensure that each components' update function gets called
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
	std::string getName();
};

// ----------------------------------------------------------------------------

// The CComponent is a base class for all components, it is quite similar to the gameobject, but instead of having a list of components
// it does only know the gameobject it is attached to. It has its own transform, to be able to offset it from the gameobject itself.
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
	bool m_bReversePlay;
	bool m_bMirrorX;
	bool m_bMirrorY;

public:
	virtual void update(sf::RenderWindow* pWindow);
};

// ----------------------------------------------------------------------------

class CCursorComponent : public CComponent
{
public:
	CSpriteAsset* m_pSpriteGeneric = NULL;
	CSpriteAsset* m_pSpriteHighlight = NULL;
	bool m_bCurrentAsset = 0;

public:
	virtual void update(sf::RenderWindow* pWindow);
};

// ----------------------------------------------------------------------------

class CMoveToTarget : public CComponent
{
public:
	sf::Clock m_clockTiming;
	sf::Vector2f m_lastOrderPos;
	sf::Vector2f m_lastFramePos;
	CGameObject* m_objectToMove = NULL;
public:
	virtual void update(sf::RenderWindow* pWindow);
};