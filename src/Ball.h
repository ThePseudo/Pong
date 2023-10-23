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

	void InvertYVelocity();

	void InvertXVelocity();

private:
	SDL_FRect m_position{500, 500, 40, 40};
	SDL_FPoint m_velocity{200.0f, 200.0f};
	SDL_Renderer *m_renderer = nullptr;
	SDL_Texture *m_texture = nullptr;
};
