#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>

#include "asset.h"

class CAsset;
class CSpriteAsset;
class CSpriteMapAnimationAsset;
class CComponent;
class CCursorComponent;
class CInteractionComponent;

// ----------------------------------------------------------------------------

// The GameObject is just a container for any kind of components
// Itself only has a name, a transform and knows if it is active or not, and ensure that each components' update function gets called
class CGameObject
{
public:
	std::string m_name = "";
	sf::Vector2f m_v2fPosition = { 0,0 };
	sf::Vector2f m_v2fScale = { 1,1 };
	float m_nRotation = NULL;
	bool m_bEnabled = true;
	std::list<CComponent*> m_components;
	CInteractionComponent* m_interactionComponent = NULL;

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
	sf::Vector2f m_v2fScale = { 1,1 };
	sf::Vector2f m_v2fOrigin = { 0,0 };
	float m_nRotation = NULL;
	CGameObject* m_pParentGameObject = NULL;
	bool m_bEnabled = true;
	bool m_drawLate = false;

public:
	virtual void update(sf::RenderWindow* pWindow);
	virtual sf::Sprite* getSprite();
};

// ----------------------------------------------------------------------------

class CSpriteComponent : public CComponent
{
public:
	CSpriteAsset* m_pAsset = NULL;

public:
	virtual void update(sf::RenderWindow* pWindow);
	virtual sf::Sprite* getSprite();
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

public:
	virtual void update(sf::RenderWindow* pWindow);
	void switchAppearance(bool showHightlichtCursor = false);
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
	void activateAnimationWithGivenIndex(int index);	// enable the object with given index and deactivate every one else, also scale accordingly to the y-axis and a factor
};

// ----------------------------------------------------------------------------

class CInteractionComponent : public CComponent
{
private:
	sf::FloatRect m_boundingBox;	// collision dimension of current asset
public:
	virtual void update(sf::RenderWindow* pWindow);
	bool checkCollisionPoint(sf::Vector2f point);	// Check if a given point is within the assets limits
	bool checkCollisionBoundingBox(sf::FloatRect otherBox);	// Check if another bounding box collides with the assets limits
	bool processMouseButton(sf::Vector2f mousePos, bool leftMouseBtnWasUsed = true);	// call the different components depending on button and interaction type, will return true if the interaction component "used/consumes" that call
};

// ----------------------------------------------------------------------------

class CDescriptionComponent : public CComponent
{
private:
	float m_fontSizePreScaling;	// save the original fontsize for scaling calculations

public:
	sf::Text m_descriptionText;	// the text object itself

public:
	virtual void update(sf::RenderWindow* pWindow);
};