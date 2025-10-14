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
