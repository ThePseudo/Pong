#include "App.h"
#include "SDL3_image/SDL_image.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "SDL_video.h"
#include <iostream>

void App::Init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	m_window = SDL_CreateWindow("Pong", DEFAULT_WIDTH, DEFAULT_HEIGHT, 0);
	m_renderer = SDL_CreateRenderer(m_window, nullptr, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// Init objects
	m_ball = Ball(m_renderer);
	m_pad1 = Pad(100, static_cast<int>((DEFAULT_HEIGHT - 100) / 2), m_renderer, 255, 0, 0);
	m_pad2 = Pad(DEFAULT_WIDTH - 140, static_cast<int>((DEFAULT_HEIGHT - 100) / 2), m_renderer, 0, 255, 0);
	m_middleBar = MiddleBar(m_renderer);
	m_score = Score(m_renderer);
	// Set textures
	m_ball.SetTexture(LoadTexture("images/ball.png"));
	m_pad1.SetTexture(LoadTexture("images/paddlegreen.png"));
	m_pad2.SetTexture(LoadTexture("images/paddlered.png"));
	m_middleBar.SetTexture(LoadTexture("images/paddlewhite.png"));
	m_score.AddTexture(LoadTexture("images/0.png"));
	m_score.AddTexture(LoadTexture("images/1.png"));
	m_score.AddTexture(LoadTexture("images/2.png"));
	m_score.AddTexture(LoadTexture("images/3.png"));
	m_score.AddTexture(LoadTexture("images/4.png"));
	m_score.AddTexture(LoadTexture("images/5.png"));
	m_score.AddTexture(LoadTexture("images/6.png"));
	m_score.AddTexture(LoadTexture("images/7.png"));
	m_score.AddTexture(LoadTexture("images/8.png"));
	m_score.AddTexture(LoadTexture("images/9.png"));

	m_walls[0] = Wall(m_renderer, SDL_FRect{0, -10, DEFAULT_WIDTH, 10});
	m_walls[1] = Wall(m_renderer, SDL_FRect{0, DEFAULT_HEIGHT, DEFAULT_WIDTH, 10});
	m_triggers.push_back(Trigger());
	m_triggers.push_back(Trigger());

	m_triggers[0].SetActivationFunction([&] {
		m_score.AddScore(1);
		m_ball.Reset();
	});

	m_triggers[1].SetActivationFunction([&] {
		m_score.AddScore(0);
		m_ball.Reset();
	});

	m_triggers[0].SetActivationCondition([&] { return m_ball.GetPosition().x < 0; });

	m_triggers[1].SetActivationCondition(
		[&] { return m_ball.GetPosition().x + m_ball.GetPosition().w > DEFAULT_WIDTH; });

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
			SDL_Delay(toWait);
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
	m_middleBar.Render(SDL_Point{DEFAULT_WIDTH, DEFAULT_HEIGHT});
	m_score.Render();
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
		}
		// If next ball position .y collides with pad
		auto nextBallPosition = ballPosition;
		nextBallPosition.x += deltaTime * m_ball.GetVelocity().x;
		nextBallPosition.y += deltaTime * m_ball.GetVelocity().y;
		SDL_GetRectIntersectionFloat(&position, &nextBallPosition, &collisionRect);
		if (collisionRect.w > 0.001 && collisionRect.y <= position.y && m_ball.GetVelocity().y > 0)
		{
			m_ball.InvertYVelocity();
		}
		if (collisionRect.w > 0.001 && collisionRect.y + collisionRect.h >= position.y + position.h &&
			m_ball.GetVelocity().y < 0)
		{
			m_ball.InvertYVelocity();
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
