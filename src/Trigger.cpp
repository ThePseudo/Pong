#include "Trigger.h"

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
