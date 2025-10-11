#include "ear_trainer.h"
#include <stdarg.h>
MenuButton
CreateMenuButton (Texture image, const char *text, Color text_color,
                  Rectangle bound)
{
    return (MenuButton){ .image = image,
                         .text = text,
                         .text_color = text_color,
                         .bound = bound,
                         .is = true,
                         .action = NOT_HOVER };
}

void
calcMenuButtonPosition (MenuButton *button, ...)
{
    va_list args;
    va_start (args, button);
    MenuButton *next;
    float padding = GetRenderHeight () / 20.0;
    float position = button->bound.y + button->bound.height + padding;
    while ((next = va_arg (args, MenuButton *)) != NULL)
        {
            next->bound.y = position;
            position = next->bound.y + next->bound.height + padding;
        }
    va_end (args);
}

void
AlignScreenButtons (float height, float width, float x, float y, float padding,
                    int no_of_buttons, MenuButton *buttons)
{
    for (int i = 0; i < no_of_buttons; i++)
        {
            buttons[i].bound = (Rectangle){
                .width = width,
                .height = height,
                .x = x,
                .y = (float)GetRenderHeight () / (no_of_buttons + 1)
                     - height / 2 + (i * (height + padding)) + y
            };
        }
}
