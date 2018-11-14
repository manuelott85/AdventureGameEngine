#pragma once

rapidxml::xml_node<>* FindChildNode(rapidxml::xml_node<>* pNode, char* szName);

rapidxml::xml_node<>* FindChildNodeWithID(rapidxml::xml_node<>* pNode, char* szName, const int &id);

rapidxml::xml_attribute<>* FindAttribute(rapidxml::xml_node<>* pNode, char* szName);