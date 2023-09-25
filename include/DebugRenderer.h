#include <box2d/box2d.h>
#include <vector>
#include <SDL2/SDL.h>

class DebugRenderer : public b2Draw
{
private:
	const int CIRCLE_EDGES = 64;

	// preallocated vertices
	std::vector<SDL_Vertex> allocatedVertices;
	// precalculated indices to render polygons
	std::vector<std::vector<int>> indices;
public:
	class Base* base;

	b2Vec2 camPos = b2Vec2(0.0f, 0.0f);
	float scaleFactor = 32.0f;

	DebugRenderer(Base* base);

	void precalculateIndices();
	b2Vec2 translateToScreenCoords(b2Vec2 vec) const;

	virtual ~DebugRenderer() {}

	/// Draw a closed polygon provided in CCW order.
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	/// Draw a solid closed polygon provided in CCW order.
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	/// Draw a circle.
	void DrawCircle(const b2Vec2& center, float radius, const b2Color& color);

	/// Draw a solid circle.
	void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color);

	/// Draw a line segment.
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	/// Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	void DrawTransform(const b2Transform& xf);

	/// Draw a point.
	void DrawPoint(const b2Vec2& p, float size, const b2Color& color);
};