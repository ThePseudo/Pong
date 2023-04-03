#include "Trigger.h"

Trigger::Trigger(SDL_FRect position) : m_position(position)
{
}

const SDL_FRect &Trigger::GetPosition() const
{
	return m_position;
}

void Trigger::SetPosition(SDL_FRect position)
{
	m_position = position;
}

void Trigger::SetActivationFunction(std::function<void(void)> function)
{
	m_activationFunction = function;
}

void Trigger::Activate() const
{
	if (m_active && m_activationCondition())
	{
		m_activationFunction();
	}
}

void Trigger::SetActivationCondition(std::function<bool(void)> function)
{
	m_activationCondition = function;
}

void Trigger::SetActive(bool active)
{
	m_active = active;
}
