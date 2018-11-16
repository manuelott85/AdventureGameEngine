#include "asset.h"
#include "rapidXMLAdditions.h"

// ---------- CAsset ----------

void CAsset::start(CManager *pManager, rapidxml::xml_node<>* pNode)
{

}

void CAsset::update(sf::RenderWindow* pWindow)
{

}

std::string CAsset::getName() const { return m_name; }

// ---------- CSprite ----------

void CSprite::start(CManager *pManager, rapidxml::xml_node<>* pNode)
{
	CAsset::start(pManager, pNode);	// call parent function

	m_name = CRapidXMLAdditions::getAttributeValue(pNode, "name");	// get name from XML

	// Load texture and asign to sprite
	std::string file = pManager->m_strAssetPath + pNode->value();
	if (m_Texture.loadFromFile(file)) {
		m_Sprite.setTexture(m_Texture);
	}
}

void CSprite::update(sf::RenderWindow* pWindow)
{
	CAsset::update(pWindow);	// call parent function
	//m_Sprite.setScale(sf::Vector2f(m_v2fScale.x, m_v2fScale.y));
	//m_Sprite.setRotation(m_nRotation);
	//m_Sprite.setOrigin(sf::Vector2f(m_v2fOrigin.x * m_Texture.getSize().x, m_v2fOrigin.y * m_Texture.getSize().y));
	pWindow->draw(m_Sprite);
}

// ---------- CSpriteMap ----------

void CSpriteMap::start(CManager *pManager, rapidxml::xml_node<>* pNode)
{
	// Load texture and asign to sprite
	std::string file = pManager->m_strAssetPath + CRapidXMLAdditions::getAttributeValue(pNode, "src");
	m_Texture.loadFromFile(file);
}

sf::Texture* CSpriteMap::getTexture() { return &m_Texture; }

// ---------- CSpriteMapImage ----------

void CSpriteMapImage::start(CManager *pManager, rapidxml::xml_node<>* pNode, CSpriteMap* pSpriteMap)
{
	CAsset::start(pManager, pNode);	// call parent function

	m_name = CRapidXMLAdditions::getAttributeValue(pNode, "name");	// get name from XML

	m_pTexture = pSpriteMap->getTexture();	// get texture
	m_Sprite.setTexture(*m_pTexture);	// assign texture

	// Offset
	m_Rect.left = atoi(CRapidXMLAdditions::getAttributeValue(pNode, "xOffset"));
	m_Rect.top = atoi(CRapidXMLAdditions::getAttributeValue(pNode, "yOffset"));

	// Width and Height as Rect
	m_Rect.width = atoi(CRapidXMLAdditions::getAttributeValue(pNode, "width"));
	m_Rect.height = atoi(CRapidXMLAdditions::getAttributeValue(pNode, "height"));

	m_Sprite.setTextureRect(m_Rect);	// cut once to initialise the sprite
}

void CSpriteMapImage::update(sf::RenderWindow* pWindow)
{
	CAsset::update(pWindow);	// call parent function

	pWindow->draw(m_Sprite);
}