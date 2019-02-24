#include "gameobject.h"
#include "interpolation.h"
#include "inventory.h"

// ---------- CGameObject ---------------------------------------------------------------------------------------------------------------

void CGameObject::update(sf::RenderWindow* pWindow)
{
	if (!m_bEnabled)
		return;

	// for each component
	for (std::list<CComponent*>::iterator it = m_components.begin(); it != m_components.end(); ++it)
	{
		if(!(*it)->m_drawLate)	// exclude components that should be drawn at last e.g. DescriptionComponent
			(*it)->update(pWindow);	// call its update function
	}
}

std::string CGameObject::getName() { return m_name; }

// ---------- CComponent ---------------------------------------------------------------------------------------------------------------
void CComponent::update(sf::RenderWindow* pWindow) { }	// function should be overridden by derived class
sf::Sprite* CComponent::getSprite() { return NULL; }

// ---------- CSpriteComponent ---------------------------------------------------------------------------------------------------------------

void CSpriteComponent::update(sf::RenderWindow* pWindow)
{
	// skip update if the component is not active
	if (!m_bEnabled)
		return;

	// skip if there is no asset assigned
	// as the type of m_pAsset differ in CSpriteComponent and CAnimationComponent, each class need its own code
	if (m_pAsset == NULL)
		return;

	sf::Sprite* pSprite = m_pAsset->getSprite();	// assign the sprite
	if (pSprite != NULL)
	{
		// Process transform
		pSprite->setOrigin(m_v2fOrigin);
		pSprite->setPosition(m_pParentGameObject->m_v2fPosition + m_v2fPosition);
		pSprite->setScale(sf::Vector2f(m_pParentGameObject->m_v2fScale.x * m_v2fScale.x, m_pParentGameObject->m_v2fScale.y * m_v2fScale.y));
		pSprite->setRotation(m_pParentGameObject->m_nRotation + m_nRotation);
	}
	m_pAsset->update(pWindow);	// draw it on screen
}

sf::Sprite* CSpriteComponent::getSprite()
{
	return m_pAsset->getSprite();
}

// ---------- CAnimationComponent ---------------------------------------------------------------------------------------------------------------

void CAnimationComponent::update(sf::RenderWindow* pWindow)
{
	// skip update if the component is not active
	if (!m_bEnabled)
		return;

	// skip if there is no asset assigned
	// as the type of m_pAsset differ in CSpriteComponent and CAnimationComponent, each class need its own code
	if (m_pAsset == NULL)
		return;

	m_pAsset->m_pParentGameObject = this;	// assign the parent object
	sf::Sprite* pSprite = m_pAsset->getSprite();	// assign the sprite
	if (pSprite != NULL)
	{
		sf::Vector2f scale;

		pSprite->setOrigin(m_v2fOrigin);	// set origin

		// Set Position
		sf::Vector2f position = m_pParentGameObject->m_v2fPosition + m_v2fPosition;
		if (m_bMirrorX)
			position.x = position.x + (m_pAsset->getSprite()->getTextureRect().width * m_v2fScale.x);
		if (m_bMirrorY)
			position.y = position.y + (m_pAsset->getSprite()->getTextureRect().height * m_v2fScale.y);
		pSprite->setPosition(position);

		// Set scale & mirror sprite if needed
		scale.x = m_bMirrorX ? -(m_pParentGameObject->m_v2fScale.x * m_v2fScale.x) : m_pParentGameObject->m_v2fScale.x * m_v2fScale.x;
		scale.y = m_bMirrorY ? -(m_pParentGameObject->m_v2fScale.y * m_v2fScale.y) : m_pParentGameObject->m_v2fScale.y * m_v2fScale.y;
		pSprite->setScale(scale);

		pSprite->setRotation(m_pParentGameObject->m_nRotation + m_nRotation);	// set rotation
	}
	m_pAsset->update(pWindow);	// draw it on screen
}

// ---------- CCursorComponent ---------------------------------------------------------------------------------------------------------------

void CCursorComponent::update(sf::RenderWindow* pWindow)
{
	// skip update if the component is not active
	if (!m_bEnabled)
		return;

	// skip if there are no asset assigned
	if (m_pSpriteGeneric == NULL || m_pSpriteHighlight == NULL || m_pSpriteLoading == NULL)
		return;

	m_pParentGameObject->m_v2fPosition = (sf::Vector2f)sf::Mouse::getPosition(*pWindow);	// match cursorSprite to mouse pos

	// in case there is a sequence or something playing, show the loading cursor
	if (CManager::instance().m_bInputDisabled)
		switchAppearance(eCursorSprite::load);
	else
	{
		bool bShowHighlightCursor = false;

		// Check if the cursor collides with an object the player could interact with
		for (std::list<CGameObject*>::iterator itGameObject = CManager::instance().m_pActiveScene->m_Interactables.begin(); itGameObject != CManager::instance().m_pActiveScene->m_Interactables.end(); ++itGameObject)
		{
			for (std::list<CInteractionComponent*>::iterator itComponents = (*itGameObject)->m_interactionComponents.begin(); itComponents != (*itGameObject)->m_interactionComponents.end(); ++itComponents)
			{
				if ((*itComponents)->m_bEnabled)
				{
					if ((*itComponents)->checkCollisionPoint((sf::Vector2f)sf::Mouse::getPosition(*pWindow)))
					{
						bShowHighlightCursor = true;
						break;
					}
				}
			}
		}

		if (bShowHighlightCursor)
			switchAppearance(eCursorSprite::highlight);	// select the highlight sprite
		else
			switchAppearance(eCursorSprite::generic);	// select the generic sprite
	}
}

void CCursorComponent::switchAppearance(eCursorSprite cursorToShow)
{
	if (cursorToShow == eCursorSprite::highlight)
	{
		// show highlight cursor
		std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin();
		(*it)->m_bEnabled = 0;
		it++;
		(*it)->m_bEnabled = 1;
		it++;
		(*it)->m_bEnabled = 0;
	}
	if (cursorToShow == eCursorSprite::generic)
	{
		// show generic cursor
		std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin();
		(*it)->m_bEnabled = 1;
		it++;
		(*it)->m_bEnabled = 0;
		it++;
		(*it)->m_bEnabled = 0;
	}
	if (cursorToShow == eCursorSprite::load)
	{
		// show generic cursor
		std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin();
		(*it)->m_bEnabled = 0;
		it++;
		(*it)->m_bEnabled = 0;
		it++;
		(*it)->m_bEnabled = 1;
	}
}

// ---------- CMoveToTarget ---------------------------------------------------------------------------------------------------------------
void CMoveToTarget::update(sf::RenderWindow* pWindow)
{
	CGameObject* temp = CManager::instance().m_pActiveScene->m_player;
	// if the player is not at the same position than this object,
	// move it in that direction
	if (m_objectToMove->m_v2fPosition != m_pParentGameObject->m_v2fPosition)
	{
		sf::Vector2f v2fDirection = m_pParentGameObject->m_v2fPosition - m_objectToMove->m_v2fPosition;	// calculate the direction
		float magnitudeToDestination = sqrt(v2fDirection.x * v2fDirection.x + v2fDirection.y * v2fDirection.y);	// calculate the magnitude
		v2fDirection = v2fDirection / magnitudeToDestination;	// normalize the vector
		float movementspeed = m_moveSpeed * CManager::instance().m_deltaTime;	// calculate the new lenght of the vector
		v2fDirection = { v2fDirection.x * movementspeed, v2fDirection.y * movementspeed };	// apply the new lenght to the vector (distance to move per frame)
		v2fDirection = m_objectToMove->m_v2fPosition + v2fDirection;	// add everything up to get the next frame's position

		// In case the movement would overshoot the destination, set the object directly to the destination
		if (magnitudeToDestination < movementspeed)
			m_objectToMove->m_v2fPosition = m_pParentGameObject->m_v2fPosition;	// perform the set to destination action
		else
			m_objectToMove->m_v2fPosition = v2fDirection;	// perform the move action
	}
}

// ---------- CAnimationCtrl ---------------------------------------------------------------------------------------------------------------
void CAnimationCtrl::update(sf::RenderWindow* pWindow)
{
	// In case there is NO movement
	if (m_v2fLastFramePos == m_pParentGameObject->m_v2fPosition)
		activateAnimationWithGivenIndex(m_idleRightIndex);
	// in case there IS movement
	else
	{
		// get the move direction
		sf::Vector2f direction = m_pParentGameObject->m_v2fPosition - m_v2fLastFramePos;

		// movement towards the right
		if (direction.x > 0)
		{
			// movement towards the right AND bottom
			if (direction.y > 0)
			{
				if (direction.x > direction.y)	// play moveRight anim
					activateAnimationWithGivenIndex(m_moveRightIndex);
				else
					activateAnimationWithGivenIndex(m_moveDownIndex);
			}

			// movement towards the right AND top
			if (direction.y < 0)
			{
				if (direction.x > (direction.y * -1))	// play moveRight anim
					activateAnimationWithGivenIndex(m_moveRightIndex);
				else
					activateAnimationWithGivenIndex(m_moveUpIndex);
			}
		}
		// movement towards the left
		if (direction.x < 0)
		{
			// movement towards the left AND bottom
			if (direction.y > 0)
			{
				if (direction.x < (direction.y * -1))	// play moveLeft anim
					activateAnimationWithGivenIndex(m_moveLeftIndex);
				else
					activateAnimationWithGivenIndex(m_moveDownIndex);
			}

			// movement towards the left AND top
			if (direction.y < 0)
			{
				if (direction.x < direction.y)	// play moveLeft anim
					activateAnimationWithGivenIndex(m_moveLeftIndex);
				else
					activateAnimationWithGivenIndex(m_moveUpIndex);
			}
		}
	}

	m_v2fLastFramePos = m_pParentGameObject->m_v2fPosition;	// save the position of the object in this frame, to be able to compare movement in the next frame
}

// enable the object with given index and deactivate every one else, also scale accordingly to the y-axis and a factor
void CAnimationCtrl::activateAnimationWithGivenIndex(int index)
{
	int currentIndex = 0;
	for (std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin(); it != m_pParentGameObject->m_components.end(); ++it)
	{
		// if it is the element we are looking for, scale and enable it
		if (currentIndex == index)
		{
			// interpolate the scaling
			float scale = (float)interpolate(vector<double>{ 688, 900 }, vector<double>{ 1.25, 2 }, m_pParentGameObject->m_v2fPosition.y, true);
			// Clamp the value towards a minimum scale
			if (scale < 0.5)
				scale = 0.5;
			(*it)->m_v2fScale = { scale,scale };	// only the active component needs to be scaled, the others arn't visible anyways

			(*it)->m_bEnabled = true;	// enable the component
		}
		else  // otherwise disable it by default
		{
			// except the text box of the player; this line is a very specific workaround, but due to time constrains...
			bool bIsText = false;
			for (std::list<CTextbox*>::iterator itText = CManager::instance().m_pActiveScene->m_player->m_pTextComponents.begin(); itText != CManager::instance().m_pActiveScene->m_player->m_pTextComponents.end(); ++itText)
			{
				if ((*it) == (*itText))
					bIsText = true;
			}

			if(!bIsText)
				(*it)->m_bEnabled = false;
		}

		currentIndex++;
	}
}

// ---------- CInteractionComponent ---------------------------------------------------------------------------------------------------------------
void CInteractionComponent::update(sf::RenderWindow* pWindow)
{
	// skip if deactivated
	if (!m_bEnabled)
		return;

	// get the bounding box of the largest currently active drawable object (currently sf::Sprite only); Does it need to be calculated on each frame? Optimization potential here!
	for (std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin(); it != m_pParentGameObject->m_components.end(); ++it)
	{
		if ((*it)->m_bEnabled)	// skip every disabled component
		{
			float currentSize = m_boundingBox.height * m_boundingBox.width;	// calculate the face

			// continue only if the current component does have a sprite
			sf::Sprite* sampleSprite = (*it)->getSprite();
			if (sampleSprite)
			{
				sf::FloatRect sample = sampleSprite->getGlobalBounds();	// get boundaries of the current component
				// if the current component boundaries-face is larger than the current stored one, exchange it
				if (currentSize < (sample.height * sample.width))
					m_boundingBox = sample;
			}
		}
	}

	// In case there is a action sequence running
	if (m_bStillPerformingAction)
	{
		CManager::instance().m_bInputDisabled = true;	// deactivate additional input

		// In case the player is to far away, move him to the gameobject first
		sf::Vector2f distanceVector = CManager::instance().m_pActiveScene->m_player->m_v2fPosition - m_pParentGameObject->m_v2fPosition;
		float distance = sqrt(distanceVector.x * distanceVector.x + distanceVector.y * distanceVector.y);
		if (distance > 10)
			CManager::instance().m_pActiveScene->m_playerMoveToTarget->m_v2fPosition = m_pParentGameObject->m_v2fPosition;
		else
		{
			// perform action
			m_bStillPerformingAction = false;	// tell the component that its action was performed completly
			CManager::instance().m_bInputDisabled = false;	// activate input again
			performTask(m_bLeftMouseBtnWasUsed);
		}
	}
}

// Check if a given point is within the assets limits
bool CInteractionComponent::checkCollisionPoint(sf::Vector2f point)
{
	if (m_boundingBox.contains(point))
		return true;
	else
		return false;
}

// Check if another bounding box collides with the assets limits
bool CInteractionComponent::checkCollisionBoundingBox(sf::FloatRect otherBox)
{
	if (m_boundingBox.intersects(otherBox))
		return true;
	else
		return false;
}

// call the different components depending on button and interaction type, will return true if the interaction component "used/consumes" that call
bool CInteractionComponent::processMouseButton(sf::Vector2f mousePos, bool leftMouseBtnWasUsed)
{
	// check if the component "collides" with the mouse cursor position
	if (checkCollisionPoint(mousePos))
	{
		m_bStillPerformingAction = true;	// activate action sequence controlled by the update function
		m_bLeftMouseBtnWasUsed = leftMouseBtnWasUsed;	// store the last mouse button pressed
		return true;
	}
	else
		return false;
}

void CInteractionComponent::performTask(bool leftMouseBtnWasUsed)
{
	if (leftMouseBtnWasUsed)
	{
		if (m_type == "pickup")
		{
			std::string text = "";

			// show pickup text
			for (std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin(); it != m_pParentGameObject->m_components.end(); ++it)
			{
				if ((*it)->m_name == "pickup")
					if ((*it)->performAction(leftMouseBtnWasUsed))
						break;
				//if ((*it)->m_name == "description")
				//	text = "";	// read the value from the text component tagged with description
			}

			CInventoryContainer::instance().addItemToInventory(m_pParentGameObject->getName(), text);	// Add item to inventory
			m_pParentGameObject->m_bEnabled = false;	// remove item from screen
		}

		if (m_type == "use")
		{
			for (std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin(); it != m_pParentGameObject->m_components.end(); ++it)
			{
				// show text
				if ((*it)->m_name == "use")
					(*it)->performAction(leftMouseBtnWasUsed);

				// switch gameobjects to enabled
				for (std::list<std::string>::iterator itString = m_pListToEnable.begin(); itString != m_pListToEnable.end(); ++itString)
				{
					if ((*it)->m_name == *itString)
						(*it)->m_bEnabled = true;
				}

				// switch gameobjects to disabled
				for (std::list<std::string>::iterator itString = m_pListToDisable.begin(); itString != m_pListToDisable.end(); ++itString)
				{
					if ((*it)->m_name == *itString)
						(*it)->m_bEnabled = false;
				}
			}
		}

		if (m_type == "useWithRequirement")
		{
			// check if the required item is in the inventory
			bool bRequirementMet = false;
			CInventoryItem* pItem;
			for (std::list<CInventoryItem*>::iterator it = CInventoryContainer::instance().m_pInventoryItems.begin(); it != CInventoryContainer::instance().m_pInventoryItems.end(); ++it)
			{
				if ((*it)->m_name == m_neededGameObject)
				{
					bRequirementMet = true;
					pItem = *it;
				}
			}

			if (bRequirementMet)
			{
				// show comment text
				for (std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin(); it != m_pParentGameObject->m_components.end(); ++it)
				{
					if ((*it)->m_name == "useSuccess")
						(*it)->performAction(leftMouseBtnWasUsed);

					for (std::list<std::string>::iterator itString = m_pListToEnable.begin(); itString != m_pListToEnable.end(); ++itString)
					{
						if ((*it)->m_name == *itString)
							(*it)->m_bEnabled = true;
					}

					for (std::list<std::string>::iterator itString = m_pListToDisable.begin(); itString != m_pListToDisable.end(); ++itString)
					{
						if ((*it)->m_name == *itString)
							(*it)->m_bEnabled = false;
					}
				}

				CInventoryContainer::instance().m_pInventoryItems.remove(pItem);	// remove the item from the inventory
			}
			else
			{
				// show instruction text
				for (std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin(); it != m_pParentGameObject->m_components.end(); ++it)
				{
					if ((*it)->m_name == "useFailure")
						if ((*it)->performAction(leftMouseBtnWasUsed))
							break;
				}
			}
		}

		if (m_type == "loadScene")
		{
			for (std::list<CScene*>::iterator itScenes = CManager::instance().m_pScenes.begin(); itScenes != CManager::instance().m_pScenes.end(); ++itScenes)
			{
				if ((*itScenes)->m_name == *m_pListToEnable.begin())
				{
					CScene* tempScene = *itScenes;
					CManager::instance().m_pSceneToLoad = (*itScenes);	// tell the manager to load the scene with the beginning of the next frame
																		// switching the scene inbetween a single frame will crash the engine
																		// as there are some for-loops left and their iterators become invalid
				}
			}
		}
	}
	else    // If the right mouse button was used, show the description text
	{
		for (std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin(); it != m_pParentGameObject->m_components.end(); ++it)
		{
			if (!leftMouseBtnWasUsed && (*it)->m_name == "description")
				if (((CTextComponent*)(*it))->performAction(leftMouseBtnWasUsed))
					break;
		}
	}


	
}

// ---------- CTextComponent ---------------------------------------------------------------------------------------------------------------
void CTextComponent::update(sf::RenderWindow* pWindow)
{
	
}

bool CTextComponent::performAction(bool leftMouseBtnWasUsed)
{
	int index = 0;
	for (std::list<CTextbox*>::iterator itText = CManager::instance().m_pActiveScene->m_player->m_pTextComponents.begin(); itText != CManager::instance().m_pActiveScene->m_player->m_pTextComponents.end(); ++itText)
	{
		if (index == CManager::instance().m_pActiveScene->m_player->m_pTextComponents.size() - 1)
			(*itText)->showText(m_text.getString(), m_lifetime, m_text.getFont(), m_charSize, true, m_colorPrimary);	// show the text
		else
			(*itText)->showText(m_text.getString(), m_lifetime, m_text.getFont(), m_charSize, false, m_colorSecondary);	// show the text
		index++;
	}

	return true;
}

// ---------- CTextbox ---------------------------------------------------------------------------------------------------------------
void CTextbox::update(sf::RenderWindow* pWindow)
{
	// exit if component is disabled
	if (!m_bEnabled)
		return;

	if (m_bIsPrimary)	// only the primary (white) text should count
	{
		// Disable the component after lifetime
		if (timer.getElapsedTime().asSeconds() > m_lifetime)
		{
			CTextbox* temp = this;
			for (std::list<CTextbox*>::iterator it = m_pParentGameObject->m_pTextComponents.begin(); it != m_pParentGameObject->m_pTextComponents.end(); ++it)
				(*it)->m_bEnabled = false;

			// tell the current going sequence (if there is one), that one action has been accomblished

			for (std::list<CSequenceComponent*>::iterator it = CManager::instance().m_pActiveScene->m_listSequences.begin(); it != CManager::instance().m_pActiveScene->m_listSequences.end(); ++it)
			{
				if ((*it)->m_bEnabled)
					(*it)->m_pActions.pop_front();
			}

			return;
		}
	}

	// Process transform
	m_text.setOrigin(m_v2fOrigin);
	m_text.setPosition(m_pParentGameObject->m_v2fPosition + m_v2fPosition);
	m_text.setScale(sf::Vector2f(m_pParentGameObject->m_v2fScale.x * m_v2fScale.x, m_pParentGameObject->m_v2fScale.y * m_v2fScale.y));
	m_text.setRotation(m_pParentGameObject->m_nRotation + m_nRotation);

	// set position centered (very very roughly, as each character is not from the same size)
	sf::Vector2f v2fOffset;
	v2fOffset.x = (float)m_text.getString().getSize() / 2;	// only half is needed to center
	v2fOffset.x = v2fOffset.x * -15; // 15px offset

	sf::Vector2f targetPos = m_pParentGameObject->m_v2fPosition + m_v2fPosition + v2fOffset;	// add everything up to get the "should be" position

	// Check if the sentence would be outside the screen and fix it
	if (targetPos.x < 0)
		targetPos.x = 0;
	if (targetPos.y < 0)
		targetPos.y = 0;
	sf::Vector2u screenBoarder = pWindow->getSize();
	screenBoarder.x = screenBoarder.x + 2 * (unsigned int)v2fOffset.x;
	if (targetPos.x > screenBoarder.x)
		targetPos.x = (float)screenBoarder.x;
	if (targetPos.y > screenBoarder.y)
		targetPos.y = (float)screenBoarder.y;
	m_text.setPosition(targetPos);

	pWindow->draw(m_text);	// draw the text
}

void CTextbox::showText(const sf::String& text, float lifetimeInSec, const sf::Font* pFontAsset, unsigned int charSize, bool bIsPrimary, sf::Color color)
{
	CTextbox* temp = this;
	m_text.setString(text);	// assign text to say
	m_lifetime = lifetimeInSec;	// assign lifetime
	m_text.setFont(*pFontAsset);	// assign the font
	m_text.setFillColor(color);
	m_text.setCharacterSize(charSize);	// assign the character size
	m_bIsPrimary = bIsPrimary;

	timer.restart();
	m_bEnabled = true;	// activate the component
}

// ---------- CSequenceComponent ---------------------------------------------------------------------------------------------------------------
CSequenceComponent::CSequenceComponent()
{

}

void CSequenceComponent::update(sf::RenderWindow* pWindow)
{
	// only process sequences that are enabled
	if (!m_bEnabled)
		return;

	CManager::instance().m_bInputDisabled = true;

	// Perform actions as long as there are any
	if (m_pActions.size() > 0)
	{
		// as long as the action is not finished, update it
		if (!(*m_pActions.begin())->m_bFinished)
		{
			// only update the most current one
			(*m_pActions.begin())->update(pWindow);
		}
		// otherwise delete the current action and prepare for the next one
		else
			m_pActions.pop_front();	// remove the first entry
	}
	else	// or disable the sequence
	{
		m_bEnabled = false;
		CManager::instance().m_bInputDisabled = false;
	}
}

// ---------- CSequenceAction ---------------------------------------------------------------------------------------------------------------
void CSequenceAction::update(sf::RenderWindow* pWindow)
{
	// switch on action type
	switch (m_type) {
	case eActionType::move:
	{
		float distance = 0;
		sf::Vector2f distanceVector = { 0,0 };
		CManager::instance().m_pActiveScene->m_playerMoveToTarget->m_v2fPosition = m_moveToVector;
		distanceVector = CManager::instance().m_pActiveScene->m_player->m_v2fPosition - CManager::instance().m_pActiveScene->m_playerMoveToTarget->m_v2fPosition;
		distance = sqrt(distanceVector.x * distanceVector.x + distanceVector.y * distanceVector.y);
		if (distance < 10)
			m_bFinished = true;
		break;
	}
	case eActionType::say:
	{
		int index = 0;
		for (std::list<CTextbox*>::iterator itText = m_targetObject->m_pTextComponents.begin(); itText != m_targetObject->m_pTextComponents.end(); ++itText)
		{
			if (!(*itText)->m_bEnabled)
			{
				if (index == m_targetObject->m_pTextComponents.size() - 1)
					(*itText)->showText(m_text, m_lifetime, m_font, m_charSize, true, m_colorPrimary);	// show the text
				else
					(*itText)->showText(m_text, m_lifetime, m_font, m_charSize, false, m_colorSecondary);	// show the text
			}
			index++;
		}
		break;
	}
	default:
	{
		m_bFinished = true;
		break;
	}
	}
}