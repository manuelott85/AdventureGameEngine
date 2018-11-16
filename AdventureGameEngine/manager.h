#pragma once

#include <iostream>

#include "rapidxml.hpp"

class CManager
{
public:
	std::string m_strAssetPath;

private:
	CManager();	// making the constructor private prevent anyone of creating an instance of it, as it is a singleton class
	void readEveryAssetFromXML(rapidxml::xml_node<>* pRootNode);

public:
	// function to access the singleton instance
	static CManager& instance();
	void start(rapidxml::xml_node<>* pRootNode);
};