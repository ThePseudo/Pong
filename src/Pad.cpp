#include "Pad.h"

Pad::Pad(float x, float y, SDL_Renderer *renderer, int r, int g, int b) : m_renderer(renderer), m_r(r), m_g(g), m_b(b)
{
	m_position.x = x;
	m_position.y = y;
}

void Pad::Update(const uint64_t deltaTime)
{
	m_position.y += (m_velocity.y * static_cast<float>(deltaTime) / 1000.0f);
	m_velocity = SDL_FPoint{0.0f, 0.0f};
}

void Pad::Render() const
{
	SDL_SetRenderDrawColor(m_renderer, m_r, m_g, m_b, 255);
	SDL_RenderFillRect(m_renderer, &m_position);
}

void Pad::AddScore()
{
	m_score++;
}

int Pad::GetScore() const
{
	return m_score;
}

const SDL_FRect &Pad::GetPosition() const
{
	return m_position;
}

void Pad::SetPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
}

void Pad::SetYVelocity(float velocity)
{
	m_velocity.y = velocity;
}

void Pad::SetTexture(SDL_Texture *texture)
{
	m_texture = texture;
}
