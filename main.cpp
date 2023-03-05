#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

class GuiComponent 
{
public:
	virtual ~GuiComponent() = default;

	virtual GuiComponent* clone() const = 0;
	virtual bool handleEvent(const SDL_Event& event) { return false; }
	virtual void update() {}
	virtual void render() {}

	virtual bool containsPoint(int x, int y) const { return false; }

	virtual bool isDragging() const { return false; }
};

class DraggableRectangle : public GuiComponent 
{
public:
	DraggableRectangle(SDL_Renderer* renderer, const SDL_Rect& rect, const SDL_Color& color)
		: mRenderer(renderer), mRect(rect), mColor(color) {}

	bool handleEvent(const SDL_Event& event) override 
	{
		if (event.type == SDL_MOUSEBUTTONDOWN) 
		{
			int x, y;
			SDL_GetMouseState(&x, &y);

			SDL_Point mousePoint = { x, y };
			if (SDL_PointInRect(&mousePoint, &mRect)) 
			{
				mDragging = true;
				mDragOffsetX = x - mRect.x;
				mDragOffsetY = y - mRect.y;

				return true;
			}
		}
		else if (event.type == SDL_MOUSEBUTTONUP) 
		{
			mDragging = false;
		}
		else if (event.type == SDL_MOUSEMOTION && mDragging) 
		{
			int x, y;
			SDL_GetMouseState(&x, &y);

			mRect.x = x - mDragOffsetX;
			mRect.y = y - mDragOffsetY;

			return true;
		}

		return false;
	}

	void render() override 
	{
		SDL_SetRenderDrawColor(mRenderer, mColor.r, mColor.g, mColor.b, mColor.a);
		SDL_RenderFillRect(mRenderer, &mRect);
	}

	bool isDragging() const override 
	{
		return mDragging;
	}

	DraggableRectangle* clone() const override 
	{
		return new DraggableRectangle(*this);
	}

private:
	SDL_Renderer* mRenderer;
	SDL_Rect mRect;
	SDL_Color mColor;
	bool mDragging = false;
	int mDragOffsetX = 0;
	int mDragOffsetY = 0;
};


class GuiController 
{
public:
	GuiController(SDL_Renderer* renderer) : mRenderer(renderer) {}

	void handleEvent(const SDL_Event& event) 
	{
		for (auto it = mComponents.rbegin(); it != mComponents.rend(); ++it) 
		{
			auto& component = *it;
			if(component->handleEvent(event)) break;
		}
	}

	void moveToTop(GuiComponent* component)
	{
		if (component->isDragging())
		{
			std::unique_ptr<GuiComponent> cloned = std::unique_ptr<GuiComponent>(component->clone());
			mComponents.erase(std::remove(mComponents.begin(), mComponents.end(), component), mComponents.end());
			mComponents.push_back(std::move(cloned));
		}
	}

	void render() 
	{
		SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255); // Set background color to white with full alpha
		SDL_RenderClear(mRenderer); // Clear the screen with the background color

		for (auto& component : mComponents) 
		{
			component->render();
		}

		SDL_RenderPresent(mRenderer); // Show what has been drawn so far
	}

	void addComponent(std::unique_ptr<GuiComponent> component) 
	{
		mComponents.push_back(std::move(component));
	}

private:
	SDL_Renderer* mRenderer;
	std::vector<std::unique_ptr<GuiComponent>> mComponents;
};

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

	SDL_Window* window = SDL_CreateWindow("Draggable Rectangles", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	GuiController controller(renderer); // Create a new GuiController object

	SDL_Rect initialRect = { 50, 50, 100, 100 };

	// Add a few DraggableRectangle objects to the GuiController
	controller.addComponent(std::make_unique<DraggableRectangle>(renderer, initialRect, getRandomColor()));
	controller.addComponent(std::make_unique<DraggableRectangle>(renderer, SDL_Rect{ 200, 200, 100, 100 }, getRandomColor()));
	controller.addComponent(std::make_unique<DraggableRectangle>(renderer, SDL_Rect{ 400, 50, 150, 150 }, getRandomColor()));

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
				controller.addComponent(std::make_unique<DraggableRectangle>(renderer, SDL_Rect{ rand() % 500, rand() % 300, 100, 100 }, getRandomColor()));
			}
			else 
			{
				controller.handleEvent(event); // Handle events with the GuiController
			}
		}

		SDL_Delay(10);

		controller.render(); // Render the GUI with the GuiController
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}