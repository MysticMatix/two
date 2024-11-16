#pragma once

#include <SDL2/SDL.h>

#include <functional>
#include <vector>

#include "moving.hpp"

namespace buttons {
    struct Button: public moving::movable{
        SDL_Texture* texture;
        std::function<void()> onClick;
    };

    class ButtonLayer {
        public:
            /// @brief Default constructor
            ButtonLayer(int x, int y, int width, int height);
            /// @brief Default destructor
            ~ButtonLayer();

            /// @brief Add a button to the button layer
            /// @param button The button to add
            void addButton(Button button);
            /// @brief Add a button to the button layer
            /// @param rect The rectangle to render the button in
            /// @param texture The texture to render the button with
            /// @param onClick The function to call when the button is clicked
            void addButton(SDL_Rect rect, SDL_Texture* texture, std::function<void()> onClick);

            /// @brief Handle events for the button layer
            /// @param event The event to handle
            /// @return Whether the event was handled
            bool handleEvents(SDL_Event event);

            /// @brief Render the button layer
            void render();

            /// @brief Clean up the button layer
            void clean();

            /// @brief The buttons in the button layer
            std::vector<Button> buttons;

            int x, y;
            int width, height;
    };
}