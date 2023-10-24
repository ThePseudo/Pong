#pragma once

#include "SDL_rect.h"
#include "SDL_render.h"
class MiddleBar
{
public:
	MiddleBar() = default;
	explicit MiddleBar(SDL_Renderer *renderer);
	void SetTexture(SDL_Texture *texture);
	void Render(SDL_Point screenSize) const;

private:
	constexpr static SDL_Point SIZE = SDL_Point{40, 60};
	SDL_Renderer *m_renderer = nullptr;
	SDL_Texture *m_texture = nullptr;
};
