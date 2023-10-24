#include "Ball.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include <cstdlib>

Ball::Ball(SDL_Renderer *renderer) : m_renderer(renderer)
{
	srand(time(nullptr));
	int num1 = rand() % 2;
	int num2 = rand() % 2;
	m_velocity.x = (num1 > 0) ? m_velocity.x : -m_velocity.x;
	m_velocity.y = (num2 > 0) ? m_velocity.y : -m_velocity.y;
}

void Ball::Render() const
{
	SDL_RenderTexture(m_renderer, m_texture, nullptr, &m_position);
	if (!m_texture)
	{
		SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);
		SDL_RenderFillRect(m_renderer, &m_position);
	}
}

void Ball::Update(const uint64_t deltaTime)
{
	m_position.x += m_velocity.x * (float)deltaTime / 1000.0f;
	m_position.y += m_velocity.y * (float)deltaTime / 1000.0f;
}

const SDL_FRect &Ball::GetPosition() const
{
	return m_position;
}

const SDL_FPoint &Ball::GetVelocity() const
{
	return m_velocity;
}

void Ball::InvertYVelocity()
{
	m_velocity.y *= -1;
}

void Ball::InvertXVelocity()
{
	m_velocity.x *= -1;
}

void Ball::SetPosition(const float x, const float y)
{
	m_position.x = x;
	m_position.y = y;
}

void Ball::SetTexture(SDL_Texture *texture)
{
	m_texture = texture;
}

void Ball::Reset()
{
	// srand(time(nullptr));
	int num1 = rand() % 2;
	int num2 = rand() % 2;
	m_velocity.x = (num1 > 0) ? m_velocity.x : -m_velocity.x;
	m_velocity.y = (num2 > 0) ? m_velocity.y : -m_velocity.y;
	m_position = SDL_FRect{500, 500, 40, 40};
}
