#include "App.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include <iostream>
#include <memory>

void App::Init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	m_window =
		SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WIDTH, DEFAULT_HEIGHT, 0);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	m_ball = std::make_shared<Ball>(m_renderer);
	m_pad1 = std::make_shared<Pad>(100, (DEFAULT_HEIGHT - 100) / 2, m_renderer, 255, 0, 0);
	m_pad2 = std::make_shared<Pad>(DEFAULT_WIDTH - 140, (DEFAULT_HEIGHT - 100) / 2, m_renderer, 0, 255, 0);
	m_walls.push_back(Wall(m_renderer, SDL_FRect{0, 0, DEFAULT_WIDTH, 10}));
	m_walls.push_back(Wall(m_renderer, SDL_FRect{0, DEFAULT_HEIGHT - 10, DEFAULT_WIDTH, 10}));
	m_triggers.push_back(Trigger(SDL_FRect{0, 0, 10, DEFAULT_HEIGHT}));
	m_triggers.push_back(Trigger(SDL_FRect{DEFAULT_WIDTH - 10, 0, 10, DEFAULT_HEIGHT}));

	m_triggers[0].SetActivationFunction([&]() {
		m_pad2->AddScore();
		std::cout << "Score pad 1: " << m_pad1->GetScore() << std::endl;
		std::cout << "Score pad 2: " << m_pad2->GetScore() << std::endl;
		m_ball = std::make_shared<Ball>(m_renderer);
	});

	m_triggers[1].SetActivationFunction([&]() {
		m_pad1->AddScore();
		std::cout << "Score pad 1: " << m_pad1->GetScore() << std::endl;
		std::cout << "Score pad 2: " << m_pad2->GetScore() << std::endl;
		m_ball = std::make_shared<Ball>(m_renderer);
	});

	m_triggers[0].SetActivationCondition([&]() {
		SDL_FRect temp;
		return SDL_IntersectFRect(&m_ball->GetPosition(), &m_triggers[0].GetPosition(), &temp);
	});

	m_triggers[1].SetActivationCondition([&]() {
		SDL_FRect temp;
		return SDL_IntersectFRect(&m_ball->GetPosition(), &m_triggers[1].GetPosition(), &temp);
	});

	m_oldTime = SDL_GetTicks64();
}

void App::Run()
{
	SDL_Event event;
	bool shouldRun = true;
	while (true)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				shouldRun = false;
			}
			if (event.type == SDL_KEYDOWN)
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
			if (event.type == SDL_KEYUP)
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
		m_newtime = SDL_GetTicks64();
		ApplyUserInput();
		ApplyAI();
		Update(m_newtime - m_oldTime);
		CheckAndApplyCollisions();
		Render();
		int64_t toWait = 16 - static_cast<int64_t>(m_newtime - m_oldTime);
		m_oldTime = m_newtime;
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
	m_pad1->Render();
	m_pad2->Render();
	m_ball->Render();
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
	m_pad1->Update(deltaTime);
	m_pad2->Update(deltaTime);
	m_ball->Update(deltaTime);
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

void App::CheckAndApplyPadCollisions(const std::shared_ptr<Pad> &pad)
{
	auto position = pad->GetPosition();
	SDL_FRect collisionRect;
	SDL_FRect ballPosition = m_ball->GetPosition();
	for (const auto &wall : m_walls)
	{
		if (SDL_IntersectFRect(&pad->GetPosition(), &wall.GetPosition(), &collisionRect))
		{
			if (pad->GetPosition().y >= collisionRect.y)
			{
				pad->SetPosition(pad->GetPosition().x, pad->GetPosition().y + collisionRect.h);
			}
			else
			{
				pad->SetPosition(pad->GetPosition().x, pad->GetPosition().y - collisionRect.h);
			}
		}
	}
	if (SDL_IntersectFRect(&position, &ballPosition, &collisionRect) == SDL_TRUE)
	{
		m_ball->InvertXVelocity();
		if (position.x > static_cast<float>(DEFAULT_WIDTH) / 2.0f) // RIGHT SIDE
		{
			m_ball->SetPosition(position.x - ballPosition.w - 0.01f, ballPosition.y);
		}
		else
		{
			m_ball->SetPosition(position.x + position.w + 0.01f, ballPosition.y);
		}
	}
}

void App::CheckAndApplyBallCollisions()
{
	auto position = m_ball->GetPosition();
	SDL_FRect collisionRect;
	for (const auto &wall : m_walls)
	{
		if (SDL_IntersectFRect(&position, &wall.GetPosition(), &collisionRect))
		{
			m_ball->InvertYVelocity();
			if (position.y >= collisionRect.y)
			{
				m_ball->SetPosition(position.x, position.y + collisionRect.h);
			}
			else
			{
				m_ball->SetPosition(position.x, position.y - collisionRect.h);
			}
		}
	}
}

void App::ApplyAI()
{
	auto position = m_pad2->GetPosition();
	auto ballPos = m_ball->GetPosition();
	if ((ballPos.y + ballPos.h / 2.0f) < position.y)
	{
		m_pad2->SetYVelocity(-m_settings.padVelocityY);
	}
	else if ((ballPos.y + ballPos.h / 2.0f) > position.y + position.h)
	{
		m_pad2->SetYVelocity(m_settings.padVelocityY);
	}
}

void App::ApplyUserInput()
{
	if (m_downPressed)
	{
		m_pad1->SetYVelocity(m_settings.padVelocityY);
	}
	if (m_upPressed)
	{
		m_pad1->SetYVelocity(-m_settings.padVelocityY);
	}
}
