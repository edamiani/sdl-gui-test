#include "GuiController.h"

#include "Utility.h"

void GuiController::handleEvent(const SDL_Event& event)
{
	for (auto it = _components.rbegin(); it != _components.rend(); ++it)
	{
		auto& component = *it;
		if (component->handleEvent(event))
		{
			if (component->isDragging())
			{
				for (auto& other : _components)
				{
					if (other == component || other->isDragging())
					{
						// Skip self and other components being dragged.
						continue;
					}

					auto nearSide = whichSideIsNear(component->getRect(), other->getRect(), _threshold);
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

void GuiController::render()
{
	SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255); // Set background color to white with full alpha
	SDL_RenderClear(_renderer); // Clear the screen with the background color

	for (auto& component : _components)
	{
		component->render();
	}

	SDL_RenderPresent(_renderer); // Show what has been drawn so far
}

void GuiController::addComponent(std::unique_ptr<GuiComponent> component)
{
	_components.push_back(std::move(component));
}