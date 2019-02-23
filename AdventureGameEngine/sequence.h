#pragma once

#include <iostream>
#include <list>
#include <SFML/Graphics.hpp>

class CSequenceAction;
class CGameObject;
class CFontAsset;

// ----------------------------------------------------------------------------

// This is the container for a sequence; if enabled it will disable the input and play through every action step by step until it deactivates itself again
class CSequence
{
public:
	std::string m_name = "";
	bool m_bEnabled = true;
	std::list<CSequenceAction*> m_pActions;

public:
	CSequence();
	void start();
	void update(sf::RenderWindow* pWindow);
};

enum eActionType
{
	nothing, move, say
};

// ----------------------------------------------------------------------------
class CSequenceAction
{
public:
	eActionType m_type = eActionType::nothing;	// which action should be performed
	CGameObject* m_targetObject = NULL;			// object that performs the action
	sf::Vector2f m_moveToVector = { 0,0 };		// in case of a move action, this is the target vector
	CFontAsset* m_font = NULL;					// in case of a say action, this is the font to use
	float m_lifetime = 0;						// in case of a say action, this is the lifetime of the text
	std::string m_text = "";					// in case of a say action, this is the text
};