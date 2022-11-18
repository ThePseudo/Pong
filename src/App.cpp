#include "App.h"

void App::Init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	m_window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WIDTH, DEFAULT_HEIGHT,
			0);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	m_ball = std::make_shared<Ball>(m_renderer);
	m_pad1 = std::make_shared<Pad>(100, (DEFAULT_HEIGHT - 100) / 2, m_renderer, 255, 0, 0);
	m_pad2 = std::make_shared<Pad>(DEFAULT_WIDTH - 140, (DEFAULT_HEIGHT - 100) / 2, m_renderer, 0, 255, 0);
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
			if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == SDLK_DOWN)
				{
					m_downPressed = true;
				}
				else if(event.key.keysym.sym == SDLK_UP)
				{
					m_upPressed = true;
				}
			}
			if(event.type == SDL_KEYUP)
			{
				if(event.key.keysym.sym == SDLK_DOWN)
				{
					m_downPressed = false;
				}
				else if(event.key.keysym.sym == SDLK_UP)
				{
					m_upPressed = false;
				}
			}
		}
		if (!shouldRun) break;
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
	m_pad1->Render();
	m_pad2->Render();
	m_ball->Render();
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
}

void App::CheckAndApplyPadCollisions(const std::shared_ptr<Pad>& pad)
{
	auto position = pad->GetPosition();
	if (position.y < 0)
	{
		pad->SetPosition(position.x, 0);
	}
	else if (position.y + position.h > DEFAULT_HEIGHT)
	{
		pad->SetPosition(position.x, DEFAULT_HEIGHT - position.h);
	}
	SDL_FRect collisionRect;
	SDL_FRect ballPosition = m_ball->GetPosition();
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
	if (position.x <= 0)
	{
		m_pad1->AddScore();
		m_ball = std::make_shared<Ball>(m_renderer);
	}
	else if (position.x + position.w >= DEFAULT_WIDTH)
	{
		m_pad2->AddScore();
		m_ball = std::make_shared<Ball>(m_renderer);
	}
	else if (position.y <= 0)
	{
		m_ball->InvertYVelocity();
		m_ball->SetPosition(position.x, 0);
	}
	else if (position.y + position.h >= DEFAULT_HEIGHT)
	{
		m_ball->InvertYVelocity();
		m_ball->SetPosition(position.x, DEFAULT_HEIGHT - position.h);
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
	if(m_downPressed)
	{
		m_pad1->SetYVelocity(m_settings.padVelocityY);
	}
	if(m_upPressed)
	{
		m_pad1->SetYVelocity(-m_settings.padVelocityY);
	}
}
