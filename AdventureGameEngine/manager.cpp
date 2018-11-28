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

CManager::~CManager()
{
	if (m_pCursor != NULL)
		delete(m_pCursor);
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
	createEverySceneFromXML(pRootNode);
}

void CManager::update(sf::RenderWindow* pWindow)
{
	drawScene(pWindow);
}

void CManager::loadEveryAssetFromXML(rapidxml::xml_node<>* pRootNode)
{
	// Create an entity for every asset in the XML
	 rapidxml::xml_node<>* pAssets = CRapidXMLAdditions::findChildNode(pRootNode, "assets");
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
		CSpriteAsset *pSprite = new CSpriteAsset();	// create the asset's object
		pSprite->start(this, pNode);	// call its start function
		m_Assets.push_back(pSprite);	// make it available in the manager
	}
}

void CManager::loadSpriteMap(rapidxml::xml_node<>* pNode)
{
	// e.g. <spritemap src="cursors.png">
	if (strcmp(pNode->name(), "spritemap") == 0)
	{
		CSpriteMap *pSpriteMap = new CSpriteMap();	// create the asset's object
		pSpriteMap->start(this, pNode);	// call its start function

		for (rapidxml::xml_node<>* pChild = pNode->first_node(); pChild != NULL; pChild = pChild->next_sibling())
		{
			// if static image
			// e.g. <sprite name="cursor-generic" xOffset="0" yOffset="0" width="356" height="150"/>
			if (strcmp(pChild->name(), "sprite") == 0)
			{
				CSpriteMapImageAsset *pSprite = new CSpriteMapImageAsset();	// create the asset's object
				pSprite->start(this, pChild, pSpriteMap);	// call its start function
				m_Assets.push_back(pSprite);	// make it available in the manager
				int i = 5;
			}

			// if animation
			// e.g. <animation name="8" xOffset="200" yOffset="0" width="50" height="80" steps="8" time="200"/>
			if (strcmp(pChild->name(), "animation") == 0)
			{
				CSpriteMapAnimationAsset *pAnimation = new CSpriteMapAnimationAsset();	// create the asset's object
				pAnimation->start(this, pChild, pSpriteMap);	// call its start function
				m_Assets.push_back(pAnimation);	// make it available in the manager
			}
		}
	}
}

void CManager::createEverySceneFromXML(rapidxml::xml_node<>* pRootNode)
{
	for (rapidxml::xml_node<>* pSceneNode = pRootNode->first_node(); pSceneNode != NULL; pSceneNode = pSceneNode->next_sibling())
	{
		if (strcmp(pSceneNode->name(), "scene") == 0)
		{
			CScene* pScene = new CScene();
			m_Scenes.push_back(pScene);
			if (m_pActiveScene == NULL)
				m_pActiveScene = pScene;

			createEveryGameObjectFromXML(pSceneNode, pScene);
		}
	}
}

void CManager::createEveryGameObjectFromXML(rapidxml::xml_node<>* pNode, CScene* pScene)
{
	// Create all the gameobjects defined in the scene
	for (rapidxml::xml_node<>* pNodeGameObject = pNode->first_node(); pNodeGameObject != NULL; pNodeGameObject = pNodeGameObject->next_sibling())
	{
		// e.g. <gameobject name="background" posX="0" posY="0" scaleX="1" scaleY="1" rotation="0">
		if (strcmp(pNodeGameObject->name(), "gameobject") == 0)
		{
			CGameObject* pGameObject = new CGameObject();	// create a gameobject
			pScene->m_GameObjects.push_back(pGameObject);	// add it to the manager's list

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
				createAnimationComponentFromXML(pNodeComponent, pGameObject);	// create the animation component
				createCursorComponent(pNodeComponent, pGameObject);	// create the animation component
			}
		}
	}
}

void CManager::createSpriteComponentFromXML(rapidxml::xml_node<>* pNode, CGameObject* pGameObject)
{
	// e.g. <sprite load = "street" posX = "0" posY = "0" scaleX = "4" scaleY = "4" rotation = "0" originX = "0.5" originY = "0.5" / >
	if (strcmp(pNode->name(), "sprite") == 0)
	{
		CSpriteComponent* pSpriteComp = new CSpriteComponent();	// create the component itself
		pGameObject->m_components.push_back(pSpriteComp);	// add it to the gameobject
		pSpriteComp->m_pParentGameObject = pGameObject;

		std::string assetNameToLoad = CRapidXMLAdditions::getAttributeValue(pNode, "load");	// get the name of the asset to load
		pSpriteComp->m_pAsset = (CSpriteAsset*)getAssetOnName(assetNameToLoad);	// assign the pointer to the asset

		createSpriteComponentBasicData(pNode, pSpriteComp);	// load basic sprite data from XML
	}
}

void CManager::createAnimationComponentFromXML(rapidxml::xml_node<>* pNode, CGameObject* pGameObject)
{
	// e.g. <animation name="moveRight" xOffset="200" yOffset="0" width="50" height="80" steps="8" time="200"/>
	if (strcmp(pNode->name(), "animation") == 0)
	{
		CAnimationComponent* pAnimationComp = new CAnimationComponent();	// create the component itself
		pGameObject->m_components.push_back(pAnimationComp);	// add it to the gameobject
		pAnimationComp->m_pParentGameObject = pGameObject;

		std::string assetNameToLoad = CRapidXMLAdditions::getAttributeValue(pNode, "load");	// get the name of the asset to load
		pAnimationComp->m_pAsset = (CSpriteMapAnimationAsset*)getAssetOnName(assetNameToLoad);	// assign the pointer to the asset

		createSpriteComponentBasicData(pNode, pAnimationComp);	// load basic sprite data from XML

		// load animation specific data from XML
		pAnimationComp->m_bReversePlay = (bool)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "reverseplay"));
		pAnimationComp->m_bMirrorX = (bool)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "mirrorX"));
		pAnimationComp->m_bMirrorY = (bool)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "mirrorY"));

		m_pActiveScene->m_GameObjects;
		int i = 5;
	}
}

void CManager::createSpriteComponentBasicData(rapidxml::xml_node<>* pNode, CComponent* pComponent)
{
	// Store the position of the object
	pComponent->m_v2fPosition = sf::Vector2f(
		(float)atof(CRapidXMLAdditions::getAttributeValue(pNode, "posX")),
		(float)atof(CRapidXMLAdditions::getAttributeValue(pNode, "posY")));

	// Store the scale
	pComponent->m_v2fScale = sf::Vector2f(
		(float)atof(CRapidXMLAdditions::getAttributeValue(pNode, "scaleX")),
		(float)atof(CRapidXMLAdditions::getAttributeValue(pNode, "scaleY")));

	// Store the origin
	pComponent->m_v2fOrigin = sf::Vector2f(
		(float)atof(CRapidXMLAdditions::getAttributeValue(pNode, "originX")),
		(float)atof(CRapidXMLAdditions::getAttributeValue(pNode, "originY")));

	// Store rotation
	pComponent->m_nRotation = (float)atof(CRapidXMLAdditions::getAttributeValue(pNode, "rotation"));

	// Store enabled
	pComponent->m_bEnabled = (bool)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "enabled"));
}

void CManager::createCursorComponent(rapidxml::xml_node<>* pNode, CGameObject* pGameObject)
{
	if (strcmp(pNode->name(), "cursor") == 0)
	{
		m_pCursor = pGameObject;

		CCursorComponent* pCursorComp = new CCursorComponent();	// create the component itself
		pCursorComp->m_pParentGameObject = pGameObject;

		int indexGeneric = atoi(CRapidXMLAdditions::getAttributeValue(pNode, "compIndexGeneric"));
		int indexHighlight = atoi(CRapidXMLAdditions::getAttributeValue(pNode, "compIndexHighlight"));
		int index = 0;
		for (std::list<CComponent*>::iterator it = pGameObject->m_components.begin(); it != pGameObject->m_components.end(); ++it)
		{
			(*it);
			// iterate through every asset by the index defined above and assign the pointer in the component
			/*m_pSpriteGeneric = ;
			m_pSpriteHighlight = ;
			m_pCurrentAsset = ;*/
			// update the cursor etc
		}

		pGameObject->m_components.push_back(pCursorComp);	// add it to the gameobject
	}
}

void CManager::drawScene(sf::RenderWindow* pWindow)
{
	for (std::list<CGameObject*>::iterator it = m_pActiveScene->m_GameObjects.begin(); it != m_pActiveScene->m_GameObjects.end(); ++it)
	{
		(*it)->update(pWindow);
	}
}