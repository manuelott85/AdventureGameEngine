#include "asset.h"
#include "rapidXMLAdditions.h"

// ---------- CAsset ---------------------------------------------------------------------------------------------------------------

void CAsset::start(CManager *pManager, rapidxml::xml_node<>* pNode) { }	// empty (needs to be overridden by derived class)

void CAsset::update(sf::RenderWindow* pWindow) { }	// empty (needs to be overridden by derived class)

std::string CAsset::getName() const { return m_name; }	// return the name of the asset

// ---------- CSpriteMap ---------------------------------------------------------------------------------------------------------------

void CSpriteMap::start(CManager *pManager, rapidxml::xml_node<>* pNode)
{
	// Load texture and make it available for cutting into sprites
	std::string file = pManager->m_strAssetPath + CRapidXMLAdditions::getAttributeValue(pNode, "src");	// get the path
	m_Texture.loadFromFile(file);	// load texture into memory
}

sf::Texture* CSpriteMap::getTexture() { return &m_Texture; }	// return m_Texture

// ---------- CSpriteAsset ---------------------------------------------------------------------------------------------------------------

// Initialize the Asset
void CSpriteAsset::start(CManager *pManager, rapidxml::xml_node<>* pNode)
{
	m_name = CRapidXMLAdditions::getAttributeValue(pNode, "name");	// get name from XML

	// Load texture and assign to sprite
	std::string file = pManager->m_strAssetPath + pNode->value();
	if (m_Texture.loadFromFile(file))	// load texture into memory
	{
		m_Sprite.setTexture(m_Texture);	// assign it to sprite
	}
}

// calls the drawing function
void CSpriteAsset::update(sf::RenderWindow* pWindow)
{
	pWindow->draw(m_Sprite);	// draw the image
}

sf::Sprite* CSpriteAsset::getSprite() { return &m_Sprite; }	// return the sprite

// ---------- CSpriteMapImageAsset ---------------------------------------------------------------------------------------------------------------

// Initialize the asset (get a texture reference, cut out the image from the texture, and assign it to the sprite)
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

// calls the drawing function
void CSpriteMapImageAsset::update(sf::RenderWindow* pWindow)
{
	pWindow->draw(m_Sprite);	// draw the image
}

// ---------- CSpriteMapAnimationAsset ---------------------------------------------------------------------------------------------------------------

void CSpriteMapAnimationAsset::start(CManager *pManager, rapidxml::xml_node<>* pNode, CSpriteMap* pSpriteMap)
{
	// Initialize the asset(get a texture reference, cut out the image from the texture, and assign it to the sprite)
	CSpriteMapImageAsset::start(pManager, pNode, pSpriteMap);	// call the function from the parent class

	// Steps and Time
	m_nSteps = atoi(CRapidXMLAdditions::getAttributeValue(pNode, "steps"));
	m_nTime = atoi(CRapidXMLAdditions::getAttributeValue(pNode, "time"));

}

void CSpriteMapAnimationAsset::update(sf::RenderWindow* pWindow)
{
	if (m_clockTiming.getElapsedTime().asMilliseconds() >= m_nTime)	// is it time for the next animation frame?
	{
		if (m_pParentGameObject && !m_pParentGameObject->m_bReversePlay)	// play forward
		{
			// calculate next position of the next frame
			int rectLeft = m_nStepIndex * m_Rect.width + m_Rect.left;
			int rectTop = m_Rect.top;

			// in case the next frame would be outside the texture's space,
			// move to the next row and begin from x = 0
			if (rectLeft + m_Rect.width > (int)m_pTexture->getSize().x)
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
			if (rectLeft + m_Rect.width > (int)m_pTexture->getSize().x)
			{
				int temp = (rectLeft + m_Rect.width - m_pTexture->getSize().x) / m_Rect.width;
				rectLeft = (m_nStepIndex - m_nStepIndexFirstRow - temp) * m_Rect.width;	// calculate column include steps left
				m_nStepIndexFirstRow++;
				rectTop += m_Rect.height;	// move to previous row
			}

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

		m_clockTiming.restart();	// restart timer and wait for the next animation frame to show
	}

	pWindow->draw(m_Sprite);	// draw on screen
}