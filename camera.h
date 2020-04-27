#pragma once
#include "Controls.h"
#include "libbitmap.h"
#include "mesh.h"
#include "renderer.h"
#include "Vector2.h"
#include "vector3.h"

class Mesh;



class Camera
{
	int boundX[2] = {};
	int boundY[2] = {};
public:
	Vector3 up = {0, 1, 0};
	Vector3 right = {1, 0, 0};
	Vector3 front = {0, 0, 1};
	//Camera position
	Vector3 camOrigin = {0, 0, -100};

	//------

	double FocalLength = 250;
	//Flag on perspective/orthographic modes
	bool isPersp = false;

	//Mode determines how we render picture
	int MODE = 1;

	//yaw is stored in order to make correct up-down rotation 
	double yaw = 0;
	double rotSpeed = 0.3f;
	int CAMERA_ZOOM_SPEED = 3;


	Vector3 intersectPoint(Vector3 rayVector, Vector3 rayPoint, Vector3 planeNormal, Vector3 planePoint) const
	{
		Vector3 diff = rayPoint - planePoint;
		double prod1 = diff * planeNormal;
		double prod2 = rayVector * planeNormal;
		double ratio = prod1 / prod2;

		return rayPoint - rayVector * ratio;
	}


	Polygon2D project(Polygon p, bool& v) const
	{
		Vector2 p1 = project(p.vertices[0], v);
		Vector2 p2 = project(p.vertices[1], v);
		Vector2 p3 = project(p.vertices[2], v);

		return {p1, p2, p3};
	}

	Vector2 project(Vector3 p, bool& v) const
	{
		Vector2 result;
		if (isPersp)
		{
			result = perspectiveProjection(p, v);
		}
		else
		{
			result = orthographicProjection(p, v);
		}
		return result;
	}

	void render(Renderer* renderer, SDL_Renderer* render, std::vector<Polygon>& polygons)
	{
		std::sort(polygons.begin(), polygons.end(),
		          [this](const Polygon& p1, const Polygon& p2)
		          {
			          return ((camOrigin - p1.center).magnitudeSq() - (camOrigin - p2.center).magnitudeSq()) > 0;
		          });

		float min = (camOrigin - polygons[polygons.size() - 1].center).magnitudeSq();
		float max = (camOrigin - polygons[0].center).magnitudeSq();

		
		for (int i = 0; i < polygons.size(); i++)
		{
			bool visible = true;
			Polygon2D p1 = project(polygons[i], visible);
			if(i==0)
			{
				boundX[0] = std::min({p1.vertices[0].x,p1.vertices[1].x ,p1.vertices[2].x });
				boundX[1] = std::max({p1.vertices[0].x,p1.vertices[1].x ,p1.vertices[2].x });
				boundY[0] = std::min({p1.vertices[0].y,p1.vertices[1].y ,p1.vertices[2].y });
				boundY[1] = std::max({p1.vertices[0].y,p1.vertices[1].y ,p1.vertices[2].y });
			}
			else
			{
				boundX[0] = std::min({ float(boundX[0]), p1.vertices[0].x,p1.vertices[1].x ,p1.vertices[2].x });
				boundX[1] = std::max({ float(boundX[1]), p1.vertices[0].x,p1.vertices[1].x ,p1.vertices[2].x });
				boundY[0] = std::min({ float(boundY[0]), p1.vertices[0].y,p1.vertices[1].y ,p1.vertices[2].y });
				boundY[1] = std::max({ float(boundY[1]),p1.vertices[0].y,p1.vertices[1].y ,p1.vertices[2].y });
			}
			if (visible)
			{
				if (MODE == 1)
				{
					SDL_SetRenderDrawColor(render, 0, 0, 255, SDL_ALPHA_OPAQUE);
					renderer->drawTriangleWire(render, p1);
					
				}
				else
				{
					float dist = ((camOrigin - polygons[i].center).magnitudeSq() - min) / (max - min);
					float factor = 1 - dist;
					float f2 = abs(polygons[i].normal * front);
					renderer->drawTriangleSolid(render, p1, {
						                            unsigned char(10 * factor + 150 * f2),
						                            unsigned char(10 * factor + 150 * f2),
						                            unsigned char(10 * factor + 150 * f2)
					                            });
				}
			}
		}
	}

	void render(Renderer* renderer, SDL_Renderer* render, Mesh* mesh)
	{
		for (int i = 0; i < mesh->polyCount; i++)
		{
			bool visible = true;
			Polygon2D p1 = project(mesh->polygons[i], visible);
			if (visible)
			{
				if (MODE == 1)
				{
					renderer->drawTriangleWire(render, p1);
					
				}
				else
				{
					renderer->drawTriangleSolid(render, p1, {100, 100, 100});
				}
			}
		}
	}

	bool pointInPolygon(Vector3& v, Polygon& p)
	{
		//bool t = (v - p.vertices[0]).magnitudeSq() < 10|| (v - p.vertices[1]).magnitudeSq() < 10|| (v - p.vertices[2]).magnitudeSq() < 10;
		float area = ((p.vertices[0] - p.vertices[1]) ^ (p.vertices[2] - p.vertices[1])).magnitude() * 0.5f;
		float a = ((v - p.vertices[1]) ^ (v - p.vertices[2])).magnitude() / (2 * area);
		float b = ((v - p.vertices[2]) ^ (v - p.vertices[0])).magnitude() / (2 * area);
		float c = ((v - p.vertices[1]) ^ (v - p.vertices[0])).magnitude() / (2 * area);

		float tresh = 1.0f + 0.0001f;
		return a <= tresh && a >= 0 && b <= tresh && b >= 0 && c <= tresh && c >= 0 && (a + b + c) <= tresh;
	}

	void render(Bitmap& img, std::vector<Mesh*>& meshes, int width, int height, Renderer* renderer = nullptr,
	            SDL_Renderer* render = nullptr,
	            Vector3 lp = {70, 50, 70})
	{
		Vector3 ray;
		Vector3 focalPlane = camOrigin + FocalLength * front;
		float sampling = 1.0;
		float lowX = boundX[0] + width / 2;
		float highX = boundX[1] + width / 2;
		float lowY = boundY[0] + height / 2;
		float highY = boundY[1] + height / 2;
		for (float i = lowX>=0?lowX:0; i < (highX<=width?highX:width); i += sampling)
		{
			for (float j = lowY >= 0 ? lowY : 0; j < (highY <= height ? highY : height); j += sampling)
			{
				ray = front * FocalLength + right * (i - width / 2) + up * (j - height / 2);
				Color c = castRay(meshes, ray, camOrigin, lp, 1);
				
				int id = img.getPixelId(i, height - j - 1, 0);

				img.m_buffer[id + 2] = c.r;
				img.m_buffer[id + 1] = c.g;
				img.m_buffer[id] =	   c.b;				
			}
		}
	}
	Vector3 reflect(const Vector3& I, const Vector3& N)
	{
		return I - N * (I * N) * 2.0f;
	}
	Color castRay(std::vector<Mesh*>& meshes, Vector3 ray, Vector3 src, Vector3 lp = { 70, 50, 70 }, int reflects = 0)
	{
		Color c = {10,10,10};
		Hit hit = getHit(meshes, ray, src);
		if (hit.hit)
		{
			if(reflects>0 && hit.material.reflective)
			{
				//std::cout << reflects<<"cast"<<hit.normal<<"\n";
				Vector3 r = reflect(ray.normalize(), hit.normal);
				Color c = castRay(meshes, r, hit.pos+hit.normal*0.1f, lp, --reflects);
				c = c + hit.material.color;
				return c;
			}
			else
			{
				float dist = (hit.pos - lp).magnitudeSq() * 0.0007f;
				Vector3 rr = lp - hit.pos;

				Hit lhits = getHit(meshes, rr, hit.pos + rr * 0.001f);
				if (!lhits.hit)
				{
					float slope = abs(hit.normal * (hit.pos - lp).normalize());

					c.r = std::clamp<int>(hit.material.color.r * slope, 10, 255);
					c.g = std::clamp<int>(hit.material.color.g * slope, 10, 255);
					c.b = std::clamp<int>(hit.material.color.b * slope, 10, 255);
				}
				else
				{
					c = { 10,10,10 };
				}
			}
		}
		return c;
	}
	struct Hit
	{
		Vector3 pos;
		Vector3 normal;
		Material material;
		bool hit;
		Hit(Vector3 _pos, Vector3 _norm, Material _material, bool _hit) :pos(_pos), normal(_norm), material(_material), hit(_hit)
		{}
	};
	Hit getHit(std::vector<Mesh*>& meshes, Vector3 ray, Vector3 src)
	{
		Hit closestHit = { {1000,10000,1000},{0,0,1}, {{100,100,100}, false }, false};
		for (auto m : meshes)
		{
			for (auto p : m->polygons)
			{	
				Vector3 hit = intersectPoint(ray, src, p.normal, p.center);
				if (pointInPolygon(hit, p) && ray * (hit - src) > 0 && ((src - hit).magnitudeSq() < (src - closestHit.pos).magnitudeSq()))
				{
					closestHit = { hit,p.normal, m->material, true };
				}	
			}
		}

		return closestHit;
	}
	
	void rotateCam(double a, double b, double c)
	{
		camOrigin.rotateX(a);
		front.rotateX(a);
		right.rotateX(a);
		up.rotateX(a);

		camOrigin.rotateY(b);
		front.rotateY(b);
		right.rotateY(b);
		up.rotateY(b);

		camOrigin.rotateZ(c);
		front.rotateZ(c);
		right.rotateZ(c);
		up.rotateZ(c);

		front.normalize();
		right.normalize();
		up.normalize();
	}

	void handleInput(Controls& controls)
	{
		if (controls.type == SDL_KEYUP)
		{
			if (controls.key == SDLK_r)
			{
				isPersp = !isPersp;
			}

			if (controls.key == SDLK_z)
			{
				nextMode();
			}
		}

		if (controls.type == SDL_MOUSEWHEEL)
		{
			camOrigin = camOrigin + controls.event.wheel.y * CAMERA_ZOOM_SPEED * front;
		}

		if (controls.type == SDL_MOUSEMOTION)
		{
			if (controls.button == SDL_BUTTON_MIDDLE)
			{
				yaw += rotSpeed * (controls.dx);
				rotateCam(0, rotSpeed * (controls.dx), 0);

				rotateCam(0, -yaw, 0);
				rotateCam(rotSpeed * (controls.dy), 0, 0);
				rotateCam(0, yaw, 0);
			}
		}
		const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	}

protected:
	//Get camera-space coordinates of ray projected onto focal plane
	Vector2 orthographicProjection(Vector3 point, bool& v) const
	{
		//t means how many times we need to go -front to hit focal plane.
		const Vector3 focalPlane = camOrigin + FocalLength * front;
		Vector3 tmp = intersectPoint(front, point, front, focalPlane);

		//x and y coordinates of the ray in screen space
		const float projX = (tmp - camOrigin) * right;
		const float projY = (tmp - camOrigin) * up;

		const Vector2 result(projX, projY);

		return result;
	}

	Vector2 perspectiveProjection(Vector3 point, bool& v) const
	{
		const Vector3 ray = camOrigin - point;

		const Vector3 focalPlane = camOrigin + FocalLength * front;
		if (ray * front > 0)
			v = false;
		//vector that lies on focal plane
		Vector3 hit = intersectPoint(ray, camOrigin, front, focalPlane);

		const float projX = (hit - focalPlane) * right;
		const float projY = (hit - focalPlane) * up;

		const Vector2 result = {projX, projY};
		return result;
	}

	void nextMode()
	{
		MODE < 2 ? MODE++ : MODE = 1;
	}
};
