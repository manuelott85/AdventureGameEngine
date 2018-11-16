#pragma once

#include <string>
#include <SFML/Graphics.hpp>

#include "asset.h"

class CAsset;

class CGameObject
{
public:
	std::string m_name;
	sf::Vector2f m_v2fPosition;
	sf::Vector2f m_v2fScale;
	sf::Vector2f m_v2fOrigin;
	float m_nRotation;
	CAsset* m_pAsset;
	bool m_bEnabled = true;

public:
	void update(sf::RenderWindow* pWindow);
};