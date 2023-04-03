#pragma once
#include "SDL_rect.h"
#include <SDL.h>
#include <functional>

class Trigger
{
public:
	Trigger(SDL_FRect position);
	void SetPosition(SDL_FRect position);
	const SDL_FRect &GetPosition() const;
	void SetActivationFunction(std::function<void(void)> function);
	void Activate() const;
	void SetActivationCondition(std::function<bool(void)> function);
	void SetActive(bool active);

private:
	std::function<void(void)> m_activationFunction = []() {};
	std::function<bool(void)> m_activationCondition = []() { return false; };
	SDL_FRect m_position;
	bool m_active = true;
};
