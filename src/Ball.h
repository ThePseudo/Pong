#pragma once

#include "SDL_rect.h"
#include "SDL_render.h"
#include <SDL.h>

class Ball
{
public:
	Ball() = default;

	explicit Ball(SDL_Renderer *renderer);

	void Reset();

	void Render() const;

	void SetTexture(SDL_Texture *texture);

	void Update(uint64_t deltaTime);

	[[nodiscard]] const SDL_FRect &GetPosition() const;

	[[nodiscard]] const SDL_FPoint &GetVelocity() const;

	void SetPosition(float x, float y);

	void SetVelocity(SDL_FPoint velocity);

	SDL_FPoint GetBaseVelocity();

	void InvertYVelocity();

	void InvertXVelocity();

private:
	constexpr static SDL_FPoint BASE_VELOCITY = {400.0f, 400.0f};
	SDL_FRect m_position{500, 500, 40, 40};
	SDL_FPoint m_velocity = BASE_VELOCITY;
	SDL_Renderer *m_renderer = nullptr;
	SDL_Texture *m_texture = nullptr;
};
