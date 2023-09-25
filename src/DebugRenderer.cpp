#include <DebugRenderer.h>
#include <Base.h>

DebugRenderer::DebugRenderer(Base* base)
{
	this->base = base;
	
	allocatedVertices.resize(std::max(b2_maxPolygonVertices, CIRCLE_EDGES));

	precalculateIndices();
}

void DebugRenderer::precalculateIndices()
{
	// indices 0, 1 and 2 are not used
	indices.resize(b2_maxPolygonVertices + 2);

	for (size_t i = 3; i < b2_maxPolygonVertices; i++)
	{
		indices[i].resize(3 * i);

		for (size_t j = 0; j < i - 2; j++)
		{
			indices[i][3 * j] = 0;
			indices[i][3 * j + 1] = (int)j + 1;
			indices[i][3 * j + 2] = (int)j + 2;
		}
	}

	indices[b2_maxPolygonVertices + 1].resize(3 * (CIRCLE_EDGES - 2));

	for (size_t j = 0; j < (size_t)CIRCLE_EDGES - 2; j++)
	{
		indices[b2_maxPolygonVertices + 1][3 * j] = 0;
		indices[b2_maxPolygonVertices + 1][3 * j + 1] = (int)j + 1;
		indices[b2_maxPolygonVertices + 1][3 * j + 2] = (int)j + 2;
	}
}

b2Vec2 DebugRenderer::translateToScreenCoords(b2Vec2 vec) const
{
	// translate point to camera position
	vec -= camPos;
	
	// scale point for rendering
	vec *= scaleFactor;

	// SDL coordinates start from top left and y is inverted
	// adding halfWidth and halfHeight to vertices to center the polygon

	vec.x += (float)base->halfWidth; vec.y = (float)base->halfHeight - vec.y;

	return vec;
}

void DebugRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    SDL_SetRenderDrawColor(base->renderer, (uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255), (uint8_t)(color.a * 255));

	for (int i = 0; i < vertexCount; i++)
	{
		int j = (i + 1) % vertexCount;

		b2Vec2 v0 = translateToScreenCoords(vertices[i]);
		b2Vec2 v1 = translateToScreenCoords(vertices[j]);

		SDL_RenderDrawLine(base->renderer, (int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y);
	}	
}

void DebugRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	// render filled polygon
	for (int i = 0; i < vertexCount; i++)
	{
		SDL_FPoint& point = allocatedVertices[i].position;

		b2Vec2 translatedPoint = translateToScreenCoords(vertices[i]);

		point.x = translatedPoint.x;
		point.y = translatedPoint.y;

		allocatedVertices[i].color.r = (uint8_t)(color.r * 128);
		allocatedVertices[i].color.g = (uint8_t)(color.g * 128);
		allocatedVertices[i].color.b = (uint8_t)(color.b * 128);
		allocatedVertices[i].color.a = 128;
	}

	SDL_RenderGeometry(base->renderer, nullptr, allocatedVertices.data(), vertexCount,
		indices[vertexCount].data(), (int)indices[vertexCount].size());

	// render polygon outline
	SDL_SetRenderDrawColor(base->renderer, (uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255), (uint8_t)(color.a * 255));

	// SDL coordinates start from top left and y is inverted
	// adding halfWidth and halfHeight to vertices to center the polygon

	for (size_t i = 0; i < vertexCount; i++)
	{
		size_t j = (i + 1) % vertexCount;

		b2Vec2 v0 = translateToScreenCoords(vertices[i]);
		b2Vec2 v1 = translateToScreenCoords(vertices[j]);

		SDL_RenderDrawLine(base->renderer, (int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y);
	}
}

void DebugRenderer::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
	// render polygon outline
	SDL_SetRenderDrawColor(base->renderer, (uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255), (uint8_t)(color.a * 255));

	// SDL coordinates start from top left and y is inverted
	// adding halfWidth and halfHeight to vertices to center the polygon

	for (int i = 0; i < CIRCLE_EDGES; i++)
	{
		float angle = 2.0f * b2_pi * (float)i / (float)CIRCLE_EDGES;
		float nextAngle = 2.0f * b2_pi * (float)(i + 1) / (float)CIRCLE_EDGES;

		b2Vec2 v0 = b2Vec2(center.x + radius * cosf(angle), center.y + radius * sinf(angle));
		b2Vec2 v1 = b2Vec2(center.x + radius * cosf(nextAngle), center.y + radius * sinf(nextAngle));

		v0 = translateToScreenCoords(v0);
		v1 = translateToScreenCoords(v1);

		SDL_RenderDrawLine(base->renderer, (int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y);
	}
}

void DebugRenderer::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
	// render filled circle
	for (int i = 0; i < CIRCLE_EDGES; i++)
	{
		float angle = 2.0f * b2_pi * (float)i / (float)CIRCLE_EDGES;

		SDL_FPoint& point = allocatedVertices[i].position;

		b2Vec2 translatedPoint = translateToScreenCoords(b2Vec2(center.x + radius * cosf(angle), center.y + radius * sinf(angle)));

		point.x = translatedPoint.x;
		point.y = translatedPoint.y;

		allocatedVertices[i].color.r = (uint8_t)(color.r * 128);
		allocatedVertices[i].color.g = (uint8_t)(color.g * 128);
		allocatedVertices[i].color.b = (uint8_t)(color.b * 128);
		allocatedVertices[i].color.a = 128;
	}

	SDL_RenderGeometry(base->renderer, nullptr, allocatedVertices.data(), CIRCLE_EDGES, indices[b2_maxPolygonVertices + 1].data(), (int)indices[b2_maxPolygonVertices + 1].size());

	// render polygon outline
	SDL_SetRenderDrawColor(base->renderer, (uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255), (uint8_t)(color.a * 255));

	// SDL coordinates start from top left and y is inverted
	// adding halfWidth and halfHeight to vertices to center the polygon

	for (int i = 0; i < CIRCLE_EDGES; i++)
	{
		float angle = 2.0f * b2_pi * (float)i / (float)CIRCLE_EDGES;
		float nextAngle = 2.0f * b2_pi * (float)(i + 1) / (float)CIRCLE_EDGES;

		b2Vec2 v0 = b2Vec2(center.x + radius * cosf(angle), center.y + radius * sinf(angle));
		b2Vec2 v1 = b2Vec2(center.x + radius * cosf(nextAngle), center.y + radius * sinf(nextAngle));

		v0 = translateToScreenCoords(v0);
		v1 = translateToScreenCoords(v1);

		SDL_RenderDrawLine(base->renderer, (int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y);
	}

	b2Vec2 c0 = translateToScreenCoords(center);
	b2Vec2 c1 = translateToScreenCoords(center + axis);

	SDL_RenderDrawLine(base->renderer, (int)c0.x, (int)c0.y, (int)c1.x, (int)c1.y);
}

void DebugRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	SDL_SetRenderDrawColor(base->renderer, (uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255), (uint8_t)(color.a * 255));

	b2Vec2 newp1 = translateToScreenCoords(p1);
	b2Vec2 newp2 = translateToScreenCoords(p2);

	SDL_RenderDrawLine(base->renderer, (int)newp1.x, (int)newp1.y, (int)newp2.x, (int)newp2.y);
}

void DebugRenderer::DrawTransform(const b2Transform& xf)
{
	const float k_axisScale = 0.4f;

	b2Vec2 p1 = xf.p;

	b2Vec2 p2 = p1 + k_axisScale * xf.q.GetXAxis();
	DrawSegment(p1, p2, b2Color(1.0f, 0.0f, 0.0f));

	p2 = p1 + k_axisScale * xf.q.GetYAxis();
	DrawSegment(p1, p2, b2Color(0.0f, 1.0f, 0.0f));
}

void DebugRenderer::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
	b2Vec2 translatedPos = translateToScreenCoords(p);

	SDL_SetRenderDrawColor(base->renderer, (uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255), (uint8_t)(color.a * 255));

	SDL_Rect rect = { (int)(translatedPos.x - size * 0.5f), (int)(translatedPos.y - size * 0.5f), (int)size, (int)size };
	SDL_RenderFillRect(base->renderer, &rect);
}