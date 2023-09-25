#include <box2d/box2d.h>
#include <SDL2/SDL.h>
#include <vector>
#include <math.h>

class Base
{
public:
	b2World* world;
	class DebugRenderer* debugRenderer;
	SDL_Window* window;
	SDL_Renderer* renderer;
	char keyPresses[SDL_NUM_SCANCODES]{};
	std::vector<SDL_Scancode> pressedKeys;

	float deltaTime = 0.0f;
	uint32 renderFlags = 0x1F; // initially render everything

	int width = 1280;
	int height = 720;
	int halfWidth = width / 2;
	int halfHeight = height / 2;

	bool shouldQuit = false;

	Base();
	~Base();

	void handleEvents();
	void loop();
};