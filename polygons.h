#pragma once
#include "Vector2.h"
#include "vector3.h"

class Polygon
{
public:
	Vector3 vertices[3];
	Vector3 normal;
	Vector3 center;
	Polygon() : vertices{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, normal{1, 0, 0}
	{
	}
	Polygon(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 n)
	{
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] = v3;

		center = (v1 + v2 + v3) * (1.0f/3);
		normal = n;
	}
};

class Polygon2D
{
public:
	Vector2 vertices[3];
};
