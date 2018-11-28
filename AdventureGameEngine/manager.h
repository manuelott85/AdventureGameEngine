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

class CScene
{
public:
	std::list<CGameObject*> m_GameObjects;	// list of current gameobjects
};

class CManager
{
public:
	std::string m_strAssetPath;		// general path to the assets
	std::list<CAsset*> m_Assets;	// list of all loaded assets in memory
	std::list<CScene*> m_Scenes;	// list of all loaded scene
	CScene* m_pActiveScene = NULL;	// points to current scene, that will be drawn
	CGameObject* m_pCursor;			// Pointer to the cursor object

private:
	CManager();	// making the constructor private prevent anyone of creating an instance of it, as it is a singleton class
	CAsset* getAssetOnName(std::string name);	// return a pointer to a loaded asset by its name

	void loadEveryAssetFromXML(rapidxml::xml_node<>* pRootNode);	// read the XML and load the assets into the memory
	void loadSprite(rapidxml::xml_node<>* pNode);
	void loadSpriteMap(rapidxml::xml_node<>* pNode);

	void createEverySceneFromXML(rapidxml::xml_node<>* pRootNode);	// create all scenes and store them
	void createEveryGameObjectFromXML(rapidxml::xml_node<>* pNode, CScene* pScene);	// create gameobjects according to the XML
	void createSpriteComponentFromXML(rapidxml::xml_node<>* pNode, CGameObject* pGameObject);	// create a sprite components to a given gameobject
	void createAnimationComponentFromXML(rapidxml::xml_node<>* pNode, CGameObject* pGameObject);	// create an animation components to a given gameobject
	void createSpriteComponentBasicData(rapidxml::xml_node<>* pNode, CComponent* pComponent);	// create an animation components to a given gameobject
	void createCursorComponent(rapidxml::xml_node<>* pNode, CGameObject* pGameObject); // create the cursor

public:
	~CManager();
	static CManager& instance();	// function to access the singleton instance
	void start(rapidxml::xml_node<>* pRootNode);
	void update(sf::RenderWindow* pWindow);
	void drawScene(sf::RenderWindow* pWindow);
};