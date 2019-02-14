#pragma once

#include <iostream>
#include <list>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "rapidxml.hpp"
#include "asset.h"
#include "gameobject.h"

class CAsset;
class CGameObject;
class CComponent;

// ----------------------------------------------------------------------------

class CScene
{
public:
	std::list<CGameObject*> m_GameObjects;	// list of current gameobjects
	CGameObject* m_player = NULL;	// a reference to the player's gameobject
	CGameObject* m_playerMoveToTarget = NULL;	// a reference to the player's moving gameobject
};

// ----------------------------------------------------------------------------

class CManager
{
public:
	std::string m_strAssetPath;		// general path to the assets
	std::list<CAsset*> m_Assets;	// list of all loaded assets in memory
	std::list<CScene*> m_Scenes;	// list of all loaded scenes
	CScene* m_pActiveScene = NULL;	// points to current scene that will be drawn
	CGameObject* m_pCursor;			// Pointer to the cursor object
	sf::Clock m_managerClock;		// clock to count the frameDelta
	float m_deltaTime;				// last frame's delta

public:
	~CManager();
	static CManager& instance();	// function to access the singleton instance
	void start(rapidxml::xml_node<>* pRootNode);
	void update(sf::RenderWindow* pWindow);
	void drawScene(sf::RenderWindow* pWindow);

private:
	CManager();	// making the constructor private prevent anyone of creating an instance of it, as it is a singleton class
	CAsset* getAssetOnName(std::string name);	// return a pointer to a loaded asset by its name

	void loadEveryAssetFromXML(rapidxml::xml_node<>* pRootNode);	// read the XML and load the assets into the memory
	void loadSprite(rapidxml::xml_node<>* pNode);	// Create and initialize a sprite asset
	void loadSpriteMap(rapidxml::xml_node<>* pNode);	// Create and initialize a spriteMap asset
	void loadAudio(rapidxml::xml_node<>* pNode);	// Create and initialize an audio asset

	void createEverySceneFromXML(rapidxml::xml_node<>* pRootNode);	// create all scenes and store them
	void createEveryGameObjectFromXML(rapidxml::xml_node<>* pNode, CScene* pScene);	// create gameobjects of a scene according to the XML
	void createSpriteComponentFromXML(rapidxml::xml_node<>* pNode, CGameObject* pGameObject);	// create a sprite components to a given gameobject
	void createAnimationComponentFromXML(rapidxml::xml_node<>* pNode, CGameObject* pGameObject);	// create an animation components to a given gameobject
	void createSpriteComponentBasicData(rapidxml::xml_node<>* pNode, CComponent* pComponent);	// read out basic parameter and store them in the component
	void createCursorComponent(rapidxml::xml_node<>* pNode, CGameObject* pGameObject); // create the cursor components
	void createMoveToTargetComponent(rapidxml::xml_node<>* pNode, CGameObject* pGameObject); // create the moveToTarget components
	void createAnimationCtrlComponent(rapidxml::xml_node<>* pNode, CGameObject* pGameObject); // create the Animation Controller

	void setReferences();	// set all available references
};