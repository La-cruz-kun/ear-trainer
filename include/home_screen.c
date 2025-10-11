#include "ear_trainer.h"
#include <raylib.h>

static int finish = 0;
static float offset = 0;
static float velocity = 0;

void
UpdateHomeScreen (void)
{
    finish = 0;
    if (IsKeyDown (KEY_RIGHT))
        camera2d.offset.x += 5;
    if (IsKeyDown (KEY_LEFT))
        camera2d.offset.x -= 5;
    if (GetMusicTimePlayed (music[current_music_index])
        >= GetMusicTimeLength (music[current_music_index]) - 4)
        {
            StopMusicStream (music[current_music_index]);
            current_music_index = GetRandomValue (0, NO_OF_MUSIC - 1);
            printf ("%d\n", current_music_index);
        }
    if (!IsMusicStreamPlaying (music[current_music_index]))
        {
            PlayMusicStream (music[current_music_index]);
            read_id3v2 (music_file[current_music_index]);
        }
    UpdateMusicStream (music[current_music_index]);

    AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                        GetRenderWidth () * 3 / 4.0, 0,
                        GetRenderHeight () / 20.0, no_of_buttons[HOME_SCREEN],
                        buttons[HOME_SCREEN]);

    if (IsMenuButtonPressed (&buttons[HOME_SCREEN][0]))
        {
            finish = 1;
        }
    if (IsMenuButtonPressed (&buttons[HOME_SCREEN][1]))
        {
            finish = 2;
        }
    if (IsMenuButtonPressed (&buttons[HOME_SCREEN][2]))
        {
            finish = 3;
        }
    if (IsMenuButtonPressed (&buttons[HOME_SCREEN][3]))
        {
        }
    if (IsMenuButtonPressed (&buttons[HOME_SCREEN][4]))
        {
            finish = 5;
        }
    calcMenuButtonPosition (&buttons[HOME_SCREEN][0], &buttons[HOME_SCREEN][1],
                            &buttons[HOME_SCREEN][2], &buttons[HOME_SCREEN][3],
                            NULL);
}
void
DrawHomeScreen (void)
{

    BeginMode2D (camera2d);
    DrawBackgroundImage (images[0]);
    for (int i = 0; i < no_of_buttons[HOME_SCREEN]; i++)
        DrawMenuButton (buttons[HOME_SCREEN][i]);
    DrawTextEx (font1, TextFormat ("Currently Playing %s - %s", Title, Artist),
                (Vector2){ GetRenderWidth () * 1 / 100.0,
                           GetRenderHeight () * 1 / 100.0 },
                24 * GetRenderHeight () / 600.0, 1, DARKGREEN);
    EndMode2D ();
}

void
UpdatePauseToHome (void)
{
    if (!IsMusicStreamPlaying (music[current_music_index]))
        {
            PlayMusicStream (music[current_music_index]);
            read_id3v2 (music_file[current_music_index]);
        }
    UpdateMusicStream (music[current_music_index]);

    float stiffness = 100;
    float damping = 10;
    float displacement = GetRenderWidth () - offset;
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
            offset = GetRenderWidth ();
            screen_transition = false;
            camera2d.offset = (Vector2){ 0, 0 };
            offset = 0;
            velocity = 0;
        }
    else
        {
            camera2d.offset.x = offset;
        }
}

void
DrawPauseToHome (void)
{
    BeginMode2D (camera2d);
    DrawPauseScreen ();

    Rectangle dstRec1
        = { -GetRenderWidth (), 0, GetRenderWidth (), GetRenderHeight () };
    NPatchInfo ninePatchInfo1
        = { (Rectangle){ 0.0f, 0.0f, images[0].width, images[0].height },
            0,
            0,
            0,
            0,
            NPATCH_NINE_PATCH };
    DrawTextureNPatch (images[0], ninePatchInfo1, dstRec1, Vector2Zero (), 0,
                       RAYWHITE);
    AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                        GetRenderWidth () * 3 / 4.0 - GetRenderWidth (), 0,
                        GetRenderHeight () / 20.0, no_of_buttons[HOME_SCREEN],
                        buttons[HOME_SCREEN]);

    for (int i = 0; i < no_of_buttons[HOME_SCREEN]; i++)
        DrawMenuButton (buttons[HOME_SCREEN][i]);

    DrawTextEx (font1, TextFormat ("Currently Playing %s - %s", Title, Artist),
                (Vector2){ GetRenderWidth () * 1 / 100.0,
                           GetRenderHeight () * 1 / 100.0 },
                24 * GetRenderHeight () / 600.0, 1, DARKGREEN);
    EndMode2D ();
}

int
FinishHomeScreen (void)
{
    return finish;
}
