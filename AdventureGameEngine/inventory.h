#pragma once

#include <iostream>
#include <list>

class CInventoryItem;

// ----------------------------------------------------------------------------
// This is the container for every inventory item and functions to manipulate and use them
class CInventoryContainer
{
public:
	std::list<CInventoryItem*> m_pInventoryItems;

private:
	CInventoryContainer();	// making the constructor private prevent anyone of creating an instance of it, as it is a singleton class

public:
	static CInventoryContainer& instance();	// function to access the singleton instance
	void start();
	void addItemToInventory(std::string name, std::string description);
};

// ----------------------------------------------------------------------------
// Every picked up asset will be converted into this item and than added to the inventory container
class CInventoryItem
{
public:
	std::string m_name = "";
	std::string m_description = "";

public:
	CInventoryItem(std::string name, std::string description);
};