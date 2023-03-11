#include <algorithm>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>

class GuiComponent 
{
public:
	virtual ~GuiComponent() = default;
	GuiComponent(const SDL_Rect& rect) : mRect(rect) {}

	virtual GuiComponent* clone() const = 0;
	virtual bool handleEvent(const SDL_Event& event) { return false; }
	virtual void update() {}
	virtual void render() {}

	virtual bool containsPoint(int x, int y) const { return false; }

	SDL_Rect &getRect() { return mRect; }
	void setRect(const SDL_Rect &rect) { mRect = rect; }

	virtual bool isDragging() const { return false; }

private:
	SDL_Rect mRect;
};

class DraggableRectangle : public GuiComponent 
{
public:
	DraggableRectangle(SDL_Renderer* renderer, const SDL_Rect& rect, const SDL_Color& color)
		: GuiComponent(rect), mRenderer(renderer), mColor(color) {}

	bool handleEvent(const SDL_Event& event) override 
	{
		if (event.type == SDL_MOUSEBUTTONDOWN) 
		{
			int x, y;
			SDL_GetMouseState(&x, &y);

			SDL_Point mousePoint = { x, y };
			if (SDL_PointInRect(&mousePoint, &getRect())) 
			{
				mDragging = true;
				mDragOffsetX = x - getRect().x;
				mDragOffsetY = y - getRect().y;

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

			getRect().x = x - mDragOffsetX;
			getRect().y = y - mDragOffsetY;

			return true;
		}

		return false;
	}

	void render() override 
	{
		SDL_SetRenderDrawColor(mRenderer, mColor.r, mColor.g, mColor.b, mColor.a);
		SDL_RenderFillRect(mRenderer, &getRect());
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
			if (component->handleEvent(event)) 
			{ 
				if (component->isDragging())
				{
					for (auto& other : mComponents) 
					{
						if (other == component || other->isDragging()) 
						{
							// Skip self and other components being dragged.
							continue;
						}

						auto nearSide = whichSideIsNear(component->getRect(), other->getRect(), mThreshold);
						if (nearSide != RectSide::None) 
						{
							// Snap to other component.
							auto rect = component->getRect();
							auto otherRect = other->getRect();
							// Adjust position based on which edge is closest.
							// This example snaps to the top edge of the other component.
							switch (nearSide)
							{
							case RectSide::Left:
								rect.x = otherRect.x - rect.w;
								component->setRect(rect);
								break;
							case RectSide::Right:
								rect.x = otherRect.x + otherRect.w;
								component->setRect(rect);
								break;
							case RectSide::Top:
								rect.y = otherRect.y - rect.h;
								component->setRect(rect);
								break;
							case RectSide::Bottom:
								rect.y = otherRect.y + otherRect.h;
								component->setRect(rect);
								break;
							}
						}
					}
				}

				break; 
			}
		}
	}

	enum class RectSide { Left, Right, Top, Bottom, None };

	bool isNear(const SDL_Rect& rect1, const SDL_Rect& rect2, int threshold) 
	{
		// Check if left side of rect1 is near the right side of rect2
		if (abs(rect1.x - (rect2.x + rect2.w)) <= threshold &&
			((rect1.y >= rect2.y - threshold &&
			rect1.y <= rect2.y + rect2.h + threshold)
			||
			(rect1.y + rect1.w >= rect2.y - threshold &&
			rect1.y + rect1.w <= rect2.y + rect2.h + threshold))) 
		{
			return true;
		}
		// Check if right side of rect1 is near the left side of rect2
		if (abs(rect2.x - (rect1.x + rect1.w)) <= threshold &&
			((rect1.y >= rect2.y - threshold &&
			rect1.y <= rect2.y + rect2.h + threshold)
			||
			(rect1.y + rect1.w >= rect2.y - threshold &&
			rect1.y + rect1.w <= rect2.y + rect2.h + threshold))) 
		{
			return true;
		}
		// Check if top side of rect1 is near the bottom side of rect2
		if (abs(rect1.y - (rect2.y + rect2.h)) <= threshold &&
			((rect1.x >= rect2.x - threshold &&
			rect1.x <= rect2.x + rect2.w + threshold)
			||
			(rect1.x + rect1.w >= rect2.x - threshold &&
			rect1.x + rect1.w <= rect2.x + rect2.w + threshold))) 
		{
			return true;
		}
		// Check if bottom side of rect1 is near the top side of rect2
		if (abs(rect2.y - (rect1.y + rect1.h)) <= threshold &&
			((rect1.x >= rect2.x - threshold &&
			rect1.x <= rect2.x + rect2.w + threshold)
			||
			(rect1.x + rect1.w >= rect2.x - threshold &&
			rect1.x + rect1.w <= rect2.x + rect2.w + threshold))) 
		{
			return true;
		}
		return false;
	}

	bool isDiagonal(const SDL_Rect& rect1, const SDL_Rect& rect2) 
	{
		// Check if the two rectangles overlap
		if (SDL_HasIntersection(&rect1, &rect2)) {
			return false;
		}

		int l1 = rect1.x;
		int l2 = rect2.x;
		int r1 = rect1.x + rect1.w;
		int r2 = rect2.x + rect2.w;
		int t1 = rect1.y;
		int t2 = rect2.y;
		int b1 = rect1.y + rect1.h;
		int b2 = rect2.y + rect2.h;

		return
			(r1 < l2 && b1 < t2) ||
			(l1 > r2 && b1 < t2) ||
			(r1 < l2 && t1 > b2) ||
			(l1 > r2 && t1 > b2);
	}


	RectSide whichSideIsNear(const SDL_Rect& r1, const SDL_Rect& r2, int threshold) 
	{
		// Check if the rectangles overlap
		bool near = isNear(r1, r2, threshold);
		bool diagonal = isDiagonal(r1, r2);
		if (near && !diagonal)
		{
			// Determine which side is near to the other rectangle
			int left = abs(r1.x + r1.w - r2.x);
			int right = abs(r2.x + r2.w - r1.x);
			int top = abs(r1.y + r1.h - r2.y);
			int bottom = abs(r2.y + r2.h - r1.y);

			int minDistance = std::min({ left, right, top, bottom });

			if (minDistance <= threshold) 
			{
				if (minDistance == left) {
					return RectSide::Left;
				}
				else if (minDistance == right) {
					return RectSide::Right;
				}
				else if (minDistance == top) {
					return RectSide::Top;
				}
				else {
					return RectSide::Bottom;
				}
			}
		}
		return RectSide::None;
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
	int mThreshold{ 20 };
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

		//SDL_Delay(10);

		controller.render(); // Render the GUI with the GuiController
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}