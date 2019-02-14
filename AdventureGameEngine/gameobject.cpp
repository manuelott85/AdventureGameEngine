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
		float magnitude = sqrt(v2fDirection.x * v2fDirection.x + v2fDirection.y * v2fDirection.y);	// calculate the magnitude
		v2fDirection = v2fDirection / magnitude;	// normalize the vector
		float movementspeed = m_moveSpeed * CManager::instance().m_deltaTime;	// calculate the new lenght of the vector
		v2fDirection = { v2fDirection.x * movementspeed, v2fDirection.y * movementspeed };	// apply the new lenght to the vector (distance to move per frame)
		v2fDirection = m_objectToMove->m_v2fPosition + v2fDirection;	// add everything up to get the next frame's position
		m_objectToMove->m_v2fPosition = v2fDirection;	// perform the move action
	}
}
