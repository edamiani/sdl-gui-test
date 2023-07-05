#pragma once

#include "GuiComponent.h"

class DraggableRectangle : public GuiComponent
{
public:
	DraggableRectangle(SDL_Renderer* renderer, const SDL_Rect& rect, const SDL_Color& color)
		: GuiComponent(rect), _renderer(renderer), _color(color) {}

	bool handleEvent(const SDL_Event& event) override
	{
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);

			SDL_Point mousePoint = { x, y };
			if (SDL_PointInRect(&mousePoint, &getRect()))
			{
				_dragging = true;
				_dragOffsetX = x - getRect().x;
				_dragOffsetY = y - getRect().y;

				return true;
			}
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			_dragging = false;
		}
		else if (event.type == SDL_MOUSEMOTION && _dragging)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);

			getRect().x = x - _dragOffsetX;
			getRect().y = y - _dragOffsetY;

			return true;
		}

		return false;
	}

	void render() override
	{
		SDL_SetRenderDrawColor(_renderer, _color.r, _color.g, _color.b, _color.a);
		SDL_RenderFillRect(_renderer, &getRect());
	}

	bool isDragging() const override
	{
		return _dragging;
	}

	DraggableRectangle* clone() const override
	{
		return new DraggableRectangle(*this);
	}

private:
	SDL_Renderer* _renderer;
	SDL_Color _color;
	bool _dragging = false;
	int _dragOffsetX = 0;
	int _dragOffsetY = 0;
};