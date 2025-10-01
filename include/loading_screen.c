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
                            }
                    }
                    break;
                case STATE_FINISHED:
                    {
                        logoPositionX = GetRenderWidth () / 2 - 128;
                        logoPositionY = GetRenderHeight () / 2 - 128;

                        topSideRecWidth = 16;
                        leftSideRecHeight = 16;
                        bottomSideRecWidth = 16;
                        rightSideRecHeight = 16;
                        framesCounter = 0;
                        alpha = 1.0f;
                        state = 1;
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
                            lettersCount++;
                            framesCounter = 0;
                        }
                }
            else // When all letters have appeared, just fade out everything
                {
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
                    DrawText ("PRESS ENTER to START LOADING DATA", 150, 170,
                              20, DARKGRAY);
                    break;
                case STATE_LOADING:
                    {
                        DrawRectangle (150, 200, 0, 60, SKYBLUE);
                        if ((framesCounter / 15) % 2)
                            DrawText ("LOADING DATA...", 240, 210, 40,
                                      DARKBLUE);
                    }
                    break;
                case STATE_FINISHED:
                    {
                        DrawRectangle (150, 200, 500, 60, LIME);
                        DrawText ("DATA LOADED!", 250, 210, 40, GREEN);
                    }
                    break;
                default:
                    break;
                }
        }
    else if (state == 1) // Draw blinking top-left square corner
        {
            if ((framesCounter / 10) % 2)
                DrawRectangle (logoPositionX, logoPositionY, 16, 16, BLACK);
        }
    else if (state == 2) // Draw bars animation: top and left
        {
            /* DrawRectangle (logoPositionX, logoPositionY, topSideRecWidth,
             * 16, */
            /*                BLACK); */
            DrawRectangle (logoPositionX, logoPositionY, 16, leftSideRecHeight,
                           BLACK);

            DrawRectanglePro (
                (Rectangle){ logoPositionX + 300 + 120, logoPositionY, 16,
                             leftSideRecHeight },
                (Vector2){ 0, 0 }, 22.5,
                BLACK); // Draw a color-filled rectangle with pro parameters
            /* DrawRectangle (logoPositionX + 300 + 120, logoPositionY, 16,
             * leftSideRecHeight, */
            /*                BLACK); */
            /* DrawRectangle (logoPositionX + 300 + 120, logoPositionY, 16,
             * leftSideRecHeight, */
            /*                BLACK); */
            DrawRectanglePro (
                (Rectangle){ logoPositionX + 300 + 120, logoPositionY, 16,
                             leftSideRecHeight },
                (Vector2){ 0, 0 }, -22.5,
                BLACK); // Draw a color-filled rectangle with pro parameters
        }
    else if (state == 3) // Draw bars animation: bottom and right
        {
            /* DrawRectangle (logoPositionX, logoPositionY, topSideRecWidth,
             * 16, */
            /*                BLACK); */
            DrawRectangle (logoPositionX, logoPositionY, 16, leftSideRecHeight,
                           BLACK);

            /* DrawRectangle (logoPositionX + 240, logoPositionY, 16, */
            /*                rightSideRecHeight, BLACK); */
            DrawRectangle (logoPositionX, logoPositionY + 240,
                           bottomSideRecWidth, 16, BLACK);

            DrawRectanglePro (
                (Rectangle){ logoPositionX + 300 + 120, logoPositionY, 16,
                             leftSideRecHeight },
                (Vector2){ 0, 0 }, 22.5,
                BLACK); // Draw a color-filled rectangle with pro parameters
            DrawRectanglePro (
                (Rectangle){ logoPositionX + 300 + 120, logoPositionY, 16,
                             leftSideRecHeight },
                (Vector2){ 0, 0 }, -22.5,
                BLACK); // Draw a color-filled rectangle with pro parameters
            DrawRectangle (logoPositionX + 300 + 60, logoPositionY + 120,
                           (unsigned int)(bottomSideRecWidth - 100), 16,
                           BLACK);
        }
    else if (state == 4) // Draw "raylib" text-write animation + "powered by"
        {
            /* DrawRectangle (logoPositionX, logoPositionY, topSideRecWidth,
             * 16, */
            /*                Fade (BLACK, alpha)); */
            DrawRectangle (logoPositionX, logoPositionY + 16, 16,
                           leftSideRecHeight - 32, Fade (BLACK, alpha));

            /* DrawRectangle (logoPositionX + 240, logoPositionY + 16, 16, */
            /*                rightSideRecHeight - 32, Fade (BLACK, alpha)); */
            DrawRectangle (logoPositionX, logoPositionY + 240,
                           bottomSideRecWidth, 16, Fade (BLACK, alpha));

            DrawRectangle (GetRenderWidth () / 2 - 112,
                           GetRenderHeight () / 2 - 112, 224, 224,
                           Fade (RAYWHITE, alpha));

            DrawRectanglePro (
                (Rectangle){ logoPositionX + 300 + 120, logoPositionY, 16,
                             leftSideRecHeight },
                (Vector2){ 0, 0 }, 22.5,
                Fade (BLACK, alpha)); // Draw a color-filled rectangle with pro
                                      // parameters
            DrawRectanglePro (
                (Rectangle){ logoPositionX + 300 + 120, logoPositionY, 16,
                             leftSideRecHeight },
                (Vector2){ 0, 0 }, -22.5,
                Fade (BLACK, alpha)); // Draw a color-filled rectangle with pro
                                      // parameters
            DrawRectangle (logoPositionX + 300 + 60, logoPositionY + 120,
                           (unsigned int)(bottomSideRecWidth - 100), 16,
                           Fade (BLACK, alpha));
            DrawText (TextSubtext ("Cruz", 0, lettersCount),
                      GetRenderWidth () / 2 - 44, GetRenderHeight () / 2 + 48,
                      50, Fade (BLACK, alpha));

            if (framesCounter > 20)
                DrawText ("made by", logoPositionX, logoPositionY - 27, 20,
                          Fade (DARKGRAY, alpha));
        }
}

bool
FinishLoadingScreen (void)
{
    return finish;
}
