#include "inventory.h"

// ---------- CInventoryContainer ---------------------------------------------------------------------------------------------------------------

CInventoryContainer& CInventoryContainer::instance()
{
	static CInventoryContainer* instance = new CInventoryContainer();
	return *instance;
}

void CInventoryContainer::start()
{
	m_pInventoryItems.clear();
}

void CInventoryContainer::addItemToInventory(std::string name, std::string description)
{
	CInventoryItem* pItem = new CInventoryItem(name, description);	// create the item itself
	m_pInventoryItems.push_back(pItem);	// store it in the inventory
}

// ---------- CInventoryItem ---------------------------------------------------------------------------------------------------------------
CInventoryItem::CInventoryItem(std::string name, std::string description)
{
	m_name = name;
	m_description = description;
}