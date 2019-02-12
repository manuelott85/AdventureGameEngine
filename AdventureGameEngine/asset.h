#pragma once

#include <SFML/Graphics.hpp>
#include "rapidxml.hpp"

#include "manager.h"

class CManager;
class CAnimationComponent;

// ----------------------------------------------------------------------------
// Base class for all assets (virtual)
class CAsset
{
protected: 
	std::string m_name;		// this name is used to to get a pointer to this asset out of list of all loaded assets in memory

public:
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode);	// empty (needs to be overridden by derived class)
	virtual void update(sf::RenderWindow* pWindow);							// empty (needs to be overridden by derived class)
	std::string getName() const;											// return m_name of the asset
	virtual sf::Sprite* getSprite();										// return sprite of the asset (needs to be overridden by derived class)
};

// ----------------------------------------------------------------------------
// Base class for all spriteMaps (virtual)
class CSpriteMap
{
protected:
	sf::Texture m_Texture;	// store the texture asset with all the sprites in it

public:
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode);
	sf::Texture* getTexture();	// return m_Texture
};

// ----------------------------------------------------------------------------

class CSpriteAsset : public CAsset
{
protected:
	sf::Texture m_Texture;	// stores the texture
	sf::Sprite m_Sprite;	// stores the sprite

public:
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode);	// Initialize the Asset (load texture and asign to sprite)
	virtual void update(sf::RenderWindow* pWindow);	// calls the drawing function
	virtual sf::Sprite* getSprite();	// return the sprite
};

// ----------------------------------------------------------------------------

class CSpriteMapImageAsset : public CSpriteAsset
{
protected:
	sf::Texture* m_pTexture = NULL;	// stores a pointer to a SpriteMapAsset's texture
	sf::IntRect m_Rect;	// defines the part of m_pTexture that should be cut out and serves as the "childTexture" to be assigned to a sprite

public:
	// Initialize the asset (get a texture reference, cut out the image from the texture, and assign it to the sprite)
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode, CSpriteMap* pSpriteMap);
	virtual void update(sf::RenderWindow* pWindow);	// calls the drawing function
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
	virtual void start(CManager *pManager, rapidxml::xml_node<>* pNode, CSpriteMap* pSpriteMap);	// Initialize the asset
	virtual void update(sf::RenderWindow* pWindow);	// update the "current frame to show" of the animation and apply it to the sprite
};