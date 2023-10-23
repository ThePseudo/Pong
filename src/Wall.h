#pragma once

#include "SDL.h"
#include "SDL_render.h"
class Wall
{
public:
	Wall() = default;
	explicit Wall(SDL_Renderer *renderer, SDL_FRect position);
	void Render() const;
	const SDL_FRect &GetPosition() const;

private:
	SDL_FRect m_position;
	SDL_Renderer *m_renderer;
};
