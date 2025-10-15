// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Abraham Kopiam Cruz (@La-cruz-kun)


#include "ear_trainer.h"
#include <raylib.h>
#include <stdarg.h>
void
DrawBackgroundImage (Texture image)
{
    Rectangle dstRec1 = { 0, 0, GetRenderWidth (), GetRenderHeight () };
    NPatchInfo ninePatchInfo1
        = { (Rectangle){ 0.0f, 0.0f, image.width, image.height },
            0,
            0,
            0,
            0,
            NPATCH_NINE_PATCH };
    DrawTextureNPatch (image, ninePatchInfo1, dstRec1, Vector2Zero (), 0,
                       RAYWHITE);
}
void
DrawMenuButton (MenuButton button)
{
    NPatchInfo ninePatchInfo1
        = { (Rectangle){ 0.0f, 0.0f, button.image.width, button.image.height },
            0,
            0,
            0,
            0,
            NPATCH_NINE_PATCH };
    DrawTextureNPatch (button.image, ninePatchInfo1, button.bound,
                       (Vector2){ 0, 0 }, 0, RAYWHITE);
    float fontSize = 24 * GetRenderHeight () / 800.0;
    DrawText (button.text,
              button.bound.x + button.bound.width / 2
                  - TextLength (button.text) * fontSize / 4,
              button.bound.y + button.bound.height / 2, fontSize,
              button.text_color);
}
