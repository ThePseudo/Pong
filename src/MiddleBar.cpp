#include "MiddleBar.h"
#include "SDL_rect.h"
#include "SDL_render.h"

MiddleBar::MiddleBar(SDL_Renderer *renderer) : m_renderer(renderer)
{
}

void MiddleBar::SetTexture(SDL_Texture *texture)
{
	m_texture = texture;
}

void MiddleBar::Render(SDL_Point screenSize) const
{
	for (float i = 30; i < screenSize.y; i += 100)
	{
		const SDL_FRect position = {static_cast<float>(screenSize.x / 2.0 - SIZE.x / 2.0), i, SIZE.x, SIZE.y};
		SDL_RenderTexture(m_renderer, m_texture, nullptr, &position);
	}
}
