#include "Score.h"
#include "SDL_rect.h"
#include "SDL_render.h"

Score::Score(SDL_Renderer *renderer) : m_renderer(renderer)
{
}

void Score::AddTexture(SDL_Texture *texture)
{
	static size_t i = 0;
	m_textures[i] = texture;
	i++;
	i = i % m_textures.size();
}

void Score::Render() const
{
	constexpr static float BASE_POSITION = 350.0f;
	constexpr static float OFFSET = 650.0f;
	constexpr static SDL_FPoint SIZE = SDL_FPoint{40, 60};
	constexpr static float SPACING = 10.0f;
	for (size_t i = 0; i < m_score.size(); i++)
	{
		auto score = m_score[i];
		uint32_t count = 0;
		if (!score % 10)
		{
			SDL_FRect position = SDL_FRect{BASE_POSITION + OFFSET * i, 100.0f, SIZE.x, SIZE.y};
			SDL_RenderTexture(m_renderer, m_textures[0], nullptr, &position);
			score = score / 10;
			count++;
		}
		while (score)
		{
			auto digit = score % 10;
			score = score / 10;
			SDL_FRect position =
				SDL_FRect{BASE_POSITION + OFFSET * i - count * (SPACING + SIZE.x), 100.0f, SIZE.x, SIZE.y};
			SDL_RenderTexture(m_renderer, m_textures[digit], nullptr, &position);
			count++;
		}
	}
}

void Score::AddScore(size_t player)
{
	m_score[player]++;
}

uint32_t Score::GetScore(size_t player)
{
	return m_score[player];
}
