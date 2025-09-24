#include "ear_trainer.h"
#include <stdarg.h>
MenuButton CreateMenuButton(Texture image, const char *text, Color text_color, Rectangle bound) {
    return (MenuButton) {
        .image = image,
        .text = text,
        .text_color = text_color,
        .bound = bound,
        .is = true,
        .action = NOT_HOVER
    };
}

void calcMenuButtonPosition(MenuButton *button, ...) {
    va_list args;
    va_start(args, button);
    MenuButton *next;
    float padding = GetRenderHeight() / 20.0;
    float position = button->bound.y + button->bound.height + padding;
    while ((next = va_arg(args, MenuButton *)) != NULL) {
        next->bound.y = position;
        position = next->bound.y + next->bound.height + padding;

    }
    va_end(args);
}
