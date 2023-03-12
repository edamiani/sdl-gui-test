#pragma once

#include <algorithm>
#include <SDL2/SDL.h>

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
		(r1 < l2&& b1 < t2) ||
		(l1 > r2 && b1 < t2) ||
		(r1 < l2&& t1 > b2) ||
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