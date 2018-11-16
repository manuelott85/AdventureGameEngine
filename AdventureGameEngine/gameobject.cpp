#include "gameobject.h"

void CGameObject::update(sf::RenderWindow* pWindow)
{
	//m_pAsset->GetSprite()->setPosition(m_vPos);
	if(m_pAsset != NULL)
		m_pAsset->update(pWindow);
}