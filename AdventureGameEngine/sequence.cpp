#include "sequence.h"
#include "manager.h"

// ---------- CSequence ---------------------------------------------------------------------------------------------------------------
CSequence::CSequence()
{

}

void CSequence::update(sf::RenderWindow* pWindow)
{
	// only process sequences that are enabled
	if (!m_bEnabled)
		return;

	CManager::instance().m_bInputDisabled = true;

	// Perform actions as long as there are any
	if (m_pActions.size() > 0)
	{
		// as long as the action is not finished, update it
		if (!(*m_pActions.begin())->m_bFinished)
		{
			// only update the most current one
			(*m_pActions.begin())->update(pWindow);
		}
		// otherwise delete the current action and prepare for the next one
		else
			m_pActions.pop_front();	// remove the first entry
	}
	else	// or disable the sequence
	{
		m_bEnabled = false;
		CManager::instance().m_bInputDisabled = false;
	}
}

// ---------- CSequenceAction ---------------------------------------------------------------------------------------------------------------
void CSequenceAction::update(sf::RenderWindow* pWindow)
{
	// switch on action type
	switch (m_type) {
	case eActionType::move :
	{
		float distance = 0;
		sf::Vector2f distanceVector = { 0,0 };
		CManager::instance().m_pActiveScene->m_playerMoveToTarget->m_v2fPosition = m_moveToVector;
		distanceVector = CManager::instance().m_pActiveScene->m_player->m_v2fPosition - CManager::instance().m_pActiveScene->m_playerMoveToTarget->m_v2fPosition;
		distance = sqrt(distanceVector.x * distanceVector.x + distanceVector.y * distanceVector.y);
		if (distance < 10)
			m_bFinished = true;
		break;
	}
	case eActionType::say :
	{
		int index = 0;
		for (std::list<CTextbox*>::iterator itText = m_targetObject->m_pTextComponents.begin(); itText != m_targetObject->m_pTextComponents.end(); ++itText)
		{
			if (!(*itText)->m_bEnabled)
			{
				if (index == m_targetObject->m_pTextComponents.size() - 1)
					(*itText)->showText(m_text, m_lifetime, m_font, m_charSize, true);	// show the text
				else
					(*itText)->showText(m_text, m_lifetime, m_font, m_charSize, false);	// show the text
			}
			index++;
		}
		break;
	}
	default:
	{
		m_bFinished = true;
		break;
	}
	}
}