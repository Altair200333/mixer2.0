#pragma once
#include <iostream>
#include <SDL.h>

#include "camera.h"
#include "Controls.h"
#include "libbitmap.h"
#include "vector3.h"
#include "renderer.h"

class Scene
{
	bool running = true;
public:
	SDL_Window* window;
	SDL_Renderer* pen;
	Camera* camera;

	int width, height;

	Renderer* renderer;
	Controls controls;
	//-----
	Vector3 v[3] = { { 40,40,40 }, { 20,20,20 }, { 80,20,20 } };
	std::vector<Polygon> scenePolygons;
	std::vector<Mesh*> meshes;
	Scene(int w, int h)
	{
		width = w;
		height = h;

		SDL_Init(SDL_INIT_VIDEO);
		window = SDL_CreateWindow("MIXER", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
			SDL_WINDOW_OPENGL);
		pen = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

		SDL_SetRenderDrawColor(pen, 100, 100, 100, 255);

		camera = new Camera();

		renderer = new Renderer(w, h);
		
		//addMesh("box.stl");
		//addMesh("box2.stl");
		addMesh("ico.stl", { {250,100,100}, false });
		addMesh("pl.stl", { {100,100,100}, true });
		//addMesh("uv.stl", { {100,100,100}, true });
	}

	Vector3 zero = { 0,0, 0 };
	Vector3 ax1 = { 50,0,0 };
	Vector3 ax2 = { 0, 50,0 };
	Vector3 ax3 = { 0,0,50 };
	void addMesh(const char* fname, Material mat)
	{
		Mesh* mesh = new Mesh();
		mesh->material = mat;
		mesh->STLImport(fname);
		for (auto p : mesh->polygons)
		{
			scenePolygons.push_back(p);
		}
		meshes.push_back(mesh);	
	}
	void addMesh(const char* fname)
	{
		addMesh(fname, { {200,200,200},false });
	}
	void mainLoop()
	{
		while (running)
		{
			//SDL_RenderPresent(renderer);
			SDL_SetRenderDrawColor(pen, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderClear(pen);
			
			while (SDL_PollEvent(&controls.event) != 0)
			{
				if (controls.event.type == SDL_QUIT)
				{
					running = false;
				}
				controls.updateState(controls.event);
				handleInput();
			}
			
			camera->render(renderer, pen, scenePolygons);


			
			SDL_RenderPresent(pen);
		}
	}
	void handleInput()
	{		
		camera->handleInput(controls);
		if(controls.key == SDLK_F12 && controls.type == SDL_KEYUP)
		{
			std::cout << "Render\n";
			Bitmap img;
			img.m_width = width;
			img.m_height = height;
			int l = img.m_width * img.m_height * 3 + img.padding() * img.m_height;
			img.m_buffer = new uint8_t[l ]();
			for (int i = 0; i < l; i++)
			{
				img.m_buffer[i] = 10;
			}
			camera->render(img, meshes, width, height);

			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					int id = img.getPixelId(i, j, 0);
					SDL_SetRenderDrawColor(pen, img.m_buffer[id+2],
						img.m_buffer[id+1], img.m_buffer[id], 255);
					renderer->drawPoint(pen, i - width / 2, -(j - height / 2));
				}
				SDL_PollEvent(&controls.event);
			}
			SDL_RenderPresent(pen);
			bool show = true;
			while (show)
			{
				while (SDL_PollEvent(&controls.event) != 0)
				{
					controls.updateState(controls.event);
					if (controls.type == SDL_KEYDOWN)
						show = false;
				}
			}
			img.save("res.bmp");
		}
	}
	
};
