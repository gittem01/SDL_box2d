#include <Base.h>
#include <DebugRenderer.h>
#include <chrono>

void createTestBodies(b2World* world)
{
    // box0
    b2BodyDef boxBodyDef;
    boxBodyDef.type = b2_dynamicBody;
    boxBodyDef.position.Set(0.0f, 0.0f);
    b2Body* boxBody = world->CreateBody(&boxBodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(1.0f, 1.0f);
    boxBody->CreateFixture(&boxShape, 1.0f);

    boxBody->SetAngularVelocity(1.0);

    // box1
    boxBodyDef.position.Set(0.6f, 2.5f);
    boxBody = world->CreateBody(&boxBodyDef);

    boxShape.SetAsBox(1.0f, 1.0f);
    boxBody->CreateFixture(&boxShape, 1.0f);

    // second fixture for box1
    boxShape.SetAsBox(0.5f, 0.5f, b2Vec2(0.0f, 0.8f), b2_pi * 0.25f);
    boxBody->CreateFixture(&boxShape, 1.0f);

    // static box
    boxBodyDef.type = b2_staticBody;
    boxBodyDef.position.Set(0.0f, -10.0f);
    boxBody = world->CreateBody(&boxBodyDef);

    boxShape.SetAsBox(100.0f, 1.0f);
    boxBody->CreateFixture(&boxShape, 1.0f);

    // circle
    b2BodyDef circleBodyDef;
    circleBodyDef.type = b2_dynamicBody;
    circleBodyDef.position.Set(5.0f, 0.0f);
    b2Body* circleBody = world->CreateBody(&circleBodyDef);

    b2CircleShape circleShape;
    circleShape.m_radius = 1.0f;
    circleBody->CreateFixture(&circleShape, 1.0f);

    circleBody->SetAngularVelocity(8.0);

    b2MouseJointDef jDef;
    jDef.bodyA = boxBody;
    jDef.bodyB = circleBody;
    jDef.collideConnected = true;

    jDef.target = circleBody->GetPosition() - b2Vec2(1.0, 0.0);
    jDef.maxForce = circleBody->GetMass() * 20.0f;
    
    b2LinearStiffness(jDef.stiffness, jDef.damping, 1.0f, 0.0f, jDef.bodyA, jDef.bodyB);

    b2MouseJoint* mj = (b2MouseJoint*)world->CreateJoint(&jDef);

    b2Vec2 nextTarget = b2Vec2(0.0, -3.0);
    mj->SetTarget(nextTarget);
}

Base::Base()
{
    memset(keyPresses, 0, SDL_NUM_SCANCODES);

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("SDL Debug Renderer",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_DisplayMode displayMode;
    bool res = SDL_GetDisplayMode(0, 0, &displayMode);

    int refreshRate;
    if (displayMode.refresh_rate > 0 && res == 0)
    {
        refreshRate = displayMode.refresh_rate;
    }
    else
    {
		refreshRate = 60;
	}
    deltaTime = 1.0f / (float)refreshRate;

    b2Vec2 gravity = b2Vec2(0.0f, -10.0f);
    world = new b2World(gravity);

    debugRenderer = new DebugRenderer(this);
    debugRenderer->SetFlags(renderFlags);
    world->SetDebugDraw(debugRenderer);

    createTestBodies(world);
}

Base::~Base()
{
    delete world;
    delete debugRenderer;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Base::handleEvents()
{
    for (SDL_Scancode pressedKey : pressedKeys)
    {
        if (keyPresses[pressedKey]) keyPresses[pressedKey] = 1;
    }
    pressedKeys.clear();

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
        case SDL_QUIT:
            shouldQuit = true;
            break;
        case SDL_MOUSEWHEEL:
            debugRenderer->scaleFactor *= 1.0f + (float)e.wheel.y * 0.1f;
            break;
        case SDL_KEYDOWN:
            if (!e.key.repeat)
            {
                keyPresses[e.key.keysym.scancode] = 2;
                pressedKeys.push_back(e.key.keysym.scancode);
            }
            break;
        case SDL_KEYUP:
            keyPresses[e.key.keysym.scancode] = 0;
            break;
        case SDL_WINDOWEVENT:
            switch (e.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                SDL_GetWindowSize(window, &width, &height);
                halfWidth = width / 2; halfHeight = height / 2;
                break;
            }
            break;
        default:
            break;
        }
    }

    if (keyPresses[SDL_SCANCODE_A] || keyPresses[SDL_SCANCODE_LEFT])
        debugRenderer->camPos.x -= (200.0f * deltaTime) / debugRenderer->scaleFactor;
    if (keyPresses[SDL_SCANCODE_D] || keyPresses[SDL_SCANCODE_RIGHT])
        debugRenderer->camPos.x += (200.0f * deltaTime) / debugRenderer->scaleFactor;
    if (keyPresses[SDL_SCANCODE_W] || keyPresses[SDL_SCANCODE_UP])
        debugRenderer->camPos.y += (200.0f * deltaTime) / debugRenderer->scaleFactor;
    if (keyPresses[SDL_SCANCODE_S] || keyPresses[SDL_SCANCODE_DOWN])
        debugRenderer->camPos.y -= (200.0f * deltaTime) / debugRenderer->scaleFactor;

    if (keyPresses[SDL_SCANCODE_Z]) debugRenderer->scaleFactor *= 1.0f + 1.0f * deltaTime;
    if (keyPresses[SDL_SCANCODE_X]) debugRenderer->scaleFactor *= 1.0f - 1.0f * deltaTime;

    debugRenderer->scaleFactor = std::max(std::min(debugRenderer->scaleFactor, debugRenderer->maxScale), debugRenderer->minScale);

    for (int i = 0; i < 5; i++)
    {
        int maskCode = SDL_SCANCODE_1 + i;
        if (keyPresses[maskCode] == 2)
        {
            renderFlags ^= 1 << i;
        }
    }

    debugRenderer->SetFlags(renderFlags);
}

void Base::loop()
{
    while (!shouldQuit)
    {
        handleEvents();

        world->Step(deltaTime, 8, 3);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        world->DebugDraw();

        SDL_RenderPresent(renderer);
    }
}