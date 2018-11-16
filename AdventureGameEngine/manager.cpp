#include "rapidxml.hpp"

#include "manager.h"
#include "rapidXMLAdditions.h"

CManager::CManager()
{

}

CManager& CManager::instance()
{
	static CManager* instance = new CManager();
	return *instance;
}

void CManager::start(rapidxml::xml_node<>* pRootNode)
{
	readEveryAssetFromXML(pRootNode);
}

void CManager::readEveryAssetFromXML(rapidxml::xml_node<>* pRootNode)
{
	// temporary pointer
	rapidxml::xml_node<>* pAssets;
	rapidxml::xml_attribute<>* pAttr;

	// Create an entity for every asset in the XML
	pAssets = CRapidXMLAdditions::findChildNode(pRootNode, "assets");
	if (pAssets != NULL)
	{
		// Save the path into the manager
		CManager::instance().m_strAssetPath = CRapidXMLAdditions::getAttributeValue(pAssets, "path");

		// read out every of type image and create an asset
		for (rapidxml::xml_node<>* pChild = pAssets->first_node(); pChild != NULL; pChild = pChild->next_sibling())
		{
			// if node is of type sprite
			if (strcmp(pChild->name(), "sprite") == 0)
			{
				//CImage *pImage = new CImage();			// create the asset's object
				//pImage->Start(&manager, pChild);		// call its start function
				//manager.m_Entities.push_back(pImage);	// make it available in the manager
			}

		//	// if node is of type image map
		//	if (strcmp(pChild->name(), "imgmap") == 0)
		//	{
		//		CImageMap *pImagemap = new CImageMap();			// create the asset's object
		//		pImagemap->Start(&manager, pChild);				// call its start function

		//		for (xml_node<>* pSubChild = pChild->first_node(); pSubChild != NULL; pSubChild = pSubChild->next_sibling())
		//		{
		//			// if static image
		//			if (strcmp(pSubChild->name(), "subimg") == 0)
		//			{
		//				CSubImage *pImage = new CSubImage();			// create the asset's object
		//				pImage->Start(&manager, pImagemap, pSubChild);	// call its start function
		//				manager.m_Entities.push_back(pImage);			// make it available in the manager
		//			}

		//			// if animation
		//			if (strcmp(pSubChild->name(), "animation") == 0)
		//			{
		//				CAnimation *pImage = new CAnimation();			// create the asset's object
		//				pImage->Start(&manager, pImagemap, pSubChild);		// call its start function
		//				manager.m_Entities.push_back(pImage);	// make it available in the manager
		//			}
		//		}
		//	}
		}
	}
}