#include <Base.h>
#include <DebugRenderer.h>

Base::Base()
{
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

    b2Vec2 gravity = b2Vec2(0.0f, -1.0f);
    world = new b2World(gravity);

    debugRenderer = new DebugRenderer(this);
    debugRenderer->SetFlags(b2Draw::e_shapeBit | b2Draw::e_centerOfMassBit);
    world->SetDebugDraw(debugRenderer);

    b2BodyDef boxBodyDef;
    boxBodyDef.type = b2_dynamicBody;
    boxBodyDef.position.Set(0.0f, 0.0f);
    b2Body* boxBody = world->CreateBody(&boxBodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(1.0f, 1.0f);
    boxBody->CreateFixture(&boxShape, 1.0f);

    boxBody->SetAngularVelocity(1.0);

    boxBodyDef.type = b2_staticBody;
    boxBodyDef.position.Set(0.0f, -10.0f);
    boxBody = world->CreateBody(&boxBodyDef);

    boxShape.SetAsBox(100.0f, 1.0f);
    boxBody->CreateFixture(&boxShape, 1.0f);

    b2BodyDef circleBodyDef;
    circleBodyDef.type = b2_dynamicBody;
    circleBodyDef.position.Set(5.0f, 0.0f);
    b2Body* circleBody = world->CreateBody(&circleBodyDef);

    b2CircleShape circleShape;
    circleShape.m_radius = 1.0f;
    circleBody->CreateFixture(&circleShape, 1.0f);

    circleBody->SetAngularVelocity(-1.0);
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
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
        case SDL_QUIT:
            shouldQuit = true;
            break;
        case SDL_MOUSEWHEEL:
            debugRenderer->scaleFactor += e.wheel.y;
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
        if (e.type == SDL_QUIT)
        {
            shouldQuit = true;
        }
        else if (e.type == SDL_MOUSEWHEEL)
        {
            debugRenderer->scaleFactor += e.wheel.y;
        }
    }
}

void Base::loop()
{
    while (!shouldQuit)
    {
        handleEvents();

        world->Step(1.0f / 60.0f, 8, 3);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        world->DebugDraw();

        SDL_RenderPresent(renderer);
    }
}