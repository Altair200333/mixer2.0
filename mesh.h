#pragma once
#include "polygons.h"
#include "Vector2.h"
#include "vector3.h"
#include <fstream>
#include <utility>
#include <vector>

#include "renderer.h"
struct Material
{
public:
	Color color;
	bool reflective;
	Material(Color _color, bool _reflective):color(_color), reflective(_reflective)
	{}
	Material()
	{
		color = { 200,200,200 };
		reflective = true;
	}
};
class Mesh
{
public:
	std::vector<Polygon> polygons;
	int polyCount;
	Material material;
	Mesh(Vector3* vertices, int polygonsCount)
	{
		polyCount = polygonsCount;
		
		for (int i = 0; i < polyCount; i++)
		{
			polygons.push_back({ vertices[i * 3], vertices[i * 3 + 1] ,vertices[i * 3 + 2] ,
				(vertices[i * 3 + 1] - vertices[i * 3]) ^ (vertices[i * 3 + 2] - vertices[i * 3]) });
		}
	}

	Mesh(): polygons(), polyCount(0)
	{
	}

	void STLImport(const std::string& fileName)
	{
		int nVertex = 0;
		int nFacet = 0;

		std::ifstream ifs(fileName, std::ifstream::binary);

		std::filebuf* pbuf = ifs.rdbuf();

		auto size = pbuf->pubseekoff(0, ifs.end);

		pbuf->pubseekpos(0);

		char* buffer = new char[(size_t)size];

		pbuf->sgetn(buffer, size);

		char* bufptr = buffer;

		bufptr += 80; // Skip past the header.
		bufptr += 4; // Skip past the number of triangles.

		Vector3 normal;
		Vector3 v1, v2, v3;

		while (bufptr < buffer + size)
		{
			normal = fromBuf(bufptr);
			bufptr += 12;

			v1 = fromBuf(bufptr);
			bufptr += 12;

			v2 = fromBuf(bufptr);
			bufptr += 12;

			v3 = fromBuf(bufptr);
			bufptr += 12;

			const float eps = (float)1.0e-9;

			// If the normal in the STL file is blank, then create a proper normal.
			if (abs(normal.x) < eps && abs(normal.y) < eps && abs(normal.z) < eps)
			{
				Vector3 u, v;
				u = v2 - v1;
				v = v3 - v1;
				normal = u ^ v;
				normal.normalize();
			}

			nFacet++;
			nVertex += 3;

			STLAddFacet(v1, v2, v3, normal);

			bufptr += 2;
		}

		ifs.close();

		delete[] buffer;
	}
protected:
	Vector3 fromBuf(char* bufptr)
	{
		return { *(float*)(bufptr),  *(float*)(bufptr + 4),*(float*)(bufptr + 8) };
	}
	void STLAddFacet(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 normal)
	{
		polygons.push_back({v1 * 20, v2 * 20, v3 * 20, normal});
		polyCount++;
	}
};
