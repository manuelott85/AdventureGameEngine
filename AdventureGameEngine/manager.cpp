#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "rapidxml.hpp"

#include "manager.h"
#include "rapidXMLAdditions.h"
#include "asset.h"
#include "gameobject.h"

CManager::CManager()
{

}

CManager& CManager::instance()
{
	static CManager* instance = new CManager();
	return *instance;
}

CAsset* CManager::getAssetOnName(std::string name)
{
	for (std::list<CAsset*>::iterator it = m_Assets.begin(); it != m_Assets.end(); ++it)
	{
		if ((*it)->getName() == name)
		{
			return *it;
		}
	}

	return NULL;
}

void CManager::start(rapidxml::xml_node<>* pRootNode)
{
	loadEveryAssetFromXML(pRootNode);
	createEveryGameObjectFromXML(pRootNode);
}

void CManager::update(sf::RenderWindow* pWindow)
{
	drawScene(pWindow);
}

void CManager::loadEveryAssetFromXML(rapidxml::xml_node<>* pRootNode)
{
	// temporary pointer
	rapidxml::xml_node<>* pAssets;
	rapidxml::xml_attribute<>* pAttr;

	// Create an entity for every asset in the XML
	pAssets = CRapidXMLAdditions::findChildNode(pRootNode, "assets");
	if (pAssets != NULL)
	{
		// Save the path into the manager
		CManager::instance().m_strAssetPath = CRapidXMLAdditions::getAttributeValue(pAssets, "path");

		// read out every type asset and create it
		for (rapidxml::xml_node<>* pChild = pAssets->first_node(); pChild != NULL; pChild = pChild->next_sibling())
		{
			loadSprite(pChild);	// if asset is of type sprite
			loadSpriteMap(pChild);	// if asset is of type spriteMap
		}
	}
}

void CManager::loadSprite(rapidxml::xml_node<>* pNode)
{
	// e.g. <sprite name="street">cyberpunk-street.png</sprite>
	if (strcmp(pNode->name(), "sprite") == 0)
	{
		CSprite *pSprite = new CSprite();		// create the asset's object
		pSprite->start(this, pNode);			// call its start function
		m_Assets.push_back(pSprite);			// make it available in the manager
	}
}

void CManager::loadSpriteMap(rapidxml::xml_node<>* pNode)
{
	// e.g. <spritemap src="cursors.png">
	if (strcmp(pNode->name(), "spritemap") == 0)
	{
		CSpriteMap *pSpriteMap = new CSpriteMap();			// create the asset's object
		pSpriteMap->start(this, pNode);				// call its start function

		for (rapidxml::xml_node<>* pChild = pNode->first_node(); pChild != NULL; pChild = pChild->next_sibling())
		{
			// if static image
			// e.g. <sprite name="cursor-generic" xOffset="0" yOffset="0" width="356" height="150"/>
			if (strcmp(pChild->name(), "sprite") == 0)
			{
				CSpriteMapImage *pSprite = new CSpriteMapImage();			// create the asset's object
				pSprite->start(this, pChild, pSpriteMap);	// call its start function
				m_Assets.push_back(pSprite);			// make it available in the manager
				int i = 5;
			}

			//// if animation
			//// e.g. <animation name="8" xOffset="200" yOffset="0" width="50" height="80" steps="8" time="200"/>
			//if (strcmp(pChild->name(), "animation") == 0)
			//{
			//	CAnimation *pImage = new CAnimation();			// create the asset's object
			//	pImage->Start(&manager, pImagemap, pChild);		// call its start function
			//	manager.m_Entities.push_back(pImage);	// make it available in the manager
			//}
		}
	}
}

void CManager::createEveryGameObjectFromXML(rapidxml::xml_node<>* pRootNode)
{
	// Get the Scene
	rapidxml::xml_node<>* pScene = CRapidXMLAdditions::findChildNode(pRootNode, "scene");
	if (pScene == NULL)
		return;

	// Create all the gameobjects defined in the scene
	for (rapidxml::xml_node<>* pNodeGameObject = pScene->first_node(); pNodeGameObject != NULL; pNodeGameObject = pNodeGameObject->next_sibling())
	{
		// e.g. <gameobject name="background" posX="0" posY="0" scaleX="1" scaleY="1" rotation="0">
		if (strcmp(pNodeGameObject->name(), "gameobject") == 0)
		{
			CGameObject* pGameObject = new CGameObject();	// create a gameobject
			m_GameObjects.push_back(pGameObject);	// add it to the manager's list

			// Store the name of the object
			pGameObject->m_name = CRapidXMLAdditions::getAttributeValue(pNodeGameObject, "name");

			// Store the position of the object
			pGameObject->m_v2fPosition = sf::Vector2f(
				(float)atof(CRapidXMLAdditions::getAttributeValue(pNodeGameObject, "posX")),
				(float)atof(CRapidXMLAdditions::getAttributeValue(pNodeGameObject, "posY")));

			// Store the scale
			pGameObject->m_v2fScale = sf::Vector2f(
				(float)atof(CRapidXMLAdditions::getAttributeValue(pNodeGameObject, "scaleX")),
				(float)atof(CRapidXMLAdditions::getAttributeValue(pNodeGameObject, "scaleY")));

			// Store rotation
			pGameObject->m_nRotation = (float)atof(CRapidXMLAdditions::getAttributeValue(pNodeGameObject, "rotation"));

			// create all its components
			for (rapidxml::xml_node<>* pNodeComponent = pNodeGameObject->first_node(); pNodeComponent != NULL; pNodeComponent = pNodeComponent->next_sibling())
			{
				createSpriteComponentFromXML(pNodeComponent, pGameObject);	// create the sprite component
			}
		}
	}
}

void CManager::createSpriteComponentFromXML(rapidxml::xml_node<>* pNode, CGameObject* pGameObject)
{
	// e.g. <sprite load = "street" posX = "0" posY = "0" scaleX = "4" scaleY = "4" rotation = "0" originX = "0.5" originY = "0.5" / >
	if (strcmp(pNode->name(), "sprite") == 0)
	{
		std::string assetNameToLoad = CRapidXMLAdditions::getAttributeValue(pNode, "load");	// get the name of the asset to load
		pGameObject->m_pAsset = getAssetOnName(assetNameToLoad);	// assign the pointer to the asset
	}
}

void CManager::drawScene(sf::RenderWindow* pWindow)
{
	for (std::list<CGameObject*>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); ++it)
	{
		(*it)->update(pWindow);
	}
}