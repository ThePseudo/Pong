#pragma once

#include "SDL_render.h"
#include <array>
class Score
{
public:
	Score() = default;
	Score(SDL_Renderer *renderer);
	void AddTexture(SDL_Texture *texture);
	void AddScore(size_t player);
	uint32_t GetScore(size_t player);
	void Render() const;

private:
	SDL_Renderer *m_renderer;
	std::array<uint32_t, 2> m_score = {0, 0};
	std::array<SDL_Texture *, 10> m_textures = {nullptr};
};
