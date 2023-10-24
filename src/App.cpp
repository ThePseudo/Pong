#include "App.h"
#include "SDL3_image/SDL_image.h"
#include "SDL_keycode.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "SDL_video.h"
#include <cmath>
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
				switch (event.key.keysym.sym)
				{
				case SDLK_DOWN:
					m_downPressed = true;
					break;
				case SDLK_UP:
					m_upPressed = true;
					break;
				case SDLK_w:
					m_LupPressed = true;
					break;
				case SDLK_s:
					m_LdownPressed = true;
					break;
				}
			}
			if (event.type == SDL_EVENT_KEY_UP)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_DOWN:
					m_downPressed = false;
					break;
				case SDLK_UP:
					m_upPressed = false;
					break;
				case SDLK_w:
					m_LupPressed = false;
					break;
				case SDLK_s:
					m_LdownPressed = false;
					break;
				}
			}
		}
		if (!shouldRun)
			break;
		m_newTime = SDL_GetTicks();
		ApplyUserInput();
		if (!m_multiplayer)
		{
			ApplyAI();
		}
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
}

void App::CheckAndApplyPadCollisions(Pad &pad)
{
	constexpr static float SQRT2 = 1.41421356237;
	constexpr double deltaTime = 0.01;
	auto position = pad.GetPosition();
	SDL_FRect collisionRect;
	SDL_FRect ballPosition = m_ball.GetPosition();
	if (pad.GetPosition().y < 0)
	{
		pad.SetPosition(pad.GetPosition().x, 0);
	}
	if (pad.GetPosition().y + pad.GetPosition().h > DEFAULT_HEIGHT)
	{
		pad.SetPosition(pad.GetPosition().x, DEFAULT_HEIGHT - pad.GetPosition().h);
	}
	// If ball intersects pad
	if (SDL_GetRectIntersectionFloat(&position, &ballPosition, &collisionRect) == SDL_TRUE)
	{
		auto velocity = m_ball.GetBaseVelocity();
		auto center = SDL_FPoint{position.x + position.w / 2, position.y + position.h / 2};
		auto centerBall = SDL_FPoint{ballPosition.x + ballPosition.w / 2, ballPosition.y + ballPosition.h / 2};
		auto angle = atan((centerBall.y - center.y) / (centerBall.x - center.x));
		angle = fmin(angle, SDL_PI_F / 2.0f - 0.01);
		angle = fmax(angle, -SDL_PI_F / 2.0f + 0.01);
		m_ball.SetVelocity(SDL_FPoint{cos(angle) * velocity.x * 2 / SQRT2, sin(angle) * velocity.y * 2 / SQRT2});
		if (position.x > DEFAULT_WIDTH / 2.0f)
		{
			m_ball.InvertXVelocity();
			m_ball.InvertYVelocity();
		}
		if (position.y > ballPosition.y && m_ball.GetVelocity().y > 0)
		{
			m_ball.InvertYVelocity();
		}
		if (position.y + position.h < ballPosition.y + ballPosition.h && m_ball.GetVelocity().y < 0)
		{
			m_ball.InvertYVelocity();
		}
	}
}

void App::CheckAndApplyBallCollisions()
{
	auto position = m_ball.GetPosition();
	if (position.x <= 0)
	{
		m_score.AddScore(0);
		m_ball.Reset();
	}
	if (position.x + position.w >= DEFAULT_WIDTH)
	{
		m_score.AddScore(1);
		m_ball.Reset();
	}
	if (position.y < 0)
	{
		m_ball.SetPosition(position.x, 0);
		if (m_ball.GetVelocity().y < 0)
		{
			m_ball.InvertYVelocity();
		}
	}
	if (position.y + position.h > DEFAULT_HEIGHT)
	{
		m_ball.SetPosition(position.x, DEFAULT_HEIGHT - position.h);
		if (m_ball.GetBaseVelocity().y > 0)
		{
			m_ball.InvertYVelocity();
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
	if (m_multiplayer)
	{
		if (m_downPressed)
		{
			m_pad2.SetYVelocity(m_settings.padVelocityY);
		}
		if (m_upPressed)
		{
			m_pad2.SetYVelocity(-m_settings.padVelocityY);
		}
	}

	if (m_LdownPressed)
	{
		m_pad1.SetYVelocity(m_settings.padVelocityY);
	}
	if (m_LupPressed)
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
