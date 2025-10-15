// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Abraham Kopiam Cruz (@La-cruz-kun)


#include "ear_trainer.h"

static float offset = 0;
static float velocity = 0;
static int finish = 0;

void
UpdatePauseScreen (void)
{
    finish = 0;
    if (!IsMusicStreamPlaying (music[current_music_index]))
        {
            PlayMusicStream (music[current_music_index]);
            read_id3v2 (music_file[current_music_index]);
        }
    UpdateMusicStream (music[current_music_index]);
    AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                        GetRenderWidth () * 3 / 4.0, 0,
                        GetRenderHeight () / 20.0, no_of_buttons[PAUSE_SCREEN],
                        buttons[PAUSE_SCREEN]);

    if (IsMenuButtonPressed (&buttons[PAUSE_SCREEN][0]))
        {
            camera2d.offset = (Vector2){ 0, 0 };
            if (former_screen == INTERVAL_SCREEN)
                {
                    finish = 1;
                }
            else if (former_screen == FREE_SCREEN)
                {
                    finish = 2;
                }
            else if (former_screen == CHORD_SCREEN)
                {
                    finish = 4;
                }
        }
    if (IsMenuButtonPressed (&buttons[PAUSE_SCREEN][1]))
        {
            camera2d.offset = (Vector2){ 0, 0 };
            finish = 3;
        }
    if (IsMenuButtonPressed (&buttons[PAUSE_SCREEN][2]))
        {
        }
    if (IsMenuButtonPressed (&buttons[PAUSE_SCREEN][3]))
        {
            quit = true;
        }
}

void
DrawPauseScreen (void)
{
    DrawBackgroundImage (images[1]);
    for (int i = 0; i < no_of_buttons[PAUSE_SCREEN]; i++)
        DrawMenuButton (buttons[PAUSE_SCREEN][i]);
    DrawTextEx (font1, TextFormat ("Currently Playing %s - %s", Title, Artist),
                (Vector2){ GetRenderWidth () * 1 / 100.0,
                           GetRenderHeight () * 1 / 100.0 },
                24 * GetRenderHeight () / 600.0, 1, DARKBROWN);
}

void
Update3DToPause (void)
{
    if (!IsMusicStreamPlaying (music[current_music_index]))
        {
            PlayMusicStream (music[current_music_index]);
            read_id3v2 (music_file[current_music_index]);
        }
    UpdateMusicStream (music[current_music_index]);

    SpringAnimation ();
}

void
Draw3DToPause (enum Screen screen)
{
    BeginMode3D (camera);

    DrawPlaneModel ();
    DrawPiano ();

    DrawModel (models[LIGHT_CUBE], lightPosition, 0.5f, YELLOW);
    for (int i = 0; i < MAX_CHORD_SIZE; i++)
        {
            if (note_pool[i] > -1)
                {
                    DrawBoundingBoxAsCube (keyBoxes[note_pool[i]], BLUE);
                }
        }

    EndMode3D ();
    BeginMode2D (camera2d);
    for (int i = 0; i < no_of_buttons[screen]; i++)
        DrawMenuButton (buttons[screen][i]);

    AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                        GetRenderWidth () * 3 / 4.0, GetRenderHeight (),
                        GetRenderHeight () / 20.0, no_of_buttons[PAUSE_SCREEN],
                        buttons[PAUSE_SCREEN]);

    Texture2D image = images[1];
    Rectangle dstRec1
        = { 0, GetRenderHeight (), GetRenderWidth (), GetRenderHeight () };
    NPatchInfo ninePatchInfo1
        = { (Rectangle){ 0.0f, 0.0f, image.width, image.height },
            0,
            0,
            0,
            0,
            NPATCH_NINE_PATCH };
    DrawTextureNPatch (image, ninePatchInfo1, dstRec1, Vector2Zero (), 0,
                       RAYWHITE);
    for (int i = 0; i < no_of_buttons[PAUSE_SCREEN]; i++)
        DrawMenuButton (buttons[PAUSE_SCREEN][i]);
    DrawTextEx (
        font1, TextFormat ("Currently Playing %s - %s", Title, Artist),
        (Vector2){ GetRenderWidth () * 1 / 100.0,
                   GetRenderHeight () + GetRenderHeight () * 1 / 100.0 },
        24 * GetRenderHeight () / 600.0, 1, DARKBROWN);

    EndMode2D ();
}

void
SpringAnimation (void)
{
    float stiffness = 100;
    float damping = 10;
    float displacement = GetRenderHeight () - offset;
    float springForce = stiffness * displacement;
    float dampingForce = -damping * velocity;
    float force = springForce + dampingForce;
    float speed = fabsf (velocity);
    float tolerance = 0.1;
    velocity += force * GetFrameTime ();
    offset += velocity * GetFrameTime ();
    displacement = fabsf (displacement);
    if (displacement < tolerance && speed < tolerance)
        {
            offset = GetRenderHeight ();
            screen_transition = false;
            camera2d.offset.y = -offset;
            offset = 0;
            velocity = 0;
        }
    else
        {
            camera2d.offset.y = -offset;
        }
}

int
FinishPauseScreen (void)
{
    return finish;
}
