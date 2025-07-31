// InputHandler_windows.cpp

#include "core/input.h"
#include <windows.h>
#include <iostream>

#include "core/entity.h" 
#include <core/components.h>
#include <core/world.h>

EventDispatcher* InputHandler::inputDispatcher = new EventDispatcher();


void initInputHandlers() {
    std::cout << "Windows input initialized. Press ESC to quit.\n";
	
    InputHandler::GetInstance()->inputDispatcher->Subscribe<KeyPressedEvent>([](std::shared_ptr<KeyPressedEvent> e)
        {

			extern World* myWorld;
            Entity player = myWorld->get_entity_by_id(50);


            switch (e->key) 

            {
                case KeyCode::Left:
                    if (player.has_component<engine::PositionComponent>()) {
                        player.get_component<engine::PositionComponent>().x -= 0.01f;
                    }
				    break;
                case KeyCode::Right:
                    if (player.has_component<engine::PositionComponent>()) {
                        player.get_component<engine::PositionComponent>().x += 0.01f;
                    }
                    break;
				case KeyCode::Up:
                    if (player.has_component<engine::PositionComponent>()) {
                        player.get_component<engine::PositionComponent>().y+= 0.01f;
                    }
					break;
				case KeyCode::Down:
                    if (player.has_component<engine::PositionComponent>()) {
                        player.get_component<engine::PositionComponent>().y -= 0.01f;
                    }
					break;
            }
        });
}

bool isKeyPressed(int vkey) {
    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}

void pollInput() {
    if (isKeyPressed(VK_LEFT)) InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Left));
    if (isKeyPressed(VK_RIGHT))InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Right));
    if (isKeyPressed(VK_UP)) InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Up));
    if (isKeyPressed(VK_DOWN)) InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Down));
    if (isKeyPressed(VK_ESCAPE)) {
        std::cout << "Escape pressed, quitting...\n";
        exit(0);
    }
}

void shutdownInputHandlers() {
    // Optional cleanup
}
