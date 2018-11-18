#include "asset.h"
#include "rapidXMLAdditions.h"

// ---------- CSpriteMap ----------

void CSpriteMap::start(CManager *pManager, rapidxml::xml_node<>* pNode)
{
	// Load texture and asign to sprite
	std::string file = pManager->m_strAssetPath + CRapidXMLAdditions::getAttributeValue(pNode, "src");
	m_Texture.loadFromFile(file);
}

sf::Texture* CSpriteMap::getTexture() { return &m_Texture; }

// ---------- CAsset ----------

void CAsset::start(CManager *pManager, rapidxml::xml_node<>* pNode)
{

}

void CAsset::update(sf::RenderWindow* pWindow)
{

}

std::string CAsset::getName() const { return m_name; }
sf::Sprite* CAsset::getSprite() { return NULL; }

// ---------- CSpriteAsset ----------

void CSpriteAsset::start(CManager *pManager, rapidxml::xml_node<>* pNode)
{
	m_name = CRapidXMLAdditions::getAttributeValue(pNode, "name");	// get name from XML

	// Load texture and asign to sprite
	std::string file = pManager->m_strAssetPath + pNode->value();
	if (m_Texture.loadFromFile(file)) {
		m_Sprite.setTexture(m_Texture);
	}
}

void CSpriteAsset::update(sf::RenderWindow* pWindow)
{
	pWindow->draw(m_Sprite);
}

sf::Sprite* CSpriteAsset::getSprite() { return &m_Sprite; }

// ---------- CSpriteMapImageAsset ----------

void CSpriteMapImageAsset::start(CManager *pManager, rapidxml::xml_node<>* pNode, CSpriteMap* pSpriteMap)
{
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

void CSpriteMapImageAsset::update(sf::RenderWindow* pWindow)
{
	pWindow->draw(m_Sprite);
}

// ---------- CSpriteMapAnimationAsset ----------

void CSpriteMapAnimationAsset::start(CManager *pManager, rapidxml::xml_node<>* pNode, CSpriteMap* pSpriteMap)
{
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

	// Steps and Time
	m_nSteps = atoi(CRapidXMLAdditions::getAttributeValue(pNode, "steps"));
	m_nTime = atoi(CRapidXMLAdditions::getAttributeValue(pNode, "time"));

}

void CSpriteMapAnimationAsset::update(sf::RenderWindow* pWindow)
{
	if (m_clockTiming.getElapsedTime().asMilliseconds() >= m_nTime)	// is it time for the next animation frame?
	{
		if (!m_pParentGameObject->m_bReversePlay)	// play forward
		{
			// calculate next position of the next frame
			int rectLeft = m_nStepIndex * m_Rect.width + m_Rect.left;
			int rectTop = m_Rect.top;

			// in case the next frame would be outside the texture's space,
			// move to the next row and begin from x = 0
			if (rectLeft + m_Rect.width > m_pTexture->getSize().x)
			{
				rectLeft = (m_nStepIndex - m_nStepIndexFirstRow) * m_Rect.width;	// calculate column include steps left
				rectTop += m_Rect.height;	// move to next row
			}
			else
				m_nStepIndexFirstRow++;	// increase variable that stores the amount of frames of the first row

			// Apply the frame to the sprite
			m_Sprite.setTextureRect(sf::IntRect(rectLeft, rectTop, m_Rect.width, m_Rect.height));

			// Reset counter if needed
			m_nStepIndex++;
			if (m_nStepIndex > m_nSteps - 1)
			{
				m_nStepIndex = 0;
				m_nStepIndexFirstRow = 0;
			}
		}
		else    // play backward
		{
			// calculate next position of the next frame
			int rectLeft = m_nStepIndex * m_Rect.width + m_Rect.left;
			int rectTop = m_Rect.top;

			// in case the next frame would be outside the texture's space,
			// move to the previous row and begin from x = 0
			//if (rectLeft - m_Rect.width < 0)
			//if (true)
			if (rectLeft + m_Rect.width > m_pTexture->getSize().x)
			{
				float temp = (rectLeft + m_Rect.width - m_pTexture->getSize().x) / m_Rect.width;
				rectLeft = (m_nStepIndex - m_nStepIndexFirstRow - temp) * m_Rect.width;	// calculate column include steps left
				m_nStepIndexFirstRow++;
				rectTop += m_Rect.height;	// move to previous row
			}
			//else
			//	m_nStepIndexFirstRow++;	// increase variable that stores the amount of frames of the first row

			// Apply the frame to the sprite
			m_Sprite.setTextureRect(sf::IntRect(rectLeft, rectTop, m_Rect.width, m_Rect.height));

			// Reset counter if needed
			m_nStepIndex--;
			if (m_nStepIndex < 0)
			{
				m_nStepIndex = m_nSteps - 1;
				m_nStepIndexFirstRow = 0;
			}
		}

		m_clockTiming.restart();
	}

	pWindow->draw(m_Sprite);
}