#include "GuiController.h"

#include "Utility.h"

void GuiController::handleEvent(const SDL_Event& event)
{
	for (auto it = _components.rbegin(); it != _components.rend(); ++it)
	{
		auto& guiComponent = *it;
		if (guiComponent->handleEvent(event))
		{
			if (guiComponent->isDragging())
			{
				for (auto& other : _components)
				{
					if (other == guiComponent || other->isDragging())
					{
						// Skip self and other components being dragged.
						continue;
					}

					auto nearSide = whichSideIsNear(guiComponent->getRect(), other->getRect(), _threshold);
					if (nearSide == RectSide::None)
					{
						continue;
					}
					
					// Snap to other component.
					auto rect = guiComponent->getRect();
					auto otherRect = other->getRect();

					// Adjust position based on which edge is closest.
					switch (nearSide)
					{
					case RectSide::Left:
						rect.x = otherRect.x - rect.w;
						break;
					case RectSide::Right:
						rect.x = otherRect.x + otherRect.w;
						break;
					case RectSide::Top:
						rect.y = otherRect.y - rect.h;
						break;
					case RectSide::Bottom:
						rect.y = otherRect.y + otherRect.h;
						break;
					}

					guiComponent->setRect(rect);
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