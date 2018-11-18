#include "gameobject.h"

// ---------- CGameObject ----------

void CGameObject::update(sf::RenderWindow* pWindow)
{
	for (std::list<CComponent*>::iterator it = m_components.begin(); it != m_components.end(); ++it)
	{
		(*it)->update(pWindow);
	}
}

// ---------- CComponent ----------
void CComponent::update(sf::RenderWindow* pWindow)
{
	
}

// ---------- CSpriteComponent ----------

void CSpriteComponent::update(sf::RenderWindow* pWindow)
{
	// skip update if the component is not active
	if (!m_bEnabled)
		return;

	// skip if there is no asset assigned
	// as the type of m_pAsset differ in CSpriteComponent and CAnimationComponent, each class need its own code
	if (m_pAsset == NULL)
		return;

	sf::Sprite* pSprite = m_pAsset->getSprite();
	if (pSprite != NULL)
	{
		pSprite->setOrigin(m_v2fOrigin);
		pSprite->setPosition(m_pParentGameObject->m_v2fPosition + m_v2fPosition);
		pSprite->setScale(sf::Vector2f(m_pParentGameObject->m_v2fScale.x * m_v2fScale.x, m_pParentGameObject->m_v2fScale.y * m_v2fScale.y));
		pSprite->setRotation(m_pParentGameObject->m_nRotation + m_nRotation);
	}
	m_pAsset->update(pWindow);
}

// ---------- CAnimationComponent ----------

void CAnimationComponent::update(sf::RenderWindow* pWindow)
{
	// skip update if the component is not active
	if (!m_bEnabled)
		return;

	// skip if there is no asset assigned
	// as the type of m_pAsset differ in CSpriteComponent and CAnimationComponent, each class need its own code
	if (m_pAsset == NULL)
		return;

	m_pAsset->m_pParentGameObject = this;
	sf::Sprite* pSprite = m_pAsset->getSprite();
	if (pSprite != NULL)
	{

		sf::Vector2f scale;

		pSprite->setOrigin(m_v2fOrigin);

		sf::Vector2f position = m_pParentGameObject->m_v2fPosition + m_v2fPosition;
		if (m_bMirrorX)
			position.x = position.x + (m_pAsset->getSprite()->getTextureRect().width * m_v2fScale.x);
		if (m_bMirrorY)
			position.y = position.y + (m_pAsset->getSprite()->getTextureRect().height * m_v2fScale.y);

		pSprite->setPosition(position);

		// Mirror Sprite
		scale.x = m_bMirrorX ? -(m_pParentGameObject->m_v2fScale.x * m_v2fScale.x) : m_pParentGameObject->m_v2fScale.x * m_v2fScale.x;
		scale.y = m_bMirrorY ? -(m_pParentGameObject->m_v2fScale.y * m_v2fScale.y) : m_pParentGameObject->m_v2fScale.y * m_v2fScale.y;
		pSprite->setScale(scale);

		pSprite->setRotation(m_pParentGameObject->m_nRotation + m_nRotation);
	}
	m_pAsset->update(pWindow);
}