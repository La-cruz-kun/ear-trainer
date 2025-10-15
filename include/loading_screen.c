// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Abraham Kopiam Cruz (@La-cruz-kun)


#include "ear_trainer.h"
#include <raylib.h>

static enum {
    STATE_WAITING,
    STATE_LOADING,
    STATE_FINISHED
} loadState
    = STATE_WAITING;

static int framesCounter = 0;
static bool finish = false;
static int state = 0;
static int topSideRecWidth = 0;
static int leftSideRecHeight = 0;
static int bottomSideRecWidth = 0;
static int rightSideRecHeight = 0;
static int lettersCount = 0;
static float alpha = 0;

static int logoPositionX = 0;
static int logoPositionY = 0;
static Sound key_click = { 0 };
static Sound enter_click = { 0 };

void
InitLoadingScreen (void)
{
    key_click = LoadSound ("resources/audio/spacebarclick.mp3");
    enter_click = LoadSound ("resources/audio/enterkey.mp3");
}

void
UpdateLoadingScreen (void)
{

    if (state == 0)
        {
            switch (loadState)
                {
                case STATE_WAITING:
                    {

                        loadState = STATE_LOADING;
                    }
                    break;
                case STATE_LOADING:
                    {
                        framesCounter++;
                        if (init () && LoadResources () && LoadUi ()
                            && LoadPianoModel () && LoadPlaneModel ()
                            && LoadSoundInstrument () && MapPianoNotes ())
                            {
                                loadState = STATE_FINISHED;
                                framesCounter = 0;
                            }
                    }
                    break;
                case STATE_FINISHED:
                    {
                        framesCounter++;
                        logoPositionX = GetRenderWidth () / 2 - 256;
                        logoPositionY = GetRenderHeight () / 2 - 128;

                        topSideRecWidth = 16;
                        leftSideRecHeight = 16;
                        bottomSideRecWidth = 16;
                        rightSideRecHeight = 16;
                        alpha = 1.0f;
                        if (framesCounter == 80)
                            {
                                state = 1;
                                framesCounter = 0;
                            }
                    }
                    break;
                default:
                    break;
                }
        }
    else if (state == 1) // State 0: Top-left square corner blink logic
        {
            framesCounter++;

            if (framesCounter == 80)
                {
                    state = 2;
                    framesCounter
                        = 0; // Reset counter... will be used later...
                }
        }
    else if (state == 2) // State 1: Bars animation logic: top and left
        {
            topSideRecWidth += 8;
            leftSideRecHeight += 8;

            if (topSideRecWidth == 256)
                state = 3;
        }
    else if (state == 3) // State 2: Bars animation logic: bottom and right
        {
            bottomSideRecWidth += 8;
            rightSideRecHeight += 8;

            if (bottomSideRecWidth == 256)
                state = 4;
        }
    else if (state == 4) // State 3: "raylib" text-write animation logic
        {
            framesCounter++;

            if (lettersCount < 10)
                {
                    if (framesCounter
                        / 12) // Every 12 frames, one more letter!
                        {
                            if (lettersCount < 4)
                                {
                                    if (IsSoundPlaying (key_click))
                                        StopSound (key_click);
                                    PlaySound (key_click);
                                }
                            lettersCount++;
                            framesCounter = 0;
                        }
                }
            else // When all letters have appeared, just fade out everything
                {
                    StopSound (key_click);
                    if (framesCounter == 21)
                        {
                            PlaySound (enter_click);
                        }
                    if (framesCounter > 200)
                        {
                            alpha -= 0.02f;

                            if (alpha <= 0.0f)
                                {
                                    alpha = 0.0f;
                                    finish = true;
                                }
                        }
                }
        }
}

void
DrawLoadingScreen (void)
{
    if (state == 0)
        {
            switch (loadState)
                {
                case STATE_WAITING:
                    DrawText ("PRESS ENTER to START LOADING DATA",
                              GetRenderWidth () / 2.0,
                              GetRenderHeight () / 2.0, 20, DARKGRAY);
                    break;
                case STATE_LOADING:
                    {
                        DrawRectangle (150, 200, 0, 60, SKYBLUE);
                        if ((framesCounter / 15) % 2)
                            DrawTextEx (
                                font2, "LOADING DATA...",
                                (Vector2){ GetRenderWidth () / 2.0
                                               - TextLength ("LOADING DATA...")
                                                     * 64 / 4.0,
                                           GetRenderHeight () / 2.0 },
                                64, 1, DARKGRAY); // Draw text using font and
                                                  // additional parameters
                    }
                    break;
                case STATE_FINISHED:
                    {
                        DrawRectangle (GetRenderWidth () / 2.0 - 500 / 2.0,
                                       GetRenderHeight () / 2.0, 500, 60,
                                       DARKGRAY);
                        DrawTextEx (
                            font2, "DATA LOADED!",
                            (Vector2){ GetRenderWidth () / 2.0
                                           - TextLength ("DATA LOADED!") * 64
                                                 / 4.0,
                                       GetRenderHeight () / 2.0 },
                            64, 1, RAYWHITE); // Draw text using font and
                                              // additional parameters
                    }
                    break;
                default:
                    break;
                }
        }
    else if (state == 1) // Draw blinking top-left square corner
        {
            if ((framesCounter / 10) % 2)
                DrawRectangle (logoPositionX, logoPositionY, 16, 16, RAYWHITE);
        }
    else if (state == 2) // Draw bars animation: top and left
        {
            DrawRectangle (logoPositionX, logoPositionY, 16, leftSideRecHeight,
                           RAYWHITE);

            DrawRectanglePro (
                (Rectangle){ logoPositionX + 300 + 120, logoPositionY, 16,
                             leftSideRecHeight },
                (Vector2){ 0, 0 }, 22.5,
                RAYWHITE); // Draw a color-filled rectangle with pro parameters
            DrawRectanglePro (
                (Rectangle){ logoPositionX + 300 + 120, logoPositionY, 16,
                             leftSideRecHeight },
                (Vector2){ 0, 0 }, -22.5,
                RAYWHITE); // Draw a color-filled rectangle with pro parameters
        }
    else if (state == 3) // Draw bars animation: bottom and right
        {
            DrawRectangle (logoPositionX, logoPositionY, 16, leftSideRecHeight,
                           RAYWHITE);

            DrawRectangle (logoPositionX, logoPositionY + 240,
                           bottomSideRecWidth, 16, RAYWHITE);

            DrawRectanglePro ((Rectangle){ logoPositionX + 300 + 120,
                                           logoPositionY, 16,
                                           leftSideRecHeight },
                              (Vector2){ 0, 0 }, 22.5, RAYWHITE);
            DrawRectanglePro ((Rectangle){ logoPositionX + 300 + 120,
                                           logoPositionY, 16,
                                           leftSideRecHeight },
                              (Vector2){ 0, 0 }, -22.5, RAYWHITE);
            DrawRectangle (logoPositionX + 300 + 60, logoPositionY + 120,
                           (unsigned int)(bottomSideRecWidth - 100), 16,
                           RAYWHITE);
        }
    else if (state == 4) // Draw "raylib" text-write animation + "powered by"
        {
            DrawRectangle (logoPositionX, logoPositionY + 16, 16,
                           leftSideRecHeight - 32, Fade (RAYWHITE, alpha));

            DrawRectangle (logoPositionX, logoPositionY + 240,
                           bottomSideRecWidth, 16, Fade (RAYWHITE, alpha));

            /* DrawRectangle (GetRenderWidth () / 2 - 112, */
            /*                GetRenderHeight () / 2 - 112, 224, 224, */
            /*                Fade (RAYWHITE, alpha)); */

            DrawRectanglePro (
                (Rectangle){ logoPositionX + 300 + 120, logoPositionY, 16,
                             leftSideRecHeight },
                (Vector2){ 0, 0 }, 22.5,
                Fade (RAYWHITE, alpha)); // Draw a color-filled rectangle with
                                         // pro parameters
            DrawRectanglePro (
                (Rectangle){ logoPositionX + 300 + 120, logoPositionY, 16,
                             leftSideRecHeight },
                (Vector2){ 0, 0 }, -22.5,
                Fade (RAYWHITE, alpha)); // Draw a color-filled rectangle with
                                         // pro parameters
            DrawRectangle (logoPositionX + 300 + 60, logoPositionY + 120,
                           (unsigned int)(bottomSideRecWidth - 100), 16,
                           Fade (RAYWHITE, alpha));

            DrawTextEx (font3, TextSubtext ("Cruz", 0, lettersCount),
                        (Vector2){ GetRenderWidth () / 2.0 - 96,
                                   GetRenderHeight () / 2.0 + 48 },
                        64, 1, Fade (RAYWHITE, alpha));

            if (framesCounter > 20)
                DrawTextEx (font3, "made by",
                            (Vector2){ logoPositionX, logoPositionY - 27 }, 30,
                            1, Fade (DARKGRAY, alpha));
        }
}

bool
FinishLoadingScreen (void)
{
    return finish;
}
