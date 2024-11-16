#include "buttons.hpp"

#include "game.hpp"

buttons::ButtonLayer::ButtonLayer(int x, int y, int width, int height){
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

buttons::ButtonLayer::~ButtonLayer(){
    this->clean();
}

void buttons::ButtonLayer::addButton(Button button){
    this->buttons.push_back(button);
}

void buttons::ButtonLayer::addButton(SDL_Rect rect, SDL_Texture* texture, std::function<void()> onClick){
    Button button;
    button.rect = rect;
    button.texture = texture;
    button.onClick = onClick;
    this->buttons.push_back(button);
}

bool buttons::ButtonLayer::handleEvents(SDL_Event event){
    if(event.type == SDL_MOUSEBUTTONDOWN){
        int x, y;
        SDL_GetMouseState(&x, &y);
        for(auto button : this->buttons){
            if(x >= button.rect.x && x <= button.rect.x + button.rect.w && y >= button.rect.y && y <= button.rect.y + button.rect.h){
                button.onClick();
                return true;
            }
        }
    }
    return false;
}

void buttons::ButtonLayer::render(){
    for(auto button : this->buttons){
        SDL_RenderCopy(Game::renderer, button.texture, NULL, &button.rect);
    }
}

void buttons::ButtonLayer::clean(){
    for(auto button : this->buttons){
        SDL_DestroyTexture(button.texture);
    }
    this->buttons.clear();
}