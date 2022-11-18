#pragma once

#include <SDL.h>

class Ball
{
public:
	Ball() = delete;

	explicit Ball(SDL_Renderer* renderer);

	void Render() const;

	void Update(uint64_t deltaTime);

	[[nodiscard]] SDL_FRect GetPosition() const;

	void SetPosition(float x, float y);

	void InvertYVelocity();

	void InvertXVelocity();

private:

	SDL_FRect m_position{ 500, 500, 40, 40 };
	SDL_FPoint m_velocity{ 200.0f, 200.0f };
	SDL_Renderer* m_renderer;
};
