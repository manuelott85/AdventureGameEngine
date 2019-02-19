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
	if (m_pSpriteGeneric == NULL || m_pSpriteHighlight == NULL)
		return;

	m_pParentGameObject->m_v2fPosition = (sf::Vector2f)sf::Mouse::getPosition(*pWindow);	// match cursorSprite to mouse pos

	/*if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		m_bCurrentAsset = 1;
	else
		m_bCurrentAsset = 0;*/

	// Check if the cursor collides with an object the player could interact with
	for (std::list<CGameObject*>::iterator it = CManager::instance().m_pActiveScene->m_Interactables.begin(); it != CManager::instance().m_pActiveScene->m_Interactables.end(); ++it)
	{
		if ((*it)->m_interactionComponent)
		{
			if ((*it)->m_interactionComponent->checkCollisionPoint((sf::Vector2f)sf::Mouse::getPosition(*pWindow)))
			{
				switchAppearance(true);	// select the highlight sprite
				break;
			}
			else
				switchAppearance(false);	// select the generic sprite
		}
	}
}

void CCursorComponent::switchAppearance(bool showHightlichtCursor)
{
	if (showHightlichtCursor)
	{
		// show highlight cursor
		std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin();
		(*it)->m_bEnabled = 0;
		it++;
		(*it)->m_bEnabled = 1;
	}
	else
	{
		// show generic cursor
		std::list<CComponent*>::iterator it = m_pParentGameObject->m_components.begin();
		(*it)->m_bEnabled = 1;
		it++;
		(*it)->m_bEnabled = 0;
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
			(*it)->m_bEnabled = false;

		currentIndex++;
	}
}

// ---------- CInteractionComponent ---------------------------------------------------------------------------------------------------------------
void CInteractionComponent::update(sf::RenderWindow* pWindow)
{
	// get the bounding box of the largest currently active drawable object (currently sf::Sprite only)
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