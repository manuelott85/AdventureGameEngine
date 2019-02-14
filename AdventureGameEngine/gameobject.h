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
	std::string m_name = "";
	sf::Vector2f m_v2fPosition = { 0,0 };
	sf::Vector2f m_v2fScale = { 0,0 };
	float m_nRotation = NULL;
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
	sf::Vector2f m_v2fPosition = { 0,0 };
	sf::Vector2f m_v2fScale = { 0,0 };
	sf::Vector2f m_v2fOrigin = { 0,0 };
	float m_nRotation = NULL;
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
	bool m_bReversePlay = NULL;
	bool m_bMirrorX = NULL;
	bool m_bMirrorY = NULL;

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
	CGameObject* m_objectToMove = NULL;
	float m_moveSpeed = 0;
public:
	virtual void update(sf::RenderWindow* pWindow);
};

// ----------------------------------------------------------------------------

class CAnimationCtrl : public CComponent
{
public:
	sf::Vector2f m_v2fLastFramePos = { 0,0 };
	// storage for saving the "list of component" index to given animation
	int m_idleRightIndex = 0, m_idleLeftIndex = 0, m_moveRightIndex = 0, m_moveLeftIndex = 0, m_moveUpIndex = 0, m_moveDownIndex = 0;
public:
	virtual void update(sf::RenderWindow* pWindow);
private:
	void activateAnimationWithGivenIndex(int index);
};