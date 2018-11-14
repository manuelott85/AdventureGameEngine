#include <fstream>
#include "rapidxml.hpp"

rapidxml::xml_node<>* FindChildNode(rapidxml::xml_node<>* pNode, char* szName)
{
	for (rapidxml::xml_node<>* pChild = pNode->first_node(); pChild != NULL; pChild = pChild->next_sibling()) {
		if (strcmp(pChild->name(), szName) == 0) return pChild;
	}
	return NULL;
}

rapidxml::xml_attribute<>* FindAttribute(rapidxml::xml_node<>* pNode, char* szName)
{
	for (rapidxml::xml_attribute<>* pAttr = pNode->first_attribute(); pAttr != NULL; pAttr = pAttr->next_attribute())
	{
		if (strcmp(pAttr->name(), szName) == 0) return pAttr;
	}
	return NULL;
}

rapidxml::xml_node<>* FindChildNodeWithID(rapidxml::xml_node<>* pNode, char* szName, const int &id)
{
	for (rapidxml::xml_node<>* pChild = pNode->first_node(); pChild != NULL; pChild = pChild->next_sibling())
	{
		if (strcmp(pChild->name(), szName) == 0)
		{
			char buffer[65];
			rapidxml::xml_attribute<>* pAttr = FindAttribute(pChild, "id");
			if (atoi(pAttr->value()) == id)
				return pChild;
		}
	}
	return NULL;
}