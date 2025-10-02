#include "ear_trainer.h"

static int finish = 0;

void
UpdateHomeScreen (void)
{
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
            former_screen = current_screen;
            finish = 1;
            looped_once = false;
            screen_transition = true;
        }
    if (IsMenuButtonPressed (&buttons[HOME_SCREEN][1]))
        {
        }
    if (IsMenuButtonPressed (&buttons[HOME_SCREEN][2]))
        {
        }
    if (IsMenuButtonPressed (&buttons[HOME_SCREEN][3]))
        {
            quit = true;
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
    DrawTextEx (font1,
                TextFormat ("Currently Playing %s - %s", Title, Artist),
                (Vector2){ GetRenderWidth () * 1 / 100.0,
                           GetRenderHeight () * 1 / 100.0 },
                24 * GetRenderHeight () / 600.0, 1, DARKGREEN);
    DrawFPS (10, 10);
    EndMode2D ();
}

int FinishHomeScreen(void){
  return finish;
}
