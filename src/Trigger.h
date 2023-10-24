#pragma once
#include "SDL_rect.h"
#include <SDL.h>
#include <functional>

class Trigger
{
public:
	Trigger() = default;
	void SetActivationFunction(std::function<void(void)> function);
	void Activate() const;
	void SetActivationCondition(std::function<bool(void)> function);
	void SetActive(bool active);

private:
	std::function<void(void)> m_activationFunction = []() {};
	std::function<bool(void)> m_activationCondition = []() { return false; };
	bool m_active = true;
};
