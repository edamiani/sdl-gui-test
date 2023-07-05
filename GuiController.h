#pragma once

#include <memory>
#include <vector>

#include "Enums.h"
#include "GuiComponent.h"

class GuiController
{
public:
	GuiController(SDL_Renderer* renderer) : _renderer(renderer) {}

	void handleEvent(const SDL_Event& event);
	void render();
	void addComponent(std::unique_ptr<GuiComponent> component);

private:
	SDL_Renderer* _renderer;
	std::vector<std::unique_ptr<GuiComponent>> _components;
	int _threshold{ 20 };
};