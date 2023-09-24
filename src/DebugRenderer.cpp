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
	indices.resize(b2_maxPolygonVertices - 1);

	for (int i = 2; i < b2_maxPolygonVertices; i++)
	{
		indices[i - 2].resize(3 * (i - 1));

		for (int j = 0; j < i - 1; j++)
		{
			indices[i - 2][3 * j] = 0;
			indices[i - 2][3 * j + 1] = j + 1;
			indices[i - 2][3 * j + 2] = j + 2;
		}
	}

	indices[b2_maxPolygonVertices - 2].resize(3 * (CIRCLE_EDGES - 2));

	for (int j = 0; j < CIRCLE_EDGES - 2; j++)
	{
		indices[b2_maxPolygonVertices - 2][3 * j] = 0;
		indices[b2_maxPolygonVertices - 2][3 * j + 1] = j + 1;
		indices[b2_maxPolygonVertices - 2][3 * j + 2] = j + 2;
	}
}

b2Vec2 DebugRenderer::translateToScreenCoords(b2Vec2 vec)
{
	// translate point to camera position
	vec -= camPos;
	
	// scale point for rendering
	vec *= scaleFactor;

	// SDL coordinates start from top left and y is inverted
	// adding halfWidth and halfHeight to vertices to center the polygon

	vec.x += base->halfWidth; vec.y = base->halfHeight - vec.y;

	return vec;
}

void DebugRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	SDL_SetRenderDrawColor(base->renderer, color.r * 255, color.g * 255, color.b * 255, color.a * 255);

	for (int i = 0; i < vertexCount; i++)
	{
		int j = (i + 1) % vertexCount;

		b2Vec2 v0 = translateToScreenCoords(vertices[i]);
		b2Vec2 v1 = translateToScreenCoords(vertices[j]);

		SDL_RenderDrawLine(base->renderer, v0.x, v0.y, v1.x, v1.y);
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

		allocatedVertices[i].color.r = (Uint8)(color.r * 128);
		allocatedVertices[i].color.g = (Uint8)(color.g * 128);
		allocatedVertices[i].color.b = (Uint8)(color.b * 128);
		allocatedVertices[i].color.a = 128;
	}

	SDL_RenderGeometry(base->renderer, nullptr, allocatedVertices.data(), vertexCount, indices[vertexCount - 3].data(), indices[vertexCount - 3].size());

	// render polygon outline
	SDL_SetRenderDrawColor(base->renderer, color.r * 255, color.g * 255, color.b * 255, color.a * 255);

	// SDL coordinates start from top left and y is inverted
	// adding halfWidth and halfHeight to vertices to center the polygon

	for (int i = 0; i < vertexCount; i++)
	{
		int j = (i + 1) % vertexCount;

		b2Vec2 v0 = translateToScreenCoords(vertices[i]);
		b2Vec2 v1 = translateToScreenCoords(vertices[j]);

		SDL_RenderDrawLine(base->renderer, v0.x, v0.y, v1.x, v1.y);
	}
}

void DebugRenderer::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
	// render polygon outline
	SDL_SetRenderDrawColor(base->renderer, color.r * 255, color.g * 255, color.b * 255, color.a * 255);

	// SDL coordinates start from top left and y is inverted
	// adding halfWidth and halfHeight to vertices to center the polygon

	for (int i = 0; i < CIRCLE_EDGES; i++)
	{
		float angle = 2.0f * b2_pi * i / CIRCLE_EDGES;
		float nextAngle = 2.0f * b2_pi * (i + 1) / CIRCLE_EDGES;

		int j = (i + 1) % CIRCLE_EDGES;

		b2Vec2 v0 = b2Vec2(center.x + radius * cos(angle), center.y + radius * sin(angle));
		b2Vec2 v1 = b2Vec2(center.x + radius * cos(nextAngle), center.y + radius * sin(nextAngle));

		v0 = translateToScreenCoords(v0);
		v1 = translateToScreenCoords(v1);

		SDL_RenderDrawLine(base->renderer, v0.x, v0.y, v1.x, v1.y);
	}
}

void DebugRenderer::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
	// render filled circle
	for (int i = 0; i < CIRCLE_EDGES; i++)
	{
		float angle = 2.0f * b2_pi * i / CIRCLE_EDGES;

		SDL_FPoint& point = allocatedVertices[i].position;

		b2Vec2 translatedPoint = translateToScreenCoords(b2Vec2(center.x + radius * cos(angle), center.y + radius * sin(angle)));

		point.x = translatedPoint.x;
		point.y = translatedPoint.y;

		allocatedVertices[i].color.r = (Uint8)(color.r * 128);
		allocatedVertices[i].color.g = (Uint8)(color.g * 128);
		allocatedVertices[i].color.b = (Uint8)(color.b * 128);
		allocatedVertices[i].color.a = 128;
	}

	SDL_RenderGeometry(base->renderer, nullptr, allocatedVertices.data(), CIRCLE_EDGES, indices[b2_maxPolygonVertices - 2].data(), indices[b2_maxPolygonVertices - 2].size());

	// render polygon outline
	SDL_SetRenderDrawColor(base->renderer, color.r * 255, color.g * 255, color.b * 255, color.a * 255);

	// SDL coordinates start from top left and y is inverted
	// adding halfWidth and halfHeight to vertices to center the polygon

	for (int i = 0; i < CIRCLE_EDGES; i++)
	{
		float angle = 2.0f * b2_pi * i / CIRCLE_EDGES;
		float nextAngle = 2.0f * b2_pi * (i + 1) / CIRCLE_EDGES;

		int j = (i + 1) % CIRCLE_EDGES;

		b2Vec2 v0 = b2Vec2(center.x + radius * cos(angle), center.y + radius * sin(angle));
		b2Vec2 v1 = b2Vec2(center.x + radius * cos(nextAngle), center.y + radius * sin(nextAngle));

		v0 = translateToScreenCoords(v0);
		v1 = translateToScreenCoords(v1);

		SDL_RenderDrawLine(base->renderer, v0.x, v0.y, v1.x, v1.y);
	}

	b2Vec2 c0 = translateToScreenCoords(center);
	b2Vec2 c1 = translateToScreenCoords(center + axis);

	SDL_RenderDrawLine(base->renderer, c0.x, c0.y, c1.x, c1.y);
}

void DebugRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	SDL_SetRenderDrawColor(base->renderer, color.r * 255, color.g * 255, color.b * 255, color.a * 255);

	b2Vec2 newp1 = translateToScreenCoords(p1);
	b2Vec2 newp2 = translateToScreenCoords(p2);

	SDL_RenderDrawLine(base->renderer, newp1.x, newp1.y, newp2.x, newp2.y);
}

void DebugRenderer::DrawTransform(const b2Transform& xf)
{
	const float k_axisScale = 0.4f;

	b2Vec2 p1 = xf.p, p2;

	p2 = p1 + k_axisScale * xf.q.GetXAxis();
	DrawSegment(p1, p2, b2Color(1.0f, 0.0f, 0.0f));

	p2 = p1 + k_axisScale * xf.q.GetYAxis();
	DrawSegment(p1, p2, b2Color(0.0f, 1.0f, 0.0f));
}

void DebugRenderer::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
	SDL_SetRenderDrawColor(base->renderer, color.r * 255, color.g * 255, color.b * 255, color.a * 255);

	SDL_RenderDrawPoint(base->renderer, p.x, p.y);
}