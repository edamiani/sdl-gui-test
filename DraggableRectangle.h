#pragma once

#include "GuiComponent.h"

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