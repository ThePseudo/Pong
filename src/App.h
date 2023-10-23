#pragma once

#include "Ball.h"
#include "Pad.h"
#include "SDL_render.h"
#include "Trigger.h"
#include "Wall.h"
#include <SDL.h>
#include <memory>
#include <vector>

struct Settings
{
	const float padVelocityY = 150.0f;
};

class App
{
public:
	void Init();

	void Run();

	void Destroy();

private:
	constexpr static int DEFAULT_WIDTH = 1280;
	constexpr static int DEFAULT_HEIGHT = 720;

	SDL_Texture *LoadTexture(const char *path);

	void Render();

	void Update(uint64_t deltaTime);

	void CheckAndApplyCollisions();

	void CheckAndApplyBallCollisions();

	void CheckAndApplyPadCollisions(const std::shared_ptr<Pad> &pad);

	void ApplyAI();

	void ApplyUserInput();

	SDL_Window *m_window;
	SDL_Renderer *m_renderer;

	std::shared_ptr<Ball> m_ball;
	std::shared_ptr<Pad> m_pad1;
	std::shared_ptr<Pad> m_pad2;
	std::vector<Wall> m_walls;
	std::vector<Trigger> m_triggers;

	uint64_t m_oldTime;
	uint64_t m_newtime;

	Settings m_settings;

	bool m_downPressed = false;
	bool m_upPressed = false;
};
