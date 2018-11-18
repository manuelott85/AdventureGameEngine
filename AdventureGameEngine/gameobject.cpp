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

	CComponent::update(pWindow);	// call parent function

	if (m_pAsset == NULL)
		return;
	sf::Sprite* pSprite = m_pAsset->getSprite();
	if (pSprite != NULL)
	{
		pSprite->setPosition(m_pParentGameObject->m_v2fPosition + m_v2fPosition);
		pSprite->setScale(sf::Vector2f(m_pParentGameObject->m_v2fScale.x * m_v2fScale.x, m_pParentGameObject->m_v2fScale.y * m_v2fScale.y));
		pSprite->setOrigin(m_v2fOrigin);
		pSprite->setRotation(m_pParentGameObject->m_nRotation + m_nRotation);
	}
	m_pAsset->update(pWindow);
}