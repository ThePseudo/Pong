#pragma once

#include "SDL_render.h"
#include <SDL.h>

class Pad
{
public:
	explicit Pad(float x, float y, SDL_Renderer *renderer, int r, int g, int b);

	Pad() = delete;

	void Update(uint64_t deltaTime);

	void SetTexture(SDL_Texture *texture);

	void Render() const;

	[[nodiscard]] const SDL_FRect &GetPosition() const;

	void SetPosition(float x, float y);

	[[nodiscard]] int GetScore() const;

	void AddScore();

	void SetYVelocity(float velocity);

private:
	SDL_FRect m_position{0, 0, 60, 200};
	SDL_FPoint m_velocity{0.0f, 0.0f};
	SDL_Renderer *m_renderer;
	SDL_Texture *m_texture;
	int m_r;
	int m_g;
	int m_b;
	int m_score = 0;
};
