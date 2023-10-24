#include "Wall.h"
#include "SDL_render.h"

Wall::Wall(SDL_Renderer *renderer, SDL_FRect position) : m_renderer(renderer), m_position(position)
{
}

void Wall::Render() const
{
	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(m_renderer, &m_position);
}

const SDL_FRect &Wall::GetPosition() const
{
	return m_position;
}
