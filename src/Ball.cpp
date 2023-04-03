#include "Ball.h"
#include <cstdlib>
#include <random>

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
	SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);
	SDL_RenderFillRectF(m_renderer, &m_position);
}

void Ball::Update(const uint64_t deltaTime)
{
	m_position.x += m_velocity.x * (float)deltaTime / 1000.0f;
	m_position.y += m_velocity.y * (float)deltaTime / 1000.0f;
}

SDL_FRect Ball::GetPosition() const
{
	return m_position;
}

void Ball::InvertYVelocity()
{
	m_velocity.y *= -1;
}

void Ball::SetPosition(const float x, const float y)
{
	m_position.x = x;
	m_position.y = y;
}

void Ball::InvertXVelocity()
{
	m_velocity.x *= -1;
}
