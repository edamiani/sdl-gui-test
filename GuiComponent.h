#pragma once

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

	SDL_Rect& getRect() { return mRect; }
	void setRect(const SDL_Rect& rect) { mRect = rect; }

	virtual bool isDragging() const { return false; }

private:
	SDL_Rect mRect;
};