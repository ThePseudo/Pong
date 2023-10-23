#include "App.h"
#include "SDL3_image/SDL_image.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "SDL_video.h"
#include <iostream>
#include <memory>

void App::Init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	m_window = SDL_CreateWindow("Pong", DEFAULT_WIDTH, DEFAULT_HEIGHT, 0);
	m_renderer = SDL_CreateRenderer(m_window, nullptr, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	m_ball = Ball(m_renderer);
	m_pad1 = Pad(100, (DEFAULT_HEIGHT - 100) / 2, m_renderer, 255, 0, 0);
	m_pad2 = Pad(DEFAULT_WIDTH - 140, (DEFAULT_HEIGHT - 100) / 2, m_renderer, 0, 255, 0);
	m_ball.SetTexture(LoadTexture("images/ball.png"));
	m_pad1.SetTexture(LoadTexture("images/paddlegreen.png"));
	m_pad2.SetTexture(LoadTexture("images/paddlered.png"));

	m_walls[0] = Wall(m_renderer, SDL_FRect{0, 0, DEFAULT_WIDTH, 10});
	m_walls[1] = Wall(m_renderer, SDL_FRect{0, DEFAULT_HEIGHT - 10, DEFAULT_WIDTH, 10});
	m_triggers.push_back(Trigger(SDL_FRect{0, 0, 10, DEFAULT_HEIGHT}));
	m_triggers.push_back(Trigger(SDL_FRect{DEFAULT_WIDTH - 10, 0, 10, DEFAULT_HEIGHT}));

	m_triggers[0].SetActivationFunction([&]() {
		m_pad2.AddScore();
		std::cout << "Score pad 1: " << m_pad1.GetScore() << std::endl;
		std::cout << "Score pad 2: " << m_pad2.GetScore() << std::endl;
		m_ball.Reset();
	});

	m_triggers[1].SetActivationFunction([&]() {
		m_pad1.AddScore();
		std::cout << "Score pad 1: " << m_pad1.GetScore() << std::endl;
		std::cout << "Score pad 2: " << m_pad2.GetScore() << std::endl;
		m_ball.Reset();
	});

	m_triggers[0].SetActivationCondition([&]() {
		SDL_FRect temp;
		return SDL_GetRectIntersectionFloat(&m_ball.GetPosition(), &m_triggers[0].GetPosition(), &temp);
	});

	m_triggers[1].SetActivationCondition([&]() {
		SDL_FRect temp;
		return SDL_GetRectIntersectionFloat(&m_ball.GetPosition(), &m_triggers[1].GetPosition(), &temp);
	});

	m_oldTime = SDL_GetTicks();
}

void App::Run()
{
	SDL_Event event;
	bool shouldRun = true;
	while (true)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				shouldRun = false;
			}
			if (event.type == SDL_EVENT_KEY_DOWN)
			{
				if (event.key.keysym.sym == SDLK_DOWN)
				{
					m_downPressed = true;
				}
				else if (event.key.keysym.sym == SDLK_UP)
				{
					m_upPressed = true;
				}
			}
			if (event.type == SDL_EVENT_KEY_UP)
			{
				if (event.key.keysym.sym == SDLK_DOWN)
				{
					m_downPressed = false;
				}
				else if (event.key.keysym.sym == SDLK_UP)
				{
					m_upPressed = false;
				}
			}
		}
		if (!shouldRun)
			break;
		m_newTime = SDL_GetTicks();
		ApplyUserInput();
		ApplyAI();
		Update(m_newTime - m_oldTime);
		CheckAndApplyCollisions();
		Render();
		int64_t toWait = 16 - static_cast<int64_t>(m_newTime - m_oldTime);
		m_oldTime = m_newTime;
		if (toWait > 0)
		{
			SDL_Delay(toWait / 2);
		}
	}
}

void App::Destroy()
{
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void App::Render()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);
	m_pad1.Render();
	m_pad2.Render();
	m_ball.Render();
	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
	int width, height;
	SDL_GetWindowSize(m_window, &width, &height);
	for (const auto &wall : m_walls)
	{
		wall.Render();
	}
	SDL_RenderPresent(m_renderer);
}

void App::Update(uint64_t deltaTime)
{
	m_pad1.Update(deltaTime);
	m_pad2.Update(deltaTime);
	m_ball.Update(deltaTime);
}

void App::CheckAndApplyCollisions()
{
	CheckAndApplyBallCollisions();
	CheckAndApplyPadCollisions(m_pad1);
	CheckAndApplyPadCollisions(m_pad2);
	for (size_t i = 0; i < m_triggers.size(); i++)
	{
		m_triggers[i].Activate();
	}
}

void App::CheckAndApplyPadCollisions(Pad &pad)
{
	constexpr double deltaTime = 0.01;
	auto position = pad.GetPosition();
	SDL_FRect collisionRect;
	SDL_FRect ballPosition = m_ball.GetPosition();
	for (const auto &wall : m_walls)
	{
		if (SDL_GetRectIntersectionFloat(&pad.GetPosition(), &wall.GetPosition(), &collisionRect))
		{
			if (pad.GetPosition().y >= collisionRect.y)
			{
				pad.SetPosition(pad.GetPosition().x, pad.GetPosition().y + collisionRect.h);
			}
			else
			{
				pad.SetPosition(pad.GetPosition().x, pad.GetPosition().y - collisionRect.h);
			}
		}
	}
	// If ball intersects pad
	if (SDL_GetRectIntersectionFloat(&position, &ballPosition, &collisionRect) == SDL_TRUE)
	{
		if ((position.x > static_cast<float>(DEFAULT_WIDTH) / 2.0f && m_ball.GetVelocity().x > 0) ||
			(position.x < static_cast<float>(DEFAULT_WIDTH) / 2.0f && m_ball.GetVelocity().x < 0)) // RIGHT SIDE
		{
			m_ball.InvertXVelocity();
			// If next ball position .y collides with pad
			auto nextBallPosition = ballPosition;
			nextBallPosition.x += deltaTime * m_ball.GetVelocity().x;
			nextBallPosition.y += deltaTime * m_ball.GetVelocity().y;
			SDL_GetRectIntersectionFloat(&position, &nextBallPosition, &collisionRect);
			if (collisionRect.w > 0.001 &&
				(collisionRect.y <= position.y || collisionRect.y + collisionRect.h >= position.y + position.h))
			{
				m_ball.InvertYVelocity();
			}
		}
	}
}

void App::CheckAndApplyBallCollisions()
{
	auto position = m_ball.GetPosition();
	SDL_FRect collisionRect;
	for (const auto &wall : m_walls)
	{
		if (SDL_GetRectIntersectionFloat(&position, &wall.GetPosition(), &collisionRect))
		{
			m_ball.InvertYVelocity();
			if (position.y >= collisionRect.y)
			{
				m_ball.SetPosition(position.x, position.y + collisionRect.h);
			}
			else
			{
				m_ball.SetPosition(position.x, position.y - collisionRect.h);
			}
		}
	}
}

void App::ApplyAI()
{
	auto position = m_pad2.GetPosition();
	auto ballPos = m_ball.GetPosition();
	if ((ballPos.y + ballPos.h / 2.0f) < position.y)
	{
		m_pad2.SetYVelocity(-m_settings.padVelocityY);
	}
	else if ((ballPos.y + ballPos.h / 2.0f) > position.y + position.h)
	{
		m_pad2.SetYVelocity(m_settings.padVelocityY);
	}
}

void App::ApplyUserInput()
{
	if (m_downPressed)
	{
		m_pad1.SetYVelocity(m_settings.padVelocityY);
	}
	if (m_upPressed)
	{
		m_pad1.SetYVelocity(-m_settings.padVelocityY);
	}
}

SDL_Texture *App::LoadTexture(const char *path)
{
	SDL_Surface *surface = IMG_Load(path);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_DestroySurface(surface);
	if (!texture)
	{
		std::cout << "ERROR loading texture: " << path << std::endl;
	}
	return texture;
}
