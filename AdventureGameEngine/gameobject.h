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
class CFontAsset;
class CTextbox;
class CSequenceAction;
class CAudioAsset;

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
	std::list<CInteractionComponent*> m_interactionComponents;
	std::list<CTextbox*> m_pTextComponents;

public:
	void update(sf::RenderWindow* pWindow);
	std::string getName();
};

// ----------------------------------------------------------------------------

// This interface will process the inputs. Each action that is performed is depending on the specific component
// I may not understand interfaces completely. Cause I could have defined the only function directly in the CComponent itself,
// as it is the only one using this interface
class CInteractionInterface
{
public:
	virtual bool performAction(bool leftMouseBtnWasUsed) = 0;
};

// ----------------------------------------------------------------------------

// The CComponent is a base class for all components, it is quite similar to the gameobject, but instead of having a list of components
// it does only know the gameobject it is attached to. It has its own transform, to be able to offset it from the gameobject itself.
// it also enables each component to process inputs if so desired
class CComponent : public CInteractionInterface
{
public:
	std::string m_name = "";
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
	virtual bool performAction(bool leftMouseBtnWasUsed) { return false; };	// is overriden by each component if needed
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

enum eCursorSprite
{
	generic, highlight, load
};

class CCursorComponent : public CComponent
{
public:
	CSpriteAsset* m_pSpriteGeneric = NULL;
	CSpriteAsset* m_pSpriteHighlight = NULL;
	CSpriteAsset* m_pSpriteLoading = NULL;

public:
	virtual void update(sf::RenderWindow* pWindow);
	void switchAppearance(eCursorSprite cursorToShow = eCursorSprite::generic);
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
	int m_idleHeadIndex = 0, m_talkToThePlayerIndex = 0, m_talkIntoTheImageIndex = 0, m_talkRightIndex = 0, m_talkLeftIndex = 0;
public:
	virtual void update(sf::RenderWindow* pWindow);
private:
	void activateAnimationWithGivenIndex(int index, bool bShowHeadStill = false, bool bShowHeadTalking = false);	// enable the object with given index and deactivate every one else, also scale accordingly to the y-axis and a factor
};

// ----------------------------------------------------------------------------

class CInteractionComponent : public CComponent
{
private:
	bool m_bStillPerformingAction = false;	// prevent additional calls and perform the full action in a sequence
	bool m_bLeftMouseBtnWasUsed = true;	// store the last mouse button pressed to perform actions
	sf::FloatRect m_boundingBox;	// collision dimension of current asset

public:
	std::string m_type = "";		// store the type of interaction performed with the left mouse button
	std::string m_neededGameObject = "";	// store the name of the gameobject that is needed to perform the action successfully
	std::list<std::string> m_pComponentsToEnable;		// store the name of components to ENABLE by performing the use action
	std::list<std::string> m_pComponentsToDisable;	// store the name of components to DISABLE by performing the use action
	std::list<std::string> m_pGameObjectsToEnable;		// store the name of GameObjects to ENABLE by performing the use action
	std::list<std::string> m_pGameObjectsToDisable;	// store the name of GameObjects to DISABLE by performing the use action
	CAudioAsset* m_pAudioFile;
	sf::Sound m_sound;	// the audio component to play if the action gets triggered

public:
	virtual void update(sf::RenderWindow* pWindow);
	bool checkCollisionPoint(sf::Vector2f point);	// Check if a given point is within the assets limits
	bool checkCollisionBoundingBox(sf::FloatRect otherBox);	// Check if another bounding box collides with the assets limits
	bool processMouseButton(sf::Vector2f mousePos, bool leftMouseBtnWasUsed = true);	// call the different components depending on button and interaction type, will return true if the interaction component "used/consumes" that call
	void performTask(bool leftMouseBtnWasUsed);
};

// ----------------------------------------------------------------------------

class CTextComponent : public CComponent
{
public:
	sf::Text m_text;	// the text object itself
	float m_lifetime = 2;
	unsigned int m_charSize = 30;
	sf::Color m_colorPrimary = sf::Color::White;
	sf::Color m_colorSecondary = sf::Color::Black;

public:
	virtual void update(sf::RenderWindow* pWindow);
	virtual bool performAction(bool leftMouseBtnWasUsed);
};

// ----------------------------------------------------------------------------

class CTextbox : public CComponent
{
private:
	sf::Clock timer;	// to measure the lifetime of the text
	bool m_bIsPrimary = true;

public:
	sf::Text m_text;	// the text object itself
	float m_lifetime = 2;

public:
	virtual void update(sf::RenderWindow* pWindow);
	void showText(const sf::String& text, float lifetimeInSec, const sf::Font* pFontAsset, unsigned int charSize, bool bIsPrimary, sf::Color color);
};

// ----------------------------------------------------------------------------
class CAudioComponent : public CComponent
{
public:
	CAudioAsset* m_pAudioFile;
	sf::Sound m_sound;	// the audio component to play if the action gets triggered

public:
	virtual void update(sf::RenderWindow* pWindow);
};

// ----------------------------------------------------------------------------
// This is the container for a sequence; if enabled it will disable the input and play through every action step by step until it deactivates itself again
class CSequenceComponent : public CComponent
{
public:
	std::list<CSequenceAction*> m_pActions;

public:
	CSequenceComponent();
	void update(sf::RenderWindow* pWindow);
};

// ----------------------------------------------------------------------------

enum eActionType
{
	nothing, move, say, audio
};

// ----------------------------------------------------------------------------

class CSequenceAction
{
public:
	eActionType m_type = eActionType::nothing;	// which action should be performed
	CGameObject* m_targetObject = NULL;			// object that performs the action
	sf::Vector2f m_moveToVector = { 0,0 };		// in case of a move action, this is the target vector
	sf::Font* m_font = NULL;						// in case of a say action, this is the font to use
	float m_lifetime = 0;						// in case of a say action, this is the lifetime of the text
	unsigned int m_charSize = 30;
	std::string m_text = "";					// in case of a say action, this is the text
	bool m_bFinished = false;					// should be set to true if the action has completed
	sf::Color m_colorPrimary = sf::Color::White;
	sf::Color m_colorSecondary = sf::Color::Black;
	CAudioAsset* m_pAudioFile;
	sf::Sound m_sound;	// the audio component to play if the action gets triggered

public:
	void update(sf::RenderWindow* pWindow);
};