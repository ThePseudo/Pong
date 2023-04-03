#pragma once

#include "SDL.h"
#include "SDL_render.h"
class Wall
{
public:
	Wall(SDL_Renderer *renderer, SDL_FRect position);
	void Render() const;
	const SDL_FRect &GetPosition() const;

private:
	const SDL_FRect m_position;
	SDL_Renderer *m_renderer;
};
