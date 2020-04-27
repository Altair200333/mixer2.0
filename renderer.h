#pragma once
#include <algorithm>
#include <SDL.h>
#include <vector>


#include "mesh.h"
#include "Vector2.h"

class Color
{
public:
	unsigned char r, g, b;

	Color(const unsigned char r_c, const unsigned char g_c, const unsigned char b_c) : r(r_c), g(g_c), b(b_c)
	{
	}
	Color(): r(20), g(20), b(20)
	{}
	friend Color operator+(const Color& c1, const Color& c2)
	{
		return { unsigned char((c1.r + c2.r)/2),unsigned char((c1.g + c2.g)/2),unsigned char((c1.b + c2.b)/2) };
	}
};

//Renderer is responsible for drawing lines, points and filling polygons
class Renderer
{
	int halfW;
	int halfH;
public:
	int width;
	int height;

	Renderer(int w, int h)
	{
		width = w;
		height = h;
		halfH = height / 2;
		halfW = width / 2;
	}

	void drawPoint(SDL_Renderer* render, int x, int y)
	{
		SDL_RenderDrawPoint(render, halfW + x, halfH - y);
	}

	void drawPoint2(SDL_Renderer* render, Vector2 point)
	{
		SDL_RenderDrawPoint(render, width / 2 + int(point.x) + 200, height / 2 - int(point.y));
	}

	void drawLine(SDL_Renderer* render, Vector2 point1, Vector2 point2)
	{
		SDL_RenderDrawLine(render, halfW + int(round(point1.x)), halfH - int(round(point1.y)),
		                   halfW + int(round(point2.x)), halfH - int(round(point2.y)));
	}

	void drawTriangleWire(SDL_Renderer* render, Polygon2D polygon)
	{
		if (!isVisible(polygon))
			return;
		drawLine(render, polygon.vertices[0], polygon.vertices[1]);
		drawLine(render, polygon.vertices[1], polygon.vertices[2]);
		drawLine(render, polygon.vertices[2], polygon.vertices[0]);
	}
	
	void drawTriangleSolid(SDL_Renderer* render, Polygon2D polygon, Color c)
	{
		if(!isVisible(polygon))
			return;
		std::vector<Vector2> p;
		p.push_back(polygon.vertices[0]);
		p.push_back(polygon.vertices[1]);
		p.push_back(polygon.vertices[2]);

		const int min = std::min({ polygon.vertices[0].y, polygon.vertices[1].y, polygon.vertices[2].y });
		const int max = std::max({ polygon.vertices[0].y, polygon.vertices[1].y, polygon.vertices[2].y });
		FillShapeScanLine(render, p, min, max, c);
	}
protected:
	bool isVisible(Polygon2D& p)
	{
		return inRange(-halfW, halfW, p.vertices[0].x) && inRange(-halfW, halfW, p.vertices[1].x)
			&& inRange(-halfW, halfW, p.vertices[2].x) &&
			inRange(-halfH, halfH, p.vertices[0].y) && inRange(-halfH, halfH, p.vertices[1].y)
			&& inRange(-halfH, halfH, p.vertices[2].y);
	}
	bool inRange(int low, int high, int x)
	{
		return ((x - high) * (x - low) <= 0);
	}
	void findXIntersections(const std::vector<Vector2>& points, int pixelY, std::vector<int>& nodeX)
	{
		//  Build a list of nodes.
		int j = points.size() - 1;
		for (int i = 0; i < points.size(); ++i)
		{
			if (points[i].y < float(pixelY) && points[j].y >= float(pixelY)
				|| points[j].y < float(pixelY) && points[i].y >= float(pixelY))
			{
				nodeX.push_back(int(points[i].x + (float(pixelY) - points[i].y) / (points[j].y - points[i].y)
					* (points[j].x - points[i].x)));
			}
			j = i;
		}

		std::sort(nodeX.begin(), nodeX.end());
	}

	void drawLineAtY(SDL_Renderer* render, int pixelY, const std::vector<Vector2>& points, const Color& color)
	{
		std::vector<int> xIntersections;
		findXIntersections(points, pixelY, xIntersections);

		//  Fill the pixels between node pairs.
		for (int i = 0; i < xIntersections.size(); i += 2)
		{
			SDL_SetRenderDrawColor(render, color.r, color.g, color.b, 255);
			drawLine(render, {xIntersections[i], pixelY}, {xIntersections[i + 1], pixelY});
		}
	}

	void FillShapeScanLine(SDL_Renderer* render, const std::vector<Vector2>& points, int minY, int maxY,
	                       const Color& color)
	{
		for (int pixelY = minY; pixelY <= maxY; ++pixelY)
		{
			drawLineAtY(render, pixelY, points, color);
		}
	}
};
