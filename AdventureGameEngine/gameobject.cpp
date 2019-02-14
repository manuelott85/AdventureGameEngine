#include "gameobject.h"

// ---------- CGameObject ---------------------------------------------------------------------------------------------------------------

void CGameObject::update(sf::RenderWindow* pWindow)
{
	// for each component
	for (std::list<CComponent*>::iterator it = m_components.begin(); it != m_components.end(); ++it)
	{
		(*it)->update(pWindow);	// call its update function
	}
}

std::string CGameObject::getName() { return m_name; }

// ---------- CComponent ---------------------------------------------------------------------------------------------------------------
void CComponent::update(sf::RenderWindow* pWindow) { }	// function should be overridden by derived class

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
	if (m_pSpriteGeneric == NULL || m_pSpriteHighlight == NULL)
		return;

	m_pParentGameObject->m_v2fPosition = (sf::Vector2f)sf::Mouse::getPosition(*pWindow);	// match cursorSprite to mouse pos

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		m_bCurrentAsset = 1;
	else
		m_bCurrentAsset = 0;

	if (!m_bCurrentAsset)
	{
		// show generic cursor
		std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin();
		(*it)->m_bEnabled = 1;
		it++;
		(*it)->m_bEnabled = 0;
	}
	else
	{
		// show highlight cursor
		std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin();
		(*it)->m_bEnabled = 0;
		it++;
		(*it)->m_bEnabled = 1;
	}
}

// ---------- CMoveToTarget ---------------------------------------------------------------------------------------------------------------
void CMoveToTarget::update(sf::RenderWindow* pWindow)
{
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
		//float sqrMagnitude = direction.x * direction.x + direction.y * direction.y;

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

		//if (sqrMagnitude > 0)	// if moving right- or upwards
		//{
		//	if (direction.x > direction.y)	// movement towards the right is dominant
		//		activateAnimationWithGivenIndex(m_moveRightIndex);
		//	else	// movement towards the top is dominant
		//		activateAnimationWithGivenIndex(m_moveDownIndex);
		//}		
		//else // if moving left- or downwards
		//{
		//	if (direction.x < direction.y)	// movement towards the left is dominant
		//		activateAnimationWithGivenIndex(m_moveLeftIndex);
		//	else	// movement towards the bottom is dominant
		//		activateAnimationWithGivenIndex(m_moveUpIndex);
		//}
	}

	m_v2fLastFramePos = m_pParentGameObject->m_v2fPosition;
}

void CAnimationCtrl::activateAnimationWithGivenIndex(int index)
{
	int currentIndex = 0;
	for (std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin(); it != m_pParentGameObject->m_components.end(); ++it)
	{
		if (currentIndex == index)
			(*it)->m_bEnabled = true;
		else
			(*it)->m_bEnabled = false;
		currentIndex++;
	}
}