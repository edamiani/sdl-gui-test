#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

#include "DraggableRectangle.h"
#include "GuiController.h"

SDL_Color getRandomColor() 
{
	SDL_Color color = { 0, 0, 0, 255 }; // Initialize to black with full alpha
	color.r = rand() % 256; // Random red component (0-255)
	color.g = rand() % 256; // Random green component (0-255)
	color.b = rand() % 256; // Random blue component (0-255)
	return color;
}

int main(int argc, char* argv[]) 
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow(
		"Draggable Rectangles", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	// Create a new GuiController object
	GuiController controller(renderer);

	SDL_Rect initialRect = { 50, 50, 100, 100 };

	// Add a few DraggableRectangle objects to the GuiController
	controller.addComponent(std::make_unique<DraggableRectangle>(
		renderer, initialRect, getRandomColor()));
	controller.addComponent(std::make_unique<DraggableRectangle>(
		renderer, SDL_Rect{ 200, 200, 100, 100 }, getRandomColor()));
	controller.addComponent(std::make_unique<DraggableRectangle>(
		renderer, SDL_Rect{ 400, 50, 150, 150 }, getRandomColor()));

	bool running = true;
	while (running) 
	{
		SDL_Event event;
		while (SDL_PollEvent(&event)) 
		{
			if (event.type == SDL_QUIT) 
			{
				running = false;
			}
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) 
			{
				running = false;
			}
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r && SDL_GetModState() & KMOD_CTRL) 
			{
				controller.addComponent(std::make_unique<DraggableRectangle>(
					renderer, SDL_Rect{ rand() % 500, rand() % 300, 100, 100 }, getRandomColor()));
			}
			else 
			{
				// Handle events with the GuiController
				controller.handleEvent(event);
			}
		}

		SDL_Delay(10);

		// Render the GUI with the GuiController
		controller.render();
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}