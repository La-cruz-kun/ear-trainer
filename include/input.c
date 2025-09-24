#include "ear_trainer.h"

bool IsMenuButtonPressed(MenuButton *button) {
    if (CheckCollisionPointRec(GetMousePosition(), button->bound)) {
        button->action = HOVER;
        button->bound.width = button->bound.width * 1.2;
        button->bound.height = button->bound.height *1.2;
        button->bound.x = button->bound.x * 0.98;
        button->bound.y = button->bound.y * 0.98;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            button->action = CLICK;
            return true;
        }
    }
    return false;

}
