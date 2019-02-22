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

void CManager::start(rapidxml::xml_node<>* pRootNode)
{
	loadEveryAssetFromXML(pRootNode);
	createEverySceneFromXML(pRootNode);
	setReferences();
}

void CManager::update(sf::RenderWindow* pWindow)
{
	// Calculate the delta time
	m_deltaTime = m_managerClock.getElapsedTime().asSeconds();
	m_managerClock.restart();

	drawScene(pWindow);
}

void CManager::drawScene(sf::RenderWindow* pWindow)
{
	// loop through the array of gameobjects
	for (std::list<CGameObject*>::iterator it = m_pActiveScene->m_GameObjects.begin(); it != m_pActiveScene->m_GameObjects.end(); ++it)
	{
		if ((*it)->m_bEnabled)
			(*it)->update(pWindow);	// call their update function
	}

	// late update for each component
	for (std::list<CComponent*>::iterator it = m_pActiveScene->m_ComponentsDrawLate.begin(); it != m_pActiveScene->m_ComponentsDrawLate.end(); ++it)
	{
		(*it)->update(pWindow);	// call its update function
	}
}

// return a pointer to a loaded asset by its name
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

// read the XML and load the assets into the memory
void CManager::loadEveryAssetFromXML(rapidxml::xml_node<>* pRootNode)
{
	// Create an entity for every asset in the XML
	 rapidxml::xml_node<>* pAssets = CRapidXMLAdditions::findChildNode(pRootNode, "assets");	// get the "assets" container
	if (pAssets != NULL)
	{
		CManager::instance().m_strAssetPath = CRapidXMLAdditions::getAttributeValue(pAssets, "path"); // Save the path into the manager

		// read out every type asset and create it
		for (rapidxml::xml_node<>* pChild = pAssets->first_node(); pChild != NULL; pChild = pChild->next_sibling())
		{
			loadSprite(pChild);	// if asset is of type sprite
			loadSpriteMap(pChild);	// if asset is of type spriteMap
			loadAudio(pChild);	// if asset is of type audio
			loadFont(pChild);	// if asset is of type font
		}
	}
}

// Create and initialize a sprite asset
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

// Create and initialize a spriteMap asset
void CManager::loadSpriteMap(rapidxml::xml_node<>* pNode)
{
	// e.g. <spritemap src="cursors.png">
	if (strcmp(pNode->name(), "spritemap") == 0)
	{
		CSpriteMap *pSpriteMap = new CSpriteMap();	// create the asset's object
		pSpriteMap->start(this, pNode);	// call its start function

		// loop through each child
		for (rapidxml::xml_node<>* pChild = pNode->first_node(); pChild != NULL; pChild = pChild->next_sibling())
		{
			// if static image
			// e.g. <sprite name="cursor-generic" xOffset="0" yOffset="0" width="356" height="150"/>
			if (strcmp(pChild->name(), "sprite") == 0)
			{
				CSpriteMapImageAsset *pSprite = new CSpriteMapImageAsset();	// create the asset's object
				pSprite->start(this, pChild, pSpriteMap);	// call its start function
				m_Assets.push_back(pSprite);	// make it available in the manager
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

// Create and initialize an audio asset
void CManager::loadAudio(rapidxml::xml_node<>* pNode)
{
	// e.g. <audio name="lightswitch">nameOfFile.ogg</audio>
	if (strcmp(pNode->name(), "audio") == 0)
	{
		//#### AudioType is missing ####
		//CSpriteAsset *pAudio = new CSpriteAsset();	// create the asset's object
		//pAudio->start(this, pNode);	// call its start function
		//m_Assets.push_back(pAudio);	// make it available in the manager
	}
}

// Create and initialize a font asset
void CManager::loadFont(rapidxml::xml_node<>* pNode)
{
	// e.g. <font name="montserratR" src="Montserrat-Regular.ttf"/>
	if (strcmp(pNode->name(), "font") == 0)
	{
		CFontAsset *pFont = new CFontAsset();	// create the asset's object
		pFont->start(this, pNode);	// call its start function
		m_Assets.push_back(pFont);	// make it available in the manager
		int i = 5;
	}
}

// create all scenes and store them
void CManager::createEverySceneFromXML(rapidxml::xml_node<>* pRootNode)
{
	// Loop through every childnode of root
	for (rapidxml::xml_node<>* pSceneNode = pRootNode->first_node(); pSceneNode != NULL; pSceneNode = pSceneNode->next_sibling())
	{
		// Find all scenes
		if (strcmp(pSceneNode->name(), "scene") == 0)
		{
			CScene* pScene = new CScene();	// create the scene's object
			m_Scenes.push_back(pScene);	// make it available in the manager
			// if there us no active scene, assign the first one found
			if (m_pActiveScene == NULL)
				m_pActiveScene = pScene;

			createEveryGameObjectFromXML(pSceneNode, pScene);	// create all gameobjects of that scene
		}
	}
}

// create gameobjects of a scene according to the XML
void CManager::createEveryGameObjectFromXML(rapidxml::xml_node<>* pSceneNode, CScene* pScene)
{
	// Create all the gameobjects defined in the scene
	for (rapidxml::xml_node<>* pNodeGameObject = pSceneNode->first_node(); pNodeGameObject != NULL; pNodeGameObject = pNodeGameObject->next_sibling())
	{
		// e.g. <gameobject name="background" posX="0" posY="0" scaleX="1" scaleY="1" rotation="0">
		if (strcmp(pNodeGameObject->name(), "gameobject") == 0)
		{
			CGameObject* pGameObject = new CGameObject();	// create a gameobject
			pScene->m_GameObjects.push_back(pGameObject);	// add it to the manager's list of gameobjects of that given scene

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

			// Store enabled
			char* objectEnabled = CRapidXMLAdditions::getAttributeValue(pNodeGameObject, "enabled");
			if (objectEnabled != "")	// set this value only if it is actually specified in the XML
				pGameObject->m_bEnabled = (bool)atoi(objectEnabled);

			// create all its components
			for (rapidxml::xml_node<>* pNodeComponent = pNodeGameObject->first_node(); pNodeComponent != NULL; pNodeComponent = pNodeComponent->next_sibling())
			{
				createSpriteComponentFromXML(pNodeComponent, pGameObject);	// create the sprite component
				createAnimationComponentFromXML(pNodeComponent, pGameObject);	// create the animation component
				createCursorComponent(pNodeComponent, pGameObject);	// create the cursor component
				createMoveToTargetComponent(pNodeComponent, pGameObject);	// create the moveToTarget component
				createAnimationCtrlComponent(pNodeComponent, pGameObject);	// create the Animation Controller
				createInteractionComponent(pNodeComponent, pGameObject);	// create the interaction component
				createDescriptionComponent(pNodeComponent, pGameObject, pScene);	// create the description component
				createTextboxComponent(pNodeComponent, pGameObject, pScene);	// create the textbox component
			}
		}
	}
}

// create a sprite components to a given gameobject
void CManager::createSpriteComponentFromXML(rapidxml::xml_node<>* pNode, CGameObject* pGameObject)
{
	// e.g. <sprite load = "street" posX = "0" posY = "0" scaleX = "4" scaleY = "4" rotation = "0" originX = "0.5" originY = "0.5" / >
	if (strcmp(pNode->name(), "sprite") == 0)
	{
		CSpriteComponent* pSpriteComp = new CSpriteComponent();	// create the component itself
		pGameObject->m_components.push_back(pSpriteComp);	// add it to the gameobject
		pSpriteComp->m_pParentGameObject = pGameObject;		// letting the component know to which gameobject it is attached to

		std::string assetNameToLoad = CRapidXMLAdditions::getAttributeValue(pNode, "load");	// get the name of the asset to load
		pSpriteComp->m_pAsset = (CSpriteAsset*)getAssetOnName(assetNameToLoad);	// assign a pointer to the asset to load

		createSpriteComponentBasicData(pNode, pSpriteComp);	// load basic sprite data from XML
	}
}

// create an animation components to a given gameobject
void CManager::createAnimationComponentFromXML(rapidxml::xml_node<>* pNode, CGameObject* pGameObject)
{
	// e.g. <animation name="moveRight" xOffset="200" yOffset="0" width="50" height="80" steps="8" time="200"/>
	if (strcmp(pNode->name(), "animation") == 0)
	{
		CAnimationComponent* pAnimationComp = new CAnimationComponent();	// create the component itself
		pGameObject->m_components.push_back(pAnimationComp);	// add it to the gameobject
		pAnimationComp->m_pParentGameObject = pGameObject;		// letting the component know to which gameobject it is attached to

		std::string assetNameToLoad = CRapidXMLAdditions::getAttributeValue(pNode, "load");	// get the name of the asset to load
		pAnimationComp->m_pAsset = (CSpriteMapAnimationAsset*)getAssetOnName(assetNameToLoad);	// assign a pointer to the asset to load

		createSpriteComponentBasicData(pNode, pAnimationComp);	// load basic sprite data from XML

		// load animation specific data from XML
		pAnimationComp->m_bReversePlay = (bool)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "reverseplay"));
		pAnimationComp->m_bMirrorX = (bool)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "mirrorX"));
		pAnimationComp->m_bMirrorY = (bool)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "mirrorY"));
	}
}

// read out basic parameter and store them in the component
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

// create the cursor components
void CManager::createCursorComponent(rapidxml::xml_node<>* pNode, CGameObject* pGameObject)
{
	// e.g. <cursor compIndexGeneric="0" compIndexHighlight="1"/>
	if (strcmp(pNode->name(), "cursor") == 0)
	{
		m_pCursor = pGameObject;	// letting the manager know which gameobject is to be considered as the mouse cursor

		CCursorComponent* pCursorComp = new CCursorComponent();	// create the component itself
		pGameObject->m_components.push_back(pCursorComp);	// add it to the gameobject
		pCursorComp->m_pParentGameObject = pGameObject;		// letting the component know to which gameobject it is attached to

		// Get through every component attached to the cursor gameobject and look for the first three assigned spriteComponents
		for (std::list<CComponent*>::iterator it = pGameObject->m_components.begin(); it != pGameObject->m_components.end(); ++it)
		{
			// if it is a SpriteComponent; seems not to work as the CursorComponent can also pass this line?!
			if ((CSpriteComponent*)(*it))
			{
				// If we haven't set SpriteGeneric allready, set it
				if (pCursorComp->m_pSpriteGeneric == NULL)
				{
					pCursorComp->m_pSpriteGeneric = dynamic_cast<CSpriteComponent*>(*it)->m_pAsset;
					continue;
				}

				// If we haven't set SpriteHighlight allready, set it
				if (pCursorComp->m_pSpriteHighlight == NULL)
				{
					pCursorComp->m_pSpriteHighlight = dynamic_cast<CSpriteComponent*>(*it)->m_pAsset;
					continue;
				}

				// If we haven't set SpriteLoad allready, set it
				if (pCursorComp->m_pSpriteLoading == NULL)
				{
					pCursorComp->m_pSpriteLoading = dynamic_cast<CSpriteComponent*>(*it)->m_pAsset;
					continue;
				}
			}
		}		
	}
}

// create the moveToTarget components
void CManager::createMoveToTargetComponent(rapidxml::xml_node<>* pNode, CGameObject* pGameObject)
{
	// e.g. <moveToTarget gameobject="player0"/>
	if (strcmp(pNode->name(), "moveToTarget") == 0)
	{
		CMoveToTarget* pComponent = new CMoveToTarget();	// create the component itself
		pGameObject->m_components.push_back(pComponent);	// add it to the gameobject
		pComponent->m_pParentGameObject = pGameObject;		// letting the component know to which gameobject it is attached to

		// Store object reference
		for (std::list<CGameObject*>::iterator it = m_pActiveScene->m_GameObjects.begin(); it != m_pActiveScene->m_GameObjects.end(); ++it)
		{
			char* temp = CRapidXMLAdditions::getAttributeValue(pNode, "gameobject");
			if ((*it)->getName() == temp)
				pComponent->m_objectToMove = *it;
		}

		// Store movement speed variable
		pComponent->m_moveSpeed = (float)atof(CRapidXMLAdditions::getAttributeValue(pNode, "movespeed"));
	}
}

// create the Animation Controller
void CManager::createAnimationCtrlComponent(rapidxml::xml_node<>* pNode, CGameObject* pGameObject)
{
	// e.g. <animationController idleRightIndex="0" idleLeftIndex="1" moveRightIndex="4" moveLeftIndex="5" moveUpIndex="2" moveDownIndex="3"/>
	if (strcmp(pNode->name(), "animationController") == 0)
	{
		CAnimationCtrl* pComponent = new CAnimationCtrl();	// create the component itself
		pGameObject->m_components.push_back(pComponent);	// add it to the gameobject
		pComponent->m_pParentGameObject = pGameObject;		// letting the component know to which gameobject it is attached to

		// read out the parameters
		pComponent->m_idleRightIndex = (int)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "idleRightIndex"));
		pComponent->m_idleLeftIndex = (int)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "idleLeftIndex"));
		pComponent->m_moveRightIndex = (int)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "moveRightIndex"));
		pComponent->m_moveLeftIndex = (int)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "moveLeftIndex"));
		pComponent->m_moveUpIndex = (int)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "moveUpIndex"));
		pComponent->m_moveDownIndex = (int)atoi(CRapidXMLAdditions::getAttributeValue(pNode, "moveDownIndex"));
	}
}

// create the interaction components
void CManager::createInteractionComponent(rapidxml::xml_node<>* pNode, CGameObject* pGameObject)
{
	// e.g. <interaction type="pickUp" range="100"/> || all types = combination, pickUp, clickable
	if (strcmp(pNode->name(), "interaction") == 0)
	{
		CInteractionComponent* pComponent = new CInteractionComponent();	// create the component itself
		pGameObject->m_components.push_back(pComponent);	// add it to the gameobject
		CManager::instance().m_pActiveScene->m_Interactables.push_back(pGameObject);	// add it to the manager's list of interactive gameobjects of that given scene

		pComponent->m_pParentGameObject = pGameObject;		// letting the component know to which gameobject it is attached to
		pGameObject->m_interactionComponent = pComponent;	// store the interaction component for quick access
	}
}

// create the description components
void CManager::createDescriptionComponent(rapidxml::xml_node<>* pNode, CGameObject* pGameObject, CScene* pScene)
{
	// e.g. <description load="montserratR" posX="-180" posY="0" scaleX="1" scaleY="1" rotation="0" originX="0" originY="0" enabled="1">This is a key for a door.</description>
	if (strcmp(pNode->name(), "description") == 0)
	{
		CDescriptionComponent* pComponent = new CDescriptionComponent();	// create the component itself
		pGameObject->m_components.push_back(pComponent);	// add it to the gameobject
		pScene->m_ComponentsDrawLate.push_back(pComponent);	// add the component to the late update / late drawCall function
		pComponent->m_pParentGameObject = pGameObject;		// letting the component know to which gameobject it is attached to

		pComponent->m_descriptionText.setString(pNode->value());	// read text from XML and store it in the gameobject
		pComponent->m_lifetime = (float)atof(CRapidXMLAdditions::getAttributeValue(pNode, "lifetime"));	// read the lifetime from XML

		std::string assetNameToLoad = CRapidXMLAdditions::getAttributeValue(pNode, "load");	// get the name of the asset to load
		CFontAsset* m_pAsset = (CFontAsset*)getAssetOnName(assetNameToLoad);	// assign a pointer to the asset to load
		if (m_pAsset)
			pComponent->m_descriptionText.setFont(m_pAsset->m_font);

		pComponent->m_drawLate = true;
		createSpriteComponentBasicData(pNode, pComponent);	// load basic sprite data from XML
	}
}

// create the textbox components
void CManager::createTextboxComponent(rapidxml::xml_node<>* pNode, CGameObject* pGameObject, CScene* pScene)
{
	// e.g. <textbox posX="-180" posY="0" scaleX="1" scaleY="1" rotation="0" originX="0" originY="0" enabled="0"/>
	if (strcmp(pNode->name(), "textbox") == 0)
	{
		CTextbox* pComponent = new CTextbox();	// create the component itself
		pGameObject->m_components.push_back(pComponent);	// add it to the gameobject
		pScene->m_ComponentsDrawLate.push_back(pComponent);	// add the component to the late update / late drawCall function
		pComponent->m_pParentGameObject = pGameObject;		// letting the component know to which gameobject it is attached to
		pGameObject->m_textComponent = pComponent;	// assign the textbox component reference

		pComponent->m_drawLate = true;	// tell the component to get drawn after everything else in a second call
		createSpriteComponentBasicData(pNode, pComponent);	// load basic sprite data from XML
	}
}

// set all available references
void CManager::setReferences()
{
	for (std::list<CGameObject*>::iterator it = m_pActiveScene->m_GameObjects.begin(); it != m_pActiveScene->m_GameObjects.end(); ++it)
	{
		if ((*it)->getName() == "player")
			m_pActiveScene->m_player = *it;
		if ((*it)->getName() == "playerMoveToTarget")
			m_pActiveScene->m_playerMoveToTarget = *it;
	}
}

// tell each interaction module that the mouse was used
void CManager::processMouseInput(sf::RenderWindow* pWindow, bool leftMouseBtnWasUsed)
{
	bool bUsedInput = false;

	// Offer each interaction components to use that input. If no one will consume that input, a default action will be performed
	// like a moveing action for the charater on the left mouse button
	for (std::list<CGameObject*>::iterator it = m_pActiveScene->m_Interactables.begin(); it != m_pActiveScene->m_Interactables.end(); ++it)
	{
		bUsedInput = (*it)->m_interactionComponent->processMouseButton((sf::Vector2f)sf::Mouse::getPosition(*pWindow), leftMouseBtnWasUsed);	// Offer component to consume the input

		// only perform the first action that fits
		if (bUsedInput)
			break;
	}

	if (leftMouseBtnWasUsed && !bUsedInput)
	{
		// move the player's character to that location
		CManager::instance().m_pActiveScene->m_playerMoveToTarget->m_v2fPosition = (sf::Vector2f)sf::Mouse::getPosition(*pWindow);
	}
}