#include "gameobject.h"
#include "interpolation.h"

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

// ---------- CPlayerMoveToTarget ---------------------------------------------------------------------------------------------------------------
void CMoveToTarget::update(sf::RenderWindow* pWindow)
{
	if (m_lastOrderPos != m_pParentGameObject->m_v2fPosition)
	{
		// Debug Message
		std::cout << m_lastOrderPos.x << "|" << m_lastOrderPos.y << " updated to: " << m_pParentGameObject->m_v2fPosition.x << "|" << m_pParentGameObject->m_v2fPosition.y << std::endl;

		m_lastOrderPos = m_pParentGameObject->m_v2fPosition;	// Set lastOrderPos
		m_clockTiming.restart();	// restart the timer

		m_lastFramePos = m_objectToMove->m_v2fPosition;	// initialize movement
	}

	if (m_objectToMove->m_v2fPosition != m_pParentGameObject->m_v2fPosition)
	{
		std::vector<double> xData = { m_objectToMove->m_v2fPosition.x, m_pParentGameObject->m_v2fPosition.x };
		std::vector<double> yData = { m_objectToMove->m_v2fPosition.y, m_pParentGameObject->m_v2fPosition.y };
		float movementSpeed = 0.001;

		//float x = m_clockTiming.getElapsedTime().asSeconds() * movementSpeed + m_objectToMove->m_v2fPosition.x;
		m_lastFramePos.y = interpolate(xData, yData, m_lastFramePos.x, true);
		m_objectToMove->m_v2fPosition = m_lastFramePos;
		std::cout << "Current MoveCoord: " << m_lastFramePos.x << "|" << m_lastFramePos.y << std::endl;
		m_lastFramePos.x = m_lastFramePos.x + m_clockTiming.getElapsedTime().asSeconds() * movementSpeed;
	}
}
