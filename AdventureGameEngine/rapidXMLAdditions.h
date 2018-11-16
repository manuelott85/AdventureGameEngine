#pragma once

class CRapidXMLAdditions
{
public:
	static rapidxml::xml_node<>* findChildNode(rapidxml::xml_node<>* pNode, char* szName);

	static rapidxml::xml_node<>* findChildNodeWithID(rapidxml::xml_node<>* pNode, char* szName, const int &id);

	static rapidxml::xml_attribute<>* findAttribute(rapidxml::xml_node<>* pNode, char* szName);

	static char* getAttributeValue(rapidxml::xml_node<>* pNode, char* szName);

};