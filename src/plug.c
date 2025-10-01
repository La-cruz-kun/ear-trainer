/*
 *
 * Every funny division consisting of GetRenderWidth() or GetRenderHeight()
 * is done as a ratio for different sizes of screen i haven't really dug in
 * to make it without those divisions
 */

#include "ear_trainer.h"
#include <raylib.h>
#include <rlgl.h>
#define STB_DS_IMPLEMENTATION
#include "parser.c"
#include <plug.h>
#include <raymath.h>
#include <stb_ds.h>
#include <stdio.h>

float offset = 0;
float velocity = 0;

int note_pool[POOL_SIZE] = { -1, -1, -1, -1 };
Color note_color = BLUE;

Vector3 lightPosition = (Vector3){ 2, 4, 0 };
Vector3 lightAmbient = (Vector3){ 0.1f, 0.1f, 0.1f };
Vector3 lightDiffuse = (Vector3){ 5.0f, 5.0f, 5.0f }; // Bright light
Vector3 lightSpecular = (Vector3){ 1.0f, 1.0f, 1.0f };

float lightConstant = 1.0f;
float lightLinear = 0.09f;
float lightQuadratic = 0.032f;
static char *text
    = "!#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`\
abcdefghijklmnopqrstuvwxyz{|}~¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶·¸¹º»\
¼½¾¿ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóô\
õö÷øùúûüýþÿĀāĂăĄąĆćĈĉĊċČčĎďĐđĒēĔĕĖėĘęĚěĜĝĞğĠġĢģĤĥĦħĨĩĪīĬĭĮ\
įİıĲĳĴĵĶķĸĹĺĻļĽľŁłŃńŅņŇňŊŋŌōŎŏŐőŒœŔŕŖŗŘřŚśŜŝŞşŠšŢţŤťŦŧŨũŪū\
ŬŭŮůŰűŲųŴŵŶŷŸŹźŻżŽž";

void
plug_init (Plug *plug)
{
    plug->key = -1;
    plug->scale = -1;
    plug->current_note = -1;
    int codepointCount = 0;
    int *codepoints = LoadCodepoints (text, &codepointCount);
    plug->MenuFont
        = LoadFontEx ("resources/fonts/0xProtoNerdFontMono-Italic.ttf", 64,
                      codepoints, codepointCount);
    plug->camera.position = (Vector3){ 0, 2, 2 };
    plug->camera.target = (Vector3){ 0, 0, -0.2 };
    plug->camera.up = (Vector3){ 0, 1, 0 };
    plug->camera.projection = CAMERA_PERSPECTIVE;
    plug->camera.fovy = 45.0;

    plug->camera2d.target = (Vector2){ 0, 0 };
    plug->camera2d.offset = (Vector2){ 0, 0 };
    plug->camera2d.rotation = 0.0f;
    plug->camera2d.zoom = 1.0f;

    hmdefault (plug->Key_to_note, -1);
    hmput (plug->Key_to_note, KEY_A, C1);
    hmput (plug->Key_to_note, KEY_S, D1);
    hmput (plug->Key_to_note, KEY_D, E1);
    hmput (plug->Key_to_note, KEY_F, F1);
    hmput (plug->Key_to_note, KEY_G, G1);
    hmput (plug->Key_to_note, KEY_H, A1);
    hmput (plug->Key_to_note, KEY_J, B1);
    hmput (plug->Key_to_note, KEY_K, C2);
    hmput (plug->Key_to_note, KEY_L, D2);
    hmput (plug->Key_to_note, KEY_SEMICOLON, E2);
    hmput (plug->Key_to_note, KEY_APOSTROPHE, F2);
    hmput (plug->Key_to_note, KEY_W, CS1);
    hmput (plug->Key_to_note, KEY_E, DS1);
    hmput (plug->Key_to_note, KEY_T, FS1);
    hmput (plug->Key_to_note, KEY_Y, GS1);
    hmput (plug->Key_to_note, KEY_U, AS1);
    hmput (plug->Key_to_note, KEY_O, CS2);
    hmput (plug->Key_to_note, KEY_P, DS2);

    LoadResources (plug);

    plug->current_screen = HOME_SCREEN;
    plug->current_music_index = GetRandomValue (0, NO_OF_MUSIC - 1);

    LoadUi (plug);

    LoadPianoModel (plug);
    LoadPlaneModel (plug);
    LoadSoundInstrument (plug);

    MapPianoNotes (plug);

    SetTargetFPS (60);
    PlayMusicStream (plug->music[plug->current_music_index]);
    read_id3v2 (plug->music_file[plug->current_music_index]);
}
void
plug_update (Plug *plug)
{
    switch (plug->current_screen)
        {
        case HOME_SCREEN:
            if (IsKeyDown (KEY_RIGHT))
                plug->camera2d.offset.x += 5;
            if (IsKeyDown (KEY_LEFT))
                plug->camera2d.offset.x -= 5;
            if (GetMusicTimePlayed (plug->music[plug->current_music_index])
                >= GetMusicTimeLength (plug->music[plug->current_music_index])
                       - 4)
                {
                    StopMusicStream (plug->music[plug->current_music_index]);
                    plug->current_music_index
                        = GetRandomValue (0, NO_OF_MUSIC - 1);
                    printf ("%d\n", plug->current_music_index);
                }
            if (!IsMusicStreamPlaying (plug->music[plug->current_music_index]))
                {
                    PlayMusicStream (plug->music[plug->current_music_index]);
                    read_id3v2 (plug->music_file[plug->current_music_index]);
                }
            UpdateMusicStream (plug->music[plug->current_music_index]);

            AlignScreenButtons (
                MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                GetRenderWidth () * 3 / 4.0, 0, GetRenderHeight () / 20.0,
                plug->no_of_buttons[HOME_SCREEN], plug->buttons[HOME_SCREEN]);

            if (IsMenuButtonPressed (&plug->buttons[HOME_SCREEN][0]))
                {
                    plug->former_screen = plug->current_screen;
                    plug->current_screen = INTERVAL_SETTING_SCREEN;
                    plug->looped_once = false;
                    plug->screen_transition = true;
                }
            if (IsMenuButtonPressed (&plug->buttons[HOME_SCREEN][1]))
                {
                }
            if (IsMenuButtonPressed (&plug->buttons[HOME_SCREEN][2]))
                {
                }
            if (IsMenuButtonPressed (&plug->buttons[HOME_SCREEN][3]))
                {
                    plug->quit = true;
                }
            calcMenuButtonPosition (&plug->buttons[HOME_SCREEN][0],
                                    &plug->buttons[HOME_SCREEN][1],
                                    &plug->buttons[HOME_SCREEN][2],
                                    &plug->buttons[HOME_SCREEN][3], NULL);
            BeginDrawing ();
            ClearBackground (BLACK);
            BeginMode2D (plug->camera2d);
            DrawBackgroundImage (plug->images[0]);
            for (int i = 0; i < plug->no_of_buttons[HOME_SCREEN]; i++)
                DrawMenuButton (plug->buttons[HOME_SCREEN][i]);
            DrawTextEx (
                plug->MenuFont,
                TextFormat ("Currently Playing %s - %s", Title, Artist),
                (Vector2){ GetRenderWidth () * 1 / 100.0,
                           GetRenderHeight () * 1 / 100.0 },
                24 * GetRenderHeight () / 600.0, 1, DARKGREEN);
            DrawFPS (10, 10);
            EndMode2D ();
            EndDrawing ();
            break;

        case FREE_SCREEN:
            {
                if (plug->screen_transition)
                    {
                        if (plug->former_screen == INTERVAL_SETTING_SCREEN)
                            {
                                plug->camera2d.offset = (Vector2){ 0, 0 };
                                ScreenTransitionIntSettingToInt (plug);
                            }
                    }
                plug->time = GetTime ();
                CheckKeyPress (plug);

                // Update light position and view position
                SetShaderValue (plug->shaders[PBR_SHADER],
                                plug->shaderLoc[LIGHT_POS_LOC], &lightPosition,
                                SHADER_UNIFORM_VEC3);
                SetShaderValue (plug->shaders[PBR_SHADER],
                                plug->shaderLoc[VIEW_POS_LOC],
                                &plug->camera.position, SHADER_UNIFORM_VEC3);
                SetShaderValue (plug->shaders[PLANE_SHADER],
                                plug->shaderLoc[LIGHT_POS_LOC], &lightPosition,
                                SHADER_UNIFORM_VEC3);
                SetShaderValue (plug->shaders[PLANE_SHADER],
                                plug->shaderLoc[VIEW_POS_LOC],
                                &plug->camera.position, SHADER_UNIFORM_VEC3);
                AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                                    GetRenderWidth () * 3 / 4.0, 0,
                                    GetRenderHeight () / 20.0,
                                    plug->no_of_buttons[FREE_SCREEN],
                                    plug->buttons[FREE_SCREEN]);

                if (IsMenuButtonPressed (&plug->buttons[FREE_SCREEN][0]))
                    {
                        plug->former_screen = plug->current_screen;
                        plug->current_screen = PAUSE_SCREEN;
                        plug->screen_transition = true;
                    }
                BeginDrawing ();
                ClearBackground (BROWN);
                BeginMode3D (plug->camera);

                DrawPlaneModel (plug);
                DrawPiano (plug);

                // Draw light cube
                DrawModel (plug->models[LIGHT_CUBE], lightPosition, 0.5f,
                           YELLOW);
                for (int i = 0; i < POOL_SIZE; i++)
                    {
                        if (note_pool[i] > -1)
                            {
                                DrawBoundingBoxAsCube (
                                    plug->keyBoxes[note_pool[i]], BLUE);
                            }
                    }
                /* DrawGrid(10, 1); */
                EndMode3D ();
                for (int i = 0; i < plug->no_of_buttons[FREE_SCREEN]; i++)
                    DrawMenuButton (plug->buttons[FREE_SCREEN][i]);
                DrawFPS (10, 10);
                EndDrawing ();
                break;
            }

        case INTERVAL_SETTING_SCREEN:
            {
                if (plug->screen_transition)
                    {
                        ScreenTransitionHomeToIntSetting (plug);
                        break;
                    };
                plug->camera.position = (Vector3){ 2, 4, 4 };
                MenuButton Ok = CreateMenuButton (
                    plug->images[10], "", WHITE,
                    (Rectangle){ GetRenderWidth () * 1 / 2.0,
                                 GetRenderHeight () - GetRenderHeight () * 0.2,
                                 MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT });

                SetShaderValue (plug->shaders[PBR_SHADER],
                                plug->shaderLoc[LIGHT_POS_LOC], &lightPosition,
                                SHADER_UNIFORM_VEC3);
                SetShaderValue (plug->shaders[PBR_SHADER],
                                plug->shaderLoc[VIEW_POS_LOC],
                                &plug->camera.position, SHADER_UNIFORM_VEC3);
                SetShaderValue (plug->shaders[PLANE_SHADER],
                                plug->shaderLoc[LIGHT_POS_LOC], &lightPosition,
                                SHADER_UNIFORM_VEC3);
                SetShaderValue (plug->shaders[PLANE_SHADER],
                                plug->shaderLoc[VIEW_POS_LOC],
                                &plug->camera.position, SHADER_UNIFORM_VEC3);
                for (int i = 0;
                     i < plug->no_of_buttons[INTERVAL_SETTING_SCREEN]; i++)
                    {
                        if (i < 12)
                            {
                                float padding
                                    = GetRenderHeight () / 20.0 / 12.0;
                                float height = MENU_BUTTON_HEIGHT / 1.9;
                                plug->buttons[INTERVAL_SETTING_SCREEN][i].bound
                                    = (Rectangle){
                                          .width = MENU_BUTTON_WIDTH * 0.8,
                                          .height = height,
                                          .x = GetRenderWidth () * 1 / 4.0,
                                          .y
                                          = (float)GetRenderHeight ()
                                                / (plug->no_of_buttons
                                                       [INTERVAL_SETTING_SCREEN]
                                                   + 1)
                                            - height / 2
                                            + (i * (height + padding))
                                      };
                            }
                        else
                            {
                                float height = MENU_BUTTON_HEIGHT;
                                float padding = GetRenderHeight () / 20.0;
                                plug->buttons[INTERVAL_SETTING_SCREEN][i].bound
                                    = (Rectangle){
                                          .width = MENU_BUTTON_HEIGHT * 1.2,
                                          .height = height,
                                          .x = GetRenderWidth () * 3 / 4.0,
                                          .y
                                          = (float)GetRenderHeight ()
                                                / (plug->no_of_buttons
                                                       [INTERVAL_SETTING_SCREEN]
                                                   + 1 - 12)
                                            - height / 2
                                            + ((i - 12) * (height + padding))
                                      };
                            }
                    }

                for (int i = 0;
                     i < plug->no_of_buttons[INTERVAL_SETTING_SCREEN]; i++)
                    {
                        if (i < 12)
                            {
                                if (IsMenuButtonPressed (
                                        &plug->buttons[INTERVAL_SETTING_SCREEN]
                                                      [i]))
                                    {
                                        if (plug->key != -1)
                                            {
                                                plug->buttons
                                                    [INTERVAL_SETTING_SCREEN]
                                                    [plug->key]
                                                        .image
                                                    = plug->images
                                                          [10 + plug->key];
                                            }
                                        plug->buttons[INTERVAL_SETTING_SCREEN]
                                                     [i]
                                                         .image
                                            = plug->images[10 + 12
                                                           + i]; // 10 is the
                                        // starting
                                        // position of
                                        // the button
                                        // index 12 is the octave while i is
                                        // the offset
                                        plug->key = i;
                                        break;
                                    }
                            }
                        else
                            {
                                if (IsMenuButtonPressed (
                                        &plug->buttons[INTERVAL_SETTING_SCREEN]
                                                      [i]))
                                    {
                                        if (plug->scale != -1)
                                            {
                                                plug->buttons
                                                    [INTERVAL_SETTING_SCREEN]
                                                    [plug->scale + 12]
                                                        .image
                                                    = plug->images
                                                          [10 + plug->scale
                                                           + 24];
                                            }
                                        plug->buttons[INTERVAL_SETTING_SCREEN]
                                                     [i]
                                                         .image
                                            = plug->images[34 + 2 + i - 12];
                                        plug->scale = i - 12;
                                        break;
                                    }
                            }
                    }

                BeginDrawing ();
                ClearBackground (BLACK);
                BeginMode3D (plug->camera);
                DrawPlaneModel (plug);
                DrawPiano (plug);
                EndMode3D ();
                DrawTextEx (plug->MenuFont, "Choose A Key",
                            (Vector2){ 0, GetRenderHeight () * 1 / 1.8 },
                            36 * GetRenderHeight () / 600.0, 1,
                            (Color){ 0, 255, 255, 255 });
                DrawTextEx (plug->MenuFont, "Choose A Scale",
                            (Vector2){ GetRenderWidth () * 1 / 2.0,
                                       GetRenderHeight () * 1 / 1.8 },
                            36 * GetRenderHeight () / 600.0, 1,
                            (Color){ 0, 255, 255, 255 });
                for (int i = 0;
                     i < plug->no_of_buttons[INTERVAL_SETTING_SCREEN]; i++)
                    {
                        DrawMenuButton (
                            plug->buttons[INTERVAL_SETTING_SCREEN][i]);
                    }
                if (plug->key != -1 && plug->scale != -1)
                    {
                        if (IsMenuButtonPressed (&Ok))
                            {
                                plug->former_screen = plug->current_screen;
                                plug->current_screen = FREE_SCREEN;
                                plug->screen_transition = true;
                            }
                        DrawMenuButton (Ok);
                    }
                DrawFPS (10, 10);
                EndDrawing ();

                break;
            }

        case INTERVAL_SCREEN:
            {
                if (plug->screen_transition)
                    {
                        if (plug->former_screen == INTERVAL_SETTING_SCREEN)
                            {
                                plug->camera2d.offset = (Vector2){ 0, 0 };
                                ScreenTransitionIntSettingToInt (plug);
                            }
                        else if (plug->former_screen == PAUSE_SCREEN)
                            {
                                ScreenTransitionPauseToInt (plug);
                                break;
                            }
                    };

                // interval logic
                if (plug->current_note == -1)
                    {
                        plug->current_note = GenNote (plug->key, plug->scale);
                        PlaySound (plug->sound[plug->current_note]);
                        plug->time = 0;
                    }

                bool playing
                    = !IsSoundPlaying (plug->sound[plug->current_note])
                      && !plug->player_respond;

                if (playing)
                    {

                        // check input
                        for (int i = 0; i < hmlen (plug->Key_to_note); i++)
                            {
                                KeyboardKey key = plug->Key_to_note[i].key;
                                int note = hmget (plug->Key_to_note, key)
                                           + plug->currentOctave * OCTAVE;
                                if (IsKeyPressed (key))
                                    {
                                        PlaySound (plug->sound[note]);
                                        note_pool[0] = note;
                                        plug->player_respond = true;
                                        plug->time = 0;
                                        note_color = RED;
                                        strcpy (plug->interval_feedback,
                                                "Wrong");
                                        if (note % OCTAVE
                                            == plug->current_note % OCTAVE)
                                            {
                                                strcpy (
                                                    plug->interval_feedback,
                                                    "Correct");
                                                note_color = GREEN;
                                                plug->score += 1;
                                            }
                                    }
                            }
                    }
                else
                    {
                        plug->time += GetFrameTime ();
                        if (plug->player_respond && plug->time > 2)
                            {
                                StopSound (plug->sound[note_pool[0]]);
                                if (plug->time > 3)
                                    {
                                        strcpy (plug->interval_feedback, "");
                                        plug->time = 0;
                                        plug->player_respond = false;
                                        plug->current_note
                                            = GenNote (plug->key, plug->scale);
                                        PlaySound (
                                            plug->sound[plug->current_note]);
                                    }
                            }
                        if (IsSoundPlaying (plug->sound[plug->current_note])
                            && plug->time > 2)
                            {
                                StopSound (plug->sound[plug->current_note]);
                                plug->time = 0;
                            }
                    }
                if (IsKeyPressed (KEY_RIGHT_SHIFT))
                    {
                        if (plug->currentOctave < 7)
                            plug->currentOctave++;
                    }
                if (IsKeyPressed (KEY_LEFT_SHIFT))
                    {
                        if (plug->currentOctave > 0)
                            plug->currentOctave--;
                    }

                // Update light position and view position
                SetShaderValue (plug->shaders[PBR_SHADER],
                                plug->shaderLoc[LIGHT_POS_LOC], &lightPosition,
                                SHADER_UNIFORM_VEC3);
                SetShaderValue (plug->shaders[PBR_SHADER],
                                plug->shaderLoc[VIEW_POS_LOC],
                                &plug->camera.position, SHADER_UNIFORM_VEC3);
                SetShaderValue (plug->shaders[PLANE_SHADER],
                                plug->shaderLoc[LIGHT_POS_LOC], &lightPosition,
                                SHADER_UNIFORM_VEC3);
                SetShaderValue (plug->shaders[PLANE_SHADER],
                                plug->shaderLoc[VIEW_POS_LOC],
                                &plug->camera.position, SHADER_UNIFORM_VEC3);
                AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                                    GetRenderWidth () * 3 / 4.0, 0,
                                    GetRenderHeight () / 20.0,
                                    plug->no_of_buttons[INTERVAL_SCREEN],
                                    plug->buttons[INTERVAL_SCREEN]);

                if (IsMenuButtonPressed (&plug->buttons[INTERVAL_SCREEN][0]))
                    {
                        plug->former_screen = plug->current_screen;
                        plug->current_screen = PAUSE_SCREEN;
                        plug->screen_transition = true;
                    }

                BeginDrawing ();
                ClearBackground (BROWN);
                BeginMode3D (plug->camera);

                DrawPlaneModel (plug);
                DrawPiano (plug);

                DrawModel (plug->models[LIGHT_CUBE], lightPosition, 0.5f,
                           YELLOW);
                if (note_pool[0] > -1)
                    {
                        DrawBoundingBoxAsCube (plug->keyBoxes[note_pool[0]],
                                               note_color);
                    }
                DrawBoundingBoxAsCube (plug->keyBoxes[plug->current_note],
                                       BLUE);
                /* DrawGrid(10, 1); */
                EndMode3D ();
                for (int i = 0; i < plug->no_of_buttons[INTERVAL_SCREEN]; i++)
                    DrawMenuButton (plug->buttons[INTERVAL_SCREEN][i]);
                DrawText (
                    TextFormat ("Current Key is %s", Key_to_text (plug->key)),
                    30, 30, 24, RAYWHITE);
                DrawFPS (10, 10);
                EndDrawing ();
                break;
            }

        case PAUSE_SCREEN:
            {
                if (plug->screen_transition)
                    {
                        if (plug->former_screen == INTERVAL_SCREEN)
                            {
                                ScreenTransitionIntToPause (plug);
                                break;
                            }
                        if (plug->former_screen == FREE_SCREEN)
                            {
                                ScreenTransitionFreeToPause (plug);
                                break;
                            }
                    };
                if (!IsMusicStreamPlaying (
                        plug->music[plug->current_music_index]))
                    {
                        PlayMusicStream (
                            plug->music[plug->current_music_index]);
                        read_id3v2 (
                            plug->music_file[plug->current_music_index]);
                    }
                UpdateMusicStream (plug->music[plug->current_music_index]);
                AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                                    GetRenderWidth () * 3 / 4.0, 0,
                                    GetRenderHeight () / 20.0,
                                    plug->no_of_buttons[PAUSE_SCREEN],
                                    plug->buttons[PAUSE_SCREEN]);

                if (IsMenuButtonPressed (&plug->buttons[PAUSE_SCREEN][0]))
                    {
                        plug->former_screen = plug->current_screen;
                        plug->current_screen = INTERVAL_SCREEN;
                        plug->looped_once = false;
                        plug->screen_transition = true;
                        plug->camera2d.offset = (Vector2){ 0, 0 };
                    }
                if (IsMenuButtonPressed (&plug->buttons[PAUSE_SCREEN][1]))
                    {
                    }
                if (IsMenuButtonPressed (&plug->buttons[PAUSE_SCREEN][2]))
                    {
                    }
                if (IsMenuButtonPressed (&plug->buttons[PAUSE_SCREEN][3]))
                    {
                        plug->quit = true;
                    }
                calcMenuButtonPosition (&plug->buttons[PAUSE_SCREEN][0],
                                        &plug->buttons[PAUSE_SCREEN][1],
                                        &plug->buttons[PAUSE_SCREEN][2],
                                        &plug->buttons[PAUSE_SCREEN][3], NULL);
                BeginDrawing ();
                ClearBackground (BLACK);
                DrawBackgroundImage (plug->images[1]);
                for (int i = 0; i < plug->no_of_buttons[PAUSE_SCREEN]; i++)
                    DrawMenuButton (plug->buttons[PAUSE_SCREEN][i]);
                DrawTextEx (
                    plug->MenuFont,
                    TextFormat ("Currently Playing %s - %s", Title, Artist),
                    (Vector2){ GetRenderWidth () * 1 / 100.0,
                               GetRenderHeight () * 1 / 100.0 },
                    24 * GetRenderHeight () / 600.0, 1, DARKBROWN);
                DrawFPS (10, 10);
                EndDrawing ();
                break;
            }
        default:
            break;
        }
}
void
MapPianoNotes (Plug *plug)
{

    plug->keyBoxes[GS1] = GetMeshBoundingBox (plug->models[PIANO].meshes[2]);
    plug->keyBoxes[AS1] = GetMeshBoundingBox (plug->models[PIANO].meshes[3]);
    plug->keyBoxes[FS1] = GetMeshBoundingBox (plug->models[PIANO].meshes[4]);
    plug->keyBoxes[FS2] = GetMeshBoundingBox (plug->models[PIANO].meshes[5]);
    plug->keyBoxes[GS2] = GetMeshBoundingBox (plug->models[PIANO].meshes[6]);
    plug->keyBoxes[AS2] = GetMeshBoundingBox (plug->models[PIANO].meshes[7]);
    plug->keyBoxes[FS3] = GetMeshBoundingBox (plug->models[PIANO].meshes[8]);
    plug->keyBoxes[GS3] = GetMeshBoundingBox (plug->models[PIANO].meshes[9]);
    plug->keyBoxes[AS3] = GetMeshBoundingBox (plug->models[PIANO].meshes[10]);
    plug->keyBoxes[FS4] = GetMeshBoundingBox (plug->models[PIANO].meshes[11]);
    plug->keyBoxes[GS4] = GetMeshBoundingBox (plug->models[PIANO].meshes[12]);
    plug->keyBoxes[AS4] = GetMeshBoundingBox (plug->models[PIANO].meshes[13]);
    plug->keyBoxes[FS5] = GetMeshBoundingBox (plug->models[PIANO].meshes[14]);
    plug->keyBoxes[GS5] = GetMeshBoundingBox (plug->models[PIANO].meshes[15]);
    plug->keyBoxes[AS5] = GetMeshBoundingBox (plug->models[PIANO].meshes[16]);
    plug->keyBoxes[FS6] = GetMeshBoundingBox (plug->models[PIANO].meshes[17]);
    plug->keyBoxes[GS6] = GetMeshBoundingBox (plug->models[PIANO].meshes[18]);
    plug->keyBoxes[AS6] = GetMeshBoundingBox (plug->models[PIANO].meshes[19]);
    plug->keyBoxes[FS7] = GetMeshBoundingBox (plug->models[PIANO].meshes[20]);
    plug->keyBoxes[GS7] = GetMeshBoundingBox (plug->models[PIANO].meshes[21]);
    plug->keyBoxes[AS7] = GetMeshBoundingBox (plug->models[PIANO].meshes[22]);
    plug->keyBoxes[CS1] = GetMeshBoundingBox (plug->models[PIANO].meshes[24]);
    plug->keyBoxes[DS1] = GetMeshBoundingBox (plug->models[PIANO].meshes[25]);
    plug->keyBoxes[CS2] = GetMeshBoundingBox (plug->models[PIANO].meshes[26]);
    plug->keyBoxes[DS2] = GetMeshBoundingBox (plug->models[PIANO].meshes[27]);
    plug->keyBoxes[CS3] = GetMeshBoundingBox (plug->models[PIANO].meshes[28]);
    plug->keyBoxes[DS3] = GetMeshBoundingBox (plug->models[PIANO].meshes[29]);
    plug->keyBoxes[CS4] = GetMeshBoundingBox (plug->models[PIANO].meshes[30]);
    plug->keyBoxes[DS4] = GetMeshBoundingBox (plug->models[PIANO].meshes[31]);
    plug->keyBoxes[CS5] = GetMeshBoundingBox (plug->models[PIANO].meshes[32]);
    plug->keyBoxes[DS5] = GetMeshBoundingBox (plug->models[PIANO].meshes[33]);
    plug->keyBoxes[CS6] = GetMeshBoundingBox (plug->models[PIANO].meshes[34]);

    plug->keyBoxes[DS6] = GetMeshBoundingBox (plug->models[PIANO].meshes[35]);
    plug->keyBoxes[CS7] = GetMeshBoundingBox (plug->models[PIANO].meshes[36]);
    plug->keyBoxes[DS7] = GetMeshBoundingBox (plug->models[PIANO].meshes[37]);
    plug->keyBoxes[C1] = GetMeshBoundingBox (plug->models[PIANO].meshes[40]);
    plug->keyBoxes[D1] = GetMeshBoundingBox (plug->models[PIANO].meshes[41]);
    plug->keyBoxes[E1] = GetMeshBoundingBox (plug->models[PIANO].meshes[42]);
    plug->keyBoxes[F1] = GetMeshBoundingBox (plug->models[PIANO].meshes[43]);
    plug->keyBoxes[G1] = GetMeshBoundingBox (plug->models[PIANO].meshes[44]);
    plug->keyBoxes[A1] = GetMeshBoundingBox (plug->models[PIANO].meshes[45]);
    plug->keyBoxes[B1] = GetMeshBoundingBox (plug->models[PIANO].meshes[46]);
    plug->keyBoxes[C2] = GetMeshBoundingBox (plug->models[PIANO].meshes[47]);
    plug->keyBoxes[D2] = GetMeshBoundingBox (plug->models[PIANO].meshes[48]);
    plug->keyBoxes[E2] = GetMeshBoundingBox (plug->models[PIANO].meshes[49]);
    plug->keyBoxes[F2] = GetMeshBoundingBox (plug->models[PIANO].meshes[50]);
    plug->keyBoxes[G2] = GetMeshBoundingBox (plug->models[PIANO].meshes[51]);
    plug->keyBoxes[A2] = GetMeshBoundingBox (plug->models[PIANO].meshes[52]);
    plug->keyBoxes[B2] = GetMeshBoundingBox (plug->models[PIANO].meshes[53]);
    plug->keyBoxes[C3] = GetMeshBoundingBox (plug->models[PIANO].meshes[54]);
    plug->keyBoxes[D3] = GetMeshBoundingBox (plug->models[PIANO].meshes[55]);
    plug->keyBoxes[E3] = GetMeshBoundingBox (plug->models[PIANO].meshes[56]);
    plug->keyBoxes[F3] = GetMeshBoundingBox (plug->models[PIANO].meshes[57]);
    plug->keyBoxes[G3] = GetMeshBoundingBox (plug->models[PIANO].meshes[58]);
    plug->keyBoxes[A3] = GetMeshBoundingBox (plug->models[PIANO].meshes[59]);
    plug->keyBoxes[B3] = GetMeshBoundingBox (plug->models[PIANO].meshes[60]);
    plug->keyBoxes[C4] = GetMeshBoundingBox (plug->models[PIANO].meshes[61]);

    plug->keyBoxes[D4] = GetMeshBoundingBox (plug->models[PIANO].meshes[62]);
    plug->keyBoxes[E4] = GetMeshBoundingBox (plug->models[PIANO].meshes[63]);
    plug->keyBoxes[F4] = GetMeshBoundingBox (plug->models[PIANO].meshes[64]);
    plug->keyBoxes[G4] = GetMeshBoundingBox (plug->models[PIANO].meshes[65]);
    plug->keyBoxes[A4] = GetMeshBoundingBox (plug->models[PIANO].meshes[66]);
    plug->keyBoxes[B4] = GetMeshBoundingBox (plug->models[PIANO].meshes[67]);
    plug->keyBoxes[C5] = GetMeshBoundingBox (plug->models[PIANO].meshes[68]);
    plug->keyBoxes[D5] = GetMeshBoundingBox (plug->models[PIANO].meshes[69]);
    plug->keyBoxes[E5] = GetMeshBoundingBox (plug->models[PIANO].meshes[70]);
    plug->keyBoxes[F5] = GetMeshBoundingBox (plug->models[PIANO].meshes[71]);
    plug->keyBoxes[G5] = GetMeshBoundingBox (plug->models[PIANO].meshes[72]);
    plug->keyBoxes[A5] = GetMeshBoundingBox (plug->models[PIANO].meshes[73]);
    plug->keyBoxes[B5] = GetMeshBoundingBox (plug->models[PIANO].meshes[74]);
    plug->keyBoxes[C6] = GetMeshBoundingBox (plug->models[PIANO].meshes[75]);
    plug->keyBoxes[D6] = GetMeshBoundingBox (plug->models[PIANO].meshes[76]);
    plug->keyBoxes[E6] = GetMeshBoundingBox (plug->models[PIANO].meshes[77]);
    plug->keyBoxes[F6] = GetMeshBoundingBox (plug->models[PIANO].meshes[78]);
    plug->keyBoxes[G6] = GetMeshBoundingBox (plug->models[PIANO].meshes[79]);
    plug->keyBoxes[A6] = GetMeshBoundingBox (plug->models[PIANO].meshes[80]);
    plug->keyBoxes[B6] = GetMeshBoundingBox (plug->models[PIANO].meshes[81]);
    plug->keyBoxes[C7] = GetMeshBoundingBox (plug->models[PIANO].meshes[82]);
    plug->keyBoxes[D7] = GetMeshBoundingBox (plug->models[PIANO].meshes[83]);
    plug->keyBoxes[E7] = GetMeshBoundingBox (plug->models[PIANO].meshes[84]);
    plug->keyBoxes[F7] = GetMeshBoundingBox (plug->models[PIANO].meshes[85]);
    plug->keyBoxes[G7] = GetMeshBoundingBox (plug->models[PIANO].meshes[86]);
    plug->keyBoxes[A7] = GetMeshBoundingBox (plug->models[PIANO].meshes[87]);
    plug->keyBoxes[B7] = GetMeshBoundingBox (plug->models[PIANO].meshes[88]);
    plug->keyBoxes[C8] = GetMeshBoundingBox (plug->models[PIANO].meshes[89]);
}

void
DrawBoundingBoxAsCube (BoundingBox box, Color color)
{
    // Find center
    //
    float margin = 0.001;
    box.min = (Vector3){ box.min.x - margin, box.min.y - margin,
                         box.min.z - margin };
    box.max = (Vector3){ box.max.x + margin, box.max.y + margin,
                         box.max.z + margin };
    Vector3 center
        = { (box.min.x + box.max.x) * 0.5f, (box.min.y + box.max.y) * 0.5f,
            (box.min.z + box.max.z) * 0.5f };
    // Find size (extent along each axis)
    Vector3 size = { (box.max.x - box.min.x), (box.max.y - box.min.y),
                     (box.max.z - box.min.z) };
    DrawCube (center, size.x, size.y, size.z, color);
}

void
LoadPlaneModel (Plug *plug)
{
    plug->models[PLANE] = LoadModelFromMesh (GenMeshPlane (10, 10, 1, 1));

    plug->shaders[PLANE_SHADER] = LoadShader (
        "resources/shaders/main_vs.glsl", "resources/shaders/plane_fs.glsl");

    // Texture uniform locations
    int matDiffLoc
        = GetShaderLocation (plug->shaders[PLANE_SHADER], "material.diffuse");
    int matRoughnessLoc = GetShaderLocation (plug->shaders[PLANE_SHADER],
                                             "material.roughness");
    int matNormalLoc
        = GetShaderLocation (plug->shaders[PLANE_SHADER], "material.normal");
    int matMetallicLoc
        = GetShaderLocation (plug->shaders[PLANE_SHADER], "material.metallic");
    int matAOLoc
        = GetShaderLocation (plug->shaders[PLANE_SHADER], "material.ao");
    int tileLoc = GetShaderLocation (plug->shaders[PLANE_SHADER], "tile");

    // Set texture units
    int diffuseUnit = 0;
    int roughnessUnit = 1;
    int normalUnit = 2;
    int metallicUnit = 3;
    int aoUnit = 4;
    Vector2 tile = (Vector2){ 5, 5 };
    SetShaderValue (plug->shaders[PLANE_SHADER], matDiffLoc, &diffuseUnit,
                    SHADER_UNIFORM_INT);
    SetShaderValue (plug->shaders[PLANE_SHADER], matRoughnessLoc,
                    &roughnessUnit, SHADER_UNIFORM_INT);
    SetShaderValue (plug->shaders[PLANE_SHADER], matMetallicLoc, &metallicUnit,
                    SHADER_UNIFORM_INT);
    SetShaderValue (plug->shaders[PLANE_SHADER], matAOLoc, &aoUnit,
                    SHADER_UNIFORM_INT);
    SetShaderValue (plug->shaders[PLANE_SHADER], matNormalLoc, &normalUnit,
                    SHADER_UNIFORM_INT);
    SetShaderValue (plug->shaders[PLANE_SHADER], tileLoc, &tile,
                    SHADER_UNIFORM_VEC2);

    printf ("Number of materials: %d\n", plug->models[PLANE].materialCount);
    printf ("Number of meshes: %d\n", plug->models[PLANE].meshCount);

    // Set light uniforms
    SetShaderValue (plug->shaders[PLANE_SHADER],
                    plug->shaderLoc[LIGHT_POS_LOC], &lightPosition,
                    SHADER_UNIFORM_VEC3);
    SetShaderValue (plug->shaders[PLANE_SHADER],
                    plug->shaderLoc[LIGHT_AMB_LOC], &lightAmbient,
                    SHADER_UNIFORM_VEC3);
    SetShaderValue (plug->shaders[PLANE_SHADER],
                    plug->shaderLoc[LIGHT_DIFF_LOC], &lightDiffuse,
                    SHADER_UNIFORM_VEC3);
    SetShaderValue (plug->shaders[PLANE_SHADER],
                    plug->shaderLoc[LIGHT_SPEC_LOC], &lightSpecular,
                    SHADER_UNIFORM_VEC3);
    SetShaderValue (plug->shaders[PLANE_SHADER],
                    plug->shaderLoc[LIGHT_CONST_LOC], &lightConstant,
                    SHADER_UNIFORM_FLOAT);
    SetShaderValue (plug->shaders[PLANE_SHADER],
                    plug->shaderLoc[LIGHT_LINEAR_LOC], &lightLinear,
                    SHADER_UNIFORM_FLOAT);
    SetShaderValue (plug->shaders[PLANE_SHADER],
                    plug->shaderLoc[LIGHT_QUADRATIC_LOC], &lightQuadratic,
                    SHADER_UNIFORM_FLOAT);

    // Prepare textures for both materials
    // Material 0: Piano body

    // Extract textures from materials if they exist
    plug->maps[PLANE_ALBEDO]
        = LoadTexture ("resources/textures/stoneBrick/COL.jpg");
    plug->maps[PLANE_ROUGHNESS]
        = LoadTexture ("resources/textures/stoneBrick/ROUGHNESS.jpg");
    plug->maps[PLANE_METALLIC]
        = LoadTexture ("resources/textures/stoneBrick/METALLIC.jpg");
    plug->maps[PLANE_AO]
        = LoadTexture ("resources/textures/stoneBrick/AO.jpg");
    plug->maps[PLANE_NORMAL]
        = LoadTexture ("resources/textures/stoneBrick/NORMAL.png");
    plug->models[PLANE].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture
        = plug->maps[PLANE_ALBEDO];
    plug->models[PLANE].materials[0].maps[MATERIAL_MAP_ROUGHNESS].texture
        = plug->maps[PLANE_ROUGHNESS];
    plug->models[PLANE].materials[0].maps[MATERIAL_MAP_METALNESS].texture
        = plug->maps[PLANE_METALLIC];
    plug->models[PLANE].materials[0].maps[MATERIAL_MAP_OCCLUSION].texture
        = plug->maps[PLANE_AO];
    plug->models[PLANE].materials[0].maps[MATERIAL_MAP_NORMAL].texture
        = plug->maps[PLANE_NORMAL];

    // Assign shader to material
    plug->models[PLANE].materials[0].shader = plug->shaders[PLANE_SHADER];
}

void
LoadPianoModel (Plug *plug)
{
    plug->models[PIANO] = LoadModel ("resources/models/edited.gltf");

    plug->shaders[PBR_SHADER] = LoadShader ("resources/shaders/main_vs.glsl",
                                            "resources/shaders/main_fs.glsl");
    plug->shaders[LIGHT_SHADER] = LoadShader ("resources/shaders/main_vs.glsl",
                                              "resources/shaders/light.glsl");

    // Load models
    plug->models[LIGHT_CUBE]
        = LoadModelFromMesh (GenMeshCube (0.5f, 0.5f, 0.5f));

    // Get shader locations
    plug->shaderLoc[LIGHT_POS_LOC]
        = GetShaderLocation (plug->shaders[PBR_SHADER], "light.position");
    plug->shaderLoc[LIGHT_AMB_LOC]
        = GetShaderLocation (plug->shaders[PBR_SHADER], "light.ambient");
    plug->shaderLoc[LIGHT_DIFF_LOC]
        = GetShaderLocation (plug->shaders[PBR_SHADER], "light.diffuse");
    plug->shaderLoc[LIGHT_SPEC_LOC]
        = GetShaderLocation (plug->shaders[PBR_SHADER], "light.specular");
    plug->shaderLoc[LIGHT_CONST_LOC]
        = GetShaderLocation (plug->shaders[PBR_SHADER], "light.constant");
    plug->shaderLoc[LIGHT_LINEAR_LOC]
        = GetShaderLocation (plug->shaders[PBR_SHADER], "light.linear");
    plug->shaderLoc[LIGHT_QUADRATIC_LOC]
        = GetShaderLocation (plug->shaders[PBR_SHADER], "light.quadratic");

    plug->shaderLoc[VIEW_POS_LOC]
        = GetShaderLocation (plug->shaders[PBR_SHADER], "viewPos");

    // Texture uniform locations
    int matDiffLoc
        = GetShaderLocation (plug->shaders[PBR_SHADER], "material.diffuse");
    int matMetallicRoughnessLoc = GetShaderLocation (
        plug->shaders[PBR_SHADER], "material.metallicRoughness");
    int matNormalLoc
        = GetShaderLocation (plug->shaders[PBR_SHADER], "material.normal");

    // Set texture units
    int diffuseUnit = 0;
    int metallicRoughnessUnit = 1;
    int normalUnit = 2;
    SetShaderValue (plug->shaders[PBR_SHADER], matDiffLoc, &diffuseUnit,
                    SHADER_UNIFORM_INT);
    SetShaderValue (plug->shaders[PBR_SHADER], matMetallicRoughnessLoc,
                    &metallicRoughnessUnit, SHADER_UNIFORM_INT);
    SetShaderValue (plug->shaders[PBR_SHADER], matNormalLoc, &normalUnit,
                    SHADER_UNIFORM_INT);

    // Check how many materials we have
    printf ("Number of materials: %d\n", plug->models[PIANO].materialCount);
    printf ("Number of meshes: %d\n", plug->models[PIANO].meshCount);

    // Set up light properties

    // Set light uniforms
    SetShaderValue (plug->shaders[PBR_SHADER], plug->shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (plug->shaders[PBR_SHADER], plug->shaderLoc[LIGHT_AMB_LOC],
                    &lightAmbient, SHADER_UNIFORM_VEC3);
    SetShaderValue (plug->shaders[PBR_SHADER], plug->shaderLoc[LIGHT_DIFF_LOC],
                    &lightDiffuse, SHADER_UNIFORM_VEC3);
    SetShaderValue (plug->shaders[PBR_SHADER], plug->shaderLoc[LIGHT_SPEC_LOC],
                    &lightSpecular, SHADER_UNIFORM_VEC3);
    SetShaderValue (plug->shaders[PBR_SHADER],
                    plug->shaderLoc[LIGHT_CONST_LOC], &lightConstant,
                    SHADER_UNIFORM_FLOAT);
    SetShaderValue (plug->shaders[PBR_SHADER],
                    plug->shaderLoc[LIGHT_LINEAR_LOC], &lightLinear,
                    SHADER_UNIFORM_FLOAT);
    SetShaderValue (plug->shaders[PBR_SHADER],
                    plug->shaderLoc[LIGHT_QUADRATIC_LOC], &lightQuadratic,
                    SHADER_UNIFORM_FLOAT);

    // Prepare textures for both materials
    // Material 0: Piano body

    // Extract textures from materials if they exist
    if (plug->models[PIANO].materialCount > 1)
        {
            // First material (index 0)
            plug->maps[PIANO_ALBEDO] = plug->models[PIANO]
                                           .materials[1]
                                           .maps[MATERIAL_MAP_DIFFUSE]
                                           .texture;
            plug->maps[PIANO_METALLIC_ROUGHNESS]
                = plug->models[PIANO]
                      .materials[1]
                      .maps[MATERIAL_MAP_METALNESS]
                      .texture;
            plug->maps[PIANO_NORMAL] = plug->models[PIANO]
                                           .materials[1]
                                           .maps[MATERIAL_MAP_NORMAL]
                                           .texture;

            // Assign shader to material
            plug->models[PIANO].materials[1].shader
                = plug->shaders[PBR_SHADER];
        }

    if (plug->models[PIANO].materialCount > 2)
        {
            // Second material (index 1)
            plug->maps[BENCH_ALBEDO] = plug->models[PIANO]
                                           .materials[2]
                                           .maps[MATERIAL_MAP_DIFFUSE]
                                           .texture;
            plug->maps[BENCH_METALLIC_ROUGHNESS]
                = plug->models[PIANO]
                      .materials[2]
                      .maps[MATERIAL_MAP_METALNESS]
                      .texture;
            plug->maps[BENCH_NORMAL] = plug->models[PIANO]
                                           .materials[2]
                                           .maps[MATERIAL_MAP_NORMAL]
                                           .texture;

            // Assign shader to material
            plug->models[PIANO].materials[2].shader
                = plug->shaders[PBR_SHADER];
        }

    // Create default textures if needed
    if (plug->maps[PIANO_ALBEDO].id == 0)
        {
            Image img = GenImageColor (1, 1, WHITE);
            plug->maps[PIANO_ALBEDO] = LoadTextureFromImage (img);
            UnloadImage (img);
        }
    if (plug->maps[PIANO_METALLIC_ROUGHNESS].id == 0)
        {
            Image img = GenImageColor (
                1, 1,
                (Color){ 0, 128, 255, 255 }); // Default roughness and metallic
            plug->maps[PIANO_METALLIC_ROUGHNESS] = LoadTextureFromImage (img);
            UnloadImage (img);
        }
    if (plug->maps[PIANO_NORMAL].id == 0)
        {
            Image img = GenImageColor (
                1, 1,
                (Color){ 128, 128, 255, 255 }); // Default normal (pointing up)
            plug->maps[PIANO_NORMAL] = LoadTextureFromImage (img);
            UnloadImage (img);
        }
}

void
DrawPiano (Plug *plug)
{
    for (int i = 0; i < plug->models[PIANO].meshCount; i++)
        {
            // Get material index for this mesh
            int materialIndex = 0;
            if (plug->models[PIANO].meshMaterial != NULL)
                {
                    materialIndex = plug->models[PIANO].meshMaterial[i];
                }

            // Bind appropriate textures based on material
            if (materialIndex == 1)
                {
                    // Piano material
                    rlActiveTextureSlot (0);
                    rlEnableTexture (plug->maps[PIANO_ALBEDO].id);
                    rlActiveTextureSlot (1);
                    rlEnableTexture (plug->maps[PIANO_METALLIC_ROUGHNESS].id);
                    rlActiveTextureSlot (2);
                    rlEnableTexture (plug->maps[PIANO_NORMAL].id);
                }
            else if (materialIndex == 2)
                {
                    // Bench material
                    rlActiveTextureSlot (0);
                    rlEnableTexture (plug->maps[BENCH_ALBEDO].id);
                    rlActiveTextureSlot (1);
                    rlEnableTexture (plug->maps[BENCH_METALLIC_ROUGHNESS].id);
                    rlActiveTextureSlot (2);
                    rlEnableTexture (plug->maps[BENCH_NORMAL].id);
                }

            // Draw mesh with the PBR shader
            if (materialIndex < plug->models[PIANO].materialCount)
                {
                    DrawMesh (plug->models[PIANO].meshes[i],
                              plug->models[PIANO].materials[materialIndex],
                              (Matrix){ 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0,
                                        0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0,
                                        0.1 }); // MatrixIdentity ());
                }
        }
}

void
DrawPlaneModel (Plug *plug)
{
    rlActiveTextureSlot (0);
    rlEnableTexture (plug->maps[PLANE_ALBEDO].id);
    rlActiveTextureSlot (1);
    rlEnableTexture (plug->maps[PLANE_ROUGHNESS].id);
    rlActiveTextureSlot (2);
    rlEnableTexture (plug->maps[PLANE_NORMAL].id);
    rlActiveTextureSlot (3);
    rlEnableTexture (plug->maps[PLANE_METALLIC].id);
    rlActiveTextureSlot (4);
    rlEnableTexture (plug->maps[PLANE_AO].id);
    DrawMesh (plug->models[PLANE].meshes[0], plug->models[PLANE].materials[0],
              MatrixIdentity ());
}

void
LoadSoundInstrument (Plug *plug)
{
    // Octave 1
    plug->sound[C1] = LoadSound ("resources/instrument/piano/1-c.mp3");
    plug->sound[CS1] = LoadSound ("resources/instrument/piano/1-cs.mp3");
    plug->sound[D1] = LoadSound ("resources/instrument/piano/1-d.mp3");
    plug->sound[DS1] = LoadSound ("resources/instrument/piano/1-ds.mp3");
    plug->sound[E1] = LoadSound ("resources/instrument/piano/1-e.mp3");
    plug->sound[F1] = LoadSound ("resources/instrument/piano/1-f.mp3");
    plug->sound[FS1] = LoadSound ("resources/instrument/piano/1-fs.mp3");
    plug->sound[G1] = LoadSound ("resources/instrument/piano/1-g.mp3");
    plug->sound[GS1] = LoadSound ("resources/instrument/piano/1-gs.mp3");
    plug->sound[A1] = LoadSound ("resources/instrument/piano/1-a.mp3");
    plug->sound[AS1] = LoadSound ("resources/instrument/piano/1-as.mp3");
    plug->sound[B1] = LoadSound ("resources/instrument/piano/1-b.mp3");

    // Octave 2
    plug->sound[C2] = LoadSound ("resources/instrument/piano/2-c.mp3");
    plug->sound[CS2] = LoadSound ("resources/instrument/piano/2-cs.mp3");
    plug->sound[D2] = LoadSound ("resources/instrument/piano/2-d.mp3");
    plug->sound[DS2] = LoadSound ("resources/instrument/piano/2-ds.mp3");
    plug->sound[E2] = LoadSound ("resources/instrument/piano/2-e.mp3");
    plug->sound[F2] = LoadSound ("resources/instrument/piano/2-f.mp3");
    plug->sound[FS2] = LoadSound ("resources/instrument/piano/2-fs.mp3");
    plug->sound[G2] = LoadSound ("resources/instrument/piano/2-g.mp3");
    plug->sound[GS2] = LoadSound ("resources/instrument/piano/2-gs.mp3");
    plug->sound[A2] = LoadSound ("resources/instrument/piano/2-a.mp3");
    plug->sound[AS2] = LoadSound ("resources/instrument/piano/2-as.mp3");
    plug->sound[B2] = LoadSound ("resources/instrument/piano/2-b.mp3");

    // Octave 3
    plug->sound[C3] = LoadSound ("resources/instrument/piano/3-c.mp3");
    plug->sound[CS3] = LoadSound ("resources/instrument/piano/3-cs.mp3");
    plug->sound[D3] = LoadSound ("resources/instrument/piano/3-d.mp3");
    plug->sound[DS3] = LoadSound ("resources/instrument/piano/3-ds.mp3");
    plug->sound[E3] = LoadSound ("resources/instrument/piano/3-e.mp3");
    plug->sound[F3] = LoadSound ("resources/instrument/piano/3-f.mp3");
    plug->sound[FS3] = LoadSound ("resources/instrument/piano/3-fs.mp3");
    plug->sound[G3] = LoadSound ("resources/instrument/piano/3-g.mp3");
    plug->sound[GS3] = LoadSound ("resources/instrument/piano/3-gs.mp3");
    plug->sound[A3] = LoadSound ("resources/instrument/piano/3-a.mp3");
    plug->sound[AS3] = LoadSound ("resources/instrument/piano/3-as.mp3");
    plug->sound[B3] = LoadSound ("resources/instrument/piano/3-b.mp3");

    // Octave 4
    plug->sound[C4] = LoadSound ("resources/instrument/piano/4-c.mp3");
    plug->sound[CS4] = LoadSound ("resources/instrument/piano/4-cs.mp3");
    plug->sound[D4] = LoadSound ("resources/instrument/piano/4-d.mp3");
    plug->sound[DS4] = LoadSound ("resources/instrument/piano/4-ds.mp3");
    plug->sound[E4] = LoadSound ("resources/instrument/piano/4-e.mp3");
    plug->sound[F4] = LoadSound ("resources/instrument/piano/4-f.mp3");
    plug->sound[FS4] = LoadSound ("resources/instrument/piano/4-fs.mp3");
    plug->sound[G4] = LoadSound ("resources/instrument/piano/4-g.mp3");
    plug->sound[GS4] = LoadSound ("resources/instrument/piano/4-gs.mp3");
    plug->sound[A4] = LoadSound ("resources/instrument/piano/4-a.mp3");
    plug->sound[AS4] = LoadSound ("resources/instrument/piano/4-as.mp3");
    plug->sound[B4] = LoadSound ("resources/instrument/piano/4-b.mp3");

    // Octave 5
    plug->sound[C5] = LoadSound ("resources/instrument/piano/5-c.mp3");
    plug->sound[CS5] = LoadSound ("resources/instrument/piano/5-cs.mp3");
    plug->sound[D5] = LoadSound ("resources/instrument/piano/5-d.mp3");
    plug->sound[DS5] = LoadSound ("resources/instrument/piano/5-ds.mp3");
    plug->sound[E5] = LoadSound ("resources/instrument/piano/5-e.mp3");
    plug->sound[F5] = LoadSound ("resources/instrument/piano/5-f.mp3");
    plug->sound[FS5] = LoadSound ("resources/instrument/piano/5-fs.mp3");
    plug->sound[G5] = LoadSound ("resources/instrument/piano/5-g.mp3");
    plug->sound[GS5] = LoadSound ("resources/instrument/piano/5-gs.mp3");
    plug->sound[A5] = LoadSound ("resources/instrument/piano/5-a.mp3");
    plug->sound[AS5] = LoadSound ("resources/instrument/piano/5-as.mp3");
    plug->sound[B5] = LoadSound ("resources/instrument/piano/5-b.mp3");

    // Octave 6
    plug->sound[C6] = LoadSound ("resources/instrument/piano/6-c.mp3");
    plug->sound[CS6] = LoadSound ("resources/instrument/piano/6-cs.mp3");
    plug->sound[D6] = LoadSound ("resources/instrument/piano/6-d.mp3");
    plug->sound[DS6] = LoadSound ("resources/instrument/piano/6-ds.mp3");
    plug->sound[E6] = LoadSound ("resources/instrument/piano/6-e.mp3");
    plug->sound[F6] = LoadSound ("resources/instrument/piano/6-f.mp3");
    plug->sound[FS6] = LoadSound ("resources/instrument/piano/6-fs.mp3");
    plug->sound[G6] = LoadSound ("resources/instrument/piano/6-g.mp3");
    plug->sound[GS6] = LoadSound ("resources/instrument/piano/6-gs.mp3");
    plug->sound[A6] = LoadSound ("resources/instrument/piano/6-a.mp3");
    plug->sound[AS6] = LoadSound ("resources/instrument/piano/6-as.mp3");
    plug->sound[B6] = LoadSound ("resources/instrument/piano/6-b.mp3");

    // Octave 7
    plug->sound[C7] = LoadSound ("resources/instrument/piano/7-c.mp3");
    plug->sound[CS7] = LoadSound ("resources/instrument/piano/7-cs.mp3");
    plug->sound[D7] = LoadSound ("resources/instrument/piano/7-d.mp3");
    plug->sound[DS7] = LoadSound ("resources/instrument/piano/7-ds.mp3");
    plug->sound[E7] = LoadSound ("resources/instrument/piano/7-e.mp3");
    plug->sound[F7] = LoadSound ("resources/instrument/piano/7-f.mp3");
    plug->sound[FS7] = LoadSound ("resources/instrument/piano/7-fs.mp3");
    plug->sound[G7] = LoadSound ("resources/instrument/piano/7-g.mp3");
    plug->sound[GS7] = LoadSound ("resources/instrument/piano/7-gs.mp3");
    plug->sound[A7] = LoadSound ("resources/instrument/piano/7-a.mp3");
    plug->sound[AS7] = LoadSound ("resources/instrument/piano/7-as.mp3");
    plug->sound[B7] = LoadSound ("resources/instrument/piano/7-b.mp3");

    // Top C (octave 8)
    plug->sound[C8] = LoadSound ("resources/instrument/piano/8-c.mp3");
}
void
LoadResources (Plug *plug)
{
    plug->images[0] = LoadTexture ("resources/images/background1.jpg");
    plug->images[1] = LoadTexture ("resources/images/background2.jpg");
    // home screen buttons
    plug->images[2] = LoadTexture ("resources/images/button1.png");
    plug->images[3] = LoadTexture ("resources/images/button2.png");
    plug->images[4] = LoadTexture ("resources/images/button3.png");
    plug->images[5] = LoadTexture ("resources/images/button4.png");

    // pause screen buttons
    plug->images[6] = LoadTexture ("resources/images/button5.png");
    plug->images[7] = LoadTexture ("resources/images/button6.png");
    plug->images[8] = LoadTexture ("resources/images/button7.png");
    plug->images[9] = LoadTexture ("resources/images/button8.png");

    // interval settin
    // white button C - B
    plug->images[10] = LoadTexture ("resources/images/button9.png");
    plug->images[11] = LoadTexture ("resources/images/button10.png");
    plug->images[12] = LoadTexture ("resources/images/button11.png");
    plug->images[13] = LoadTexture ("resources/images/button12.png");
    plug->images[14] = LoadTexture ("resources/images/button13.png");
    plug->images[15] = LoadTexture ("resources/images/button14.png");
    plug->images[16] = LoadTexture ("resources/images/button15.png");
    plug->images[17] = LoadTexture ("resources/images/button16.png");
    plug->images[18] = LoadTexture ("resources/images/button17.png");
    plug->images[19] = LoadTexture ("resources/images/button18.png");
    plug->images[20] = LoadTexture ("resources/images/button19.png");
    plug->images[21] = LoadTexture ("resources/images/button20.png");

    // green button C - B
    plug->images[22] = LoadTexture ("resources/images/button21.png");
    plug->images[23] = LoadTexture ("resources/images/button22.png");
    plug->images[24] = LoadTexture ("resources/images/button23.png");
    plug->images[25] = LoadTexture ("resources/images/button24.png");
    plug->images[26] = LoadTexture ("resources/images/button25.png");
    plug->images[27] = LoadTexture ("resources/images/button26.png");
    plug->images[28] = LoadTexture ("resources/images/button27.png");
    plug->images[29] = LoadTexture ("resources/images/button28.png");
    plug->images[30] = LoadTexture ("resources/images/button29.png");
    plug->images[31] = LoadTexture ("resources/images/button30.png");
    plug->images[32] = LoadTexture ("resources/images/button31.png");
    plug->images[33] = LoadTexture ("resources/images/button32.png");

    // scale
    plug->images[34] = LoadTexture ("resources/images/button33.png");
    plug->images[35] = LoadTexture ("resources/images/button34.png");

    plug->music_file[0] = "resources/music/4AM.mp3";
    plug->music_file[1] = "resources/music/Bohemian.mp3";
    plug->music_file[2] = "resources/music/Duvet.mp3";
    plug->music_file[3] = "resources/music/Just A Joke.mp3";
    plug->music_file[4] = "resources/music/Machi No Dorufin.mp3";

    for (int i = 0; i < NO_OF_MUSIC; i++)
        {
            plug->music[i] = LoadMusicStream (plug->music_file[i]);
        }
}

void
LoadUi (Plug *plug)
{
    /***************** HOME SCREEN UI ******************/

    plug->buttons[HOME_SCREEN][0]
        = CreateMenuButton (plug->images[2], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 4.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[HOME_SCREEN][1]
        = CreateMenuButton (plug->images[3], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[HOME_SCREEN][2]
        = CreateMenuButton (plug->images[4], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[HOME_SCREEN][3]
        = CreateMenuButton (plug->images[5], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    for (plug->no_of_buttons[HOME_SCREEN] = 0;
         plug->buttons[HOME_SCREEN][plug->no_of_buttons[HOME_SCREEN]].is
         == true;
         plug->no_of_buttons[HOME_SCREEN]++)
        ;

    /***************************************************/

    /***************** INTERVAL SCREEN UI ******************/
    plug->buttons[INTERVAL_SCREEN][0]
        = CreateMenuButton (plug->images[12], "Pause", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    for (plug->no_of_buttons[INTERVAL_SCREEN] = 0;
         plug->buttons[INTERVAL_SCREEN][plug->no_of_buttons[INTERVAL_SCREEN]]
             .is
         == true;
         plug->no_of_buttons[INTERVAL_SCREEN]++)
        ;

    /***************************************************/

    /***************** FREE SCREEN UI ******************/
    plug->buttons[FREE_SCREEN][0] = plug->buttons[INTERVAL_SCREEN][0];
    for (plug->no_of_buttons[FREE_SCREEN] = 0;
         plug->buttons[FREE_SCREEN][plug->no_of_buttons[FREE_SCREEN]].is
         == true;
         plug->no_of_buttons[FREE_SCREEN]++)
        ;

    /***************************************************/

    /***************** PAUSE SCREEN UI ******************/

    plug->buttons[PAUSE_SCREEN][0]
        = CreateMenuButton (plug->images[6], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 4.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[PAUSE_SCREEN][1]
        = CreateMenuButton (plug->images[7], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[PAUSE_SCREEN][2]
        = CreateMenuButton (plug->images[8], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[PAUSE_SCREEN][3]
        = CreateMenuButton (plug->images[9], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    for (plug->no_of_buttons[PAUSE_SCREEN] = 0;
         plug->buttons[PAUSE_SCREEN][plug->no_of_buttons[PAUSE_SCREEN]].is
         == true;
         plug->no_of_buttons[PAUSE_SCREEN]++)
        ;
    /***************************************************/

    /***************** INTERVAL SETTING SCREEN UI ******************/

    // key button
    plug->buttons[INTERVAL_SETTING_SCREEN][0]
        = CreateMenuButton (plug->images[10], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[INTERVAL_SETTING_SCREEN][1]
        = CreateMenuButton (plug->images[11], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[INTERVAL_SETTING_SCREEN][2]
        = CreateMenuButton (plug->images[12], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[INTERVAL_SETTING_SCREEN][3]
        = CreateMenuButton (plug->images[13], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[INTERVAL_SETTING_SCREEN][4]
        = CreateMenuButton (plug->images[14], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[INTERVAL_SETTING_SCREEN][5]
        = CreateMenuButton (plug->images[15], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[INTERVAL_SETTING_SCREEN][6]
        = CreateMenuButton (plug->images[16], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[INTERVAL_SETTING_SCREEN][7]
        = CreateMenuButton (plug->images[17], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[INTERVAL_SETTING_SCREEN][8]
        = CreateMenuButton (plug->images[18], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[INTERVAL_SETTING_SCREEN][9]
        = CreateMenuButton (plug->images[19], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[INTERVAL_SETTING_SCREEN][10]
        = CreateMenuButton (plug->images[20], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[INTERVAL_SETTING_SCREEN][11]
        = CreateMenuButton (plug->images[21], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });

    // scale button
    plug->buttons[INTERVAL_SETTING_SCREEN][12]
        = CreateMenuButton (plug->images[34], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    plug->buttons[INTERVAL_SETTING_SCREEN][13]
        = CreateMenuButton (plug->images[35], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });

    for (plug->no_of_buttons[INTERVAL_SETTING_SCREEN] = 0;
         plug->buttons[INTERVAL_SETTING_SCREEN]
                      [plug->no_of_buttons[INTERVAL_SETTING_SCREEN]]
                          .is
         == true;
         plug->no_of_buttons[INTERVAL_SETTING_SCREEN]++)
        ;

    /***************************************************/
}

void
CheckKeyPress (Plug *plug)
{
    int key;
    while ((key = GetKeyPressed ()) != 0)
        {
            if (hmget (plug->Key_to_note, key) != -1)
                {
                    int note = hmget (plug->Key_to_note, key)
                               + plug->currentOctave * OCTAVE;
                    for (int i = 0; i < POOL_SIZE; i++)
                        {
                            if (note_pool[i] == -1)
                                {
                                    note_pool[i] = note;
                                    PlaySound (plug->sound[note]);
                                    break;
                                }
                        }
                    continue;
                }
            if (key == KEY_RIGHT_SHIFT)
                {
                    if (plug->currentOctave >= 7)
                        continue;
                    plug->currentOctave++;
                    continue;
                }
            if (key == KEY_LEFT_SHIFT)
                {
                    if (plug->currentOctave <= 0)
                        continue;
                    plug->currentOctave--;
                    continue;
                }
        }
    for (int i = 0; i < hmlen (plug->Key_to_note); i++)
        {
            KeyboardKey key = plug->Key_to_note[i].key;
            if (IsKeyReleased (key))
                {
                    int note = hmget (plug->Key_to_note, key)
                               + plug->currentOctave * OCTAVE;
                    for (int i = 0; i < POOL_SIZE; i++)
                        {
                            if (note_pool[i] == note)
                                {
                                    StopSound (plug->sound[note]);
                                    note_pool[i] = -1;
                                }
                        }
                }
        }
}

void
ScreenTransitionHomeToIntSetting (Plug *plug)
{
    // Spring bounce animation
    float stiffness = 100;
    float damping = 10;
    float displacement = GetRenderWidth () - offset;
    float springForce = stiffness * displacement;
    float dampingForce = -damping * velocity;
    float force = springForce + dampingForce;
    float speed = fabsf (velocity);
    float tolerance = 1;
    velocity += force * GetFrameTime ();
    offset += velocity * GetFrameTime ();
    displacement = fabsf (displacement);
    if (displacement < tolerance && speed < tolerance)
        {
            offset = GetRenderWidth ();
            plug->screen_transition = false;
            plug->camera2d.offset.x = -offset;
            offset = 0;
            velocity = 0;
        }
    else
        {
            plug->camera2d.offset.x = -offset;
        }
    BeginDrawing ();
    ClearBackground (BLACK);
    BeginMode2D (plug->camera2d);
    for (int i = 0; i < plug->no_of_buttons[INTERVAL_SETTING_SCREEN]; i++)
        {
            if (i < 12)
                {
                    float padding = GetRenderHeight () / 20.0 / 12.0;
                    float height = MENU_BUTTON_HEIGHT / 1.9;
                    plug->buttons[INTERVAL_SETTING_SCREEN][i].bound
                        = (Rectangle){ .width = MENU_BUTTON_WIDTH * 0.8,
                                       .height = height,
                                       .x = GetRenderWidth () * 1 / 4.0
                                            + GetRenderWidth (),
                                       .y
                                       = (float)GetRenderHeight ()
                                             / (plug->no_of_buttons
                                                    [INTERVAL_SETTING_SCREEN]
                                                + 1)
                                         - height / 2
                                         + (i * (height + padding)) };
                }
            else
                {
                    float height = MENU_BUTTON_HEIGHT;
                    float padding = GetRenderHeight () / 20.0;
                    plug->buttons[INTERVAL_SETTING_SCREEN][i].bound
                        = (Rectangle){ .width = MENU_BUTTON_HEIGHT * 1.2,
                                       .height = height,
                                       .x = GetRenderWidth () * 3 / 4.0
                                            + GetRenderWidth (),
                                       .y
                                       = (float)GetRenderHeight ()
                                             / (plug->no_of_buttons
                                                    [INTERVAL_SETTING_SCREEN]
                                                + 1 - 12)
                                         - height / 2
                                         + ((i - 12) * (height + padding)) };
                }
        }

    DrawBackgroundImage (plug->images[0]);
    for (int i = 0; i < plug->no_of_buttons[HOME_SCREEN]; i++)
        DrawMenuButton (plug->buttons[HOME_SCREEN][i]);
    DrawTextEx (plug->MenuFont,
                TextFormat ("Currently Playing %s - %s", Title, Artist),
                (Vector2){ GetRenderWidth () * 1 / 100.0,
                           GetRenderHeight () * 1 / 100.0 },
                24 * GetRenderHeight () / 600.0, 1, DARKGREEN);

    DrawTextEx (
        plug->MenuFont, "Choose A Key",
        (Vector2){ 0 + GetRenderWidth (), GetRenderHeight () * 1 / 1.8 },
        36 * GetRenderHeight () / 600.0, 1, (Color){ 0, 255, 255, 255 });
    DrawTextEx (plug->MenuFont, "Choose A Scale",
                (Vector2){ GetRenderWidth () * 1 / 2.0 + GetRenderWidth (),
                           GetRenderHeight () * 1 / 1.8 },
                36 * GetRenderHeight () / 600.0, 1,
                (Color){ 0, 255, 255, 255 });
    for (int i = 0; i < plug->no_of_buttons[INTERVAL_SETTING_SCREEN]; i++)
        {
            DrawMenuButton (plug->buttons[INTERVAL_SETTING_SCREEN][i]);
        }
    EndMode2D ();
    EndDrawing ();
}

void
ScreenTransitionIntSettingToInt (Plug *plug)
{
    Vector3 target = { 0, 2, 2 };
    plug->camera.position.x = Lerp (plug->camera.position.x, target.x, 0.04);
    plug->camera.position.y = Lerp (plug->camera.position.y, target.y, 0.04);
    plug->camera.position.z = Lerp (plug->camera.position.z, target.z, 0.04);
    float tolerance = 0.1;
    target.x += tolerance;
    target.y += tolerance;
    target.z += tolerance;
    if (plug->camera.position.x <= target.x
        && plug->camera.position.y <= target.y
        && plug->camera.position.z <= target.z)
        {
            plug->screen_transition = false;
            printf ("done\n");
        }
}

void
ScreenTransitionIntToPause (Plug *plug)
{
    if (!IsMusicStreamPlaying (plug->music[plug->current_music_index]))
        {
            PlayMusicStream (plug->music[plug->current_music_index]);
            read_id3v2 (plug->music_file[plug->current_music_index]);
        }
    UpdateMusicStream (plug->music[plug->current_music_index]);

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
            plug->screen_transition = false;
            plug->camera2d.offset.y = -offset;
            offset = 0;
            velocity = 0;
        }
    else
        {
            plug->camera2d.offset.y = -offset;
        }

    BeginDrawing ();
    ClearBackground (BROWN);

    BeginMode3D (plug->camera);

    DrawPlaneModel (plug);
    DrawPiano (plug);

    DrawModel (plug->models[LIGHT_CUBE], lightPosition, 0.5f, YELLOW);
    for (int i = 0; i < POOL_SIZE; i++)
        {
            if (note_pool[i] > -1)
                {
                    DrawBoundingBoxAsCube (plug->keyBoxes[note_pool[i]], BLUE);
                }
        }

    EndMode3D ();
    BeginMode2D (plug->camera2d);
    for (int i = 0; i < plug->no_of_buttons[INTERVAL_SCREEN]; i++)
        DrawMenuButton (plug->buttons[INTERVAL_SCREEN][i]);

    AlignScreenButtons (
        MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH, GetRenderWidth () * 3 / 4.0,
        GetRenderHeight (), GetRenderHeight () / 20.0,
        plug->no_of_buttons[PAUSE_SCREEN], plug->buttons[PAUSE_SCREEN]);

    Texture2D image = plug->images[1];
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
    for (int i = 0; i < plug->no_of_buttons[PAUSE_SCREEN]; i++)
        DrawMenuButton (plug->buttons[PAUSE_SCREEN][i]);
    DrawTextEx (
        plug->MenuFont,
        TextFormat ("Currently Playing %s - %s", Title, Artist),
        (Vector2){ GetRenderWidth () * 1 / 100.0,
                   GetRenderHeight () + GetRenderHeight () * 1 / 100.0 },
        24 * GetRenderHeight () / 600.0, 1, DARKBROWN);

    DrawFPS (10, 10);
    EndMode2D ();
    EndDrawing ();
}

void
ScreenTransitionFreeToPause (Plug *plug)
{
    if (!IsMusicStreamPlaying (plug->music[plug->current_music_index]))
        {
            PlayMusicStream (plug->music[plug->current_music_index]);
            read_id3v2 (plug->music_file[plug->current_music_index]);
        }
    UpdateMusicStream (plug->music[plug->current_music_index]);

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
            plug->screen_transition = false;
            plug->camera2d.offset.y = -offset;
            offset = 0;
            velocity = 0;
        }
    else
        {
            plug->camera2d.offset.y = -offset;
        }

    BeginDrawing ();
    ClearBackground (BROWN);

    BeginMode3D (plug->camera);

    DrawPlaneModel (plug);
    DrawPiano (plug);

    DrawModel (plug->models[LIGHT_CUBE], lightPosition, 0.5f, YELLOW);
    for (int i = 0; i < POOL_SIZE; i++)
        {
            if (note_pool[i] > -1)
                {
                    DrawBoundingBoxAsCube (plug->keyBoxes[note_pool[i]], BLUE);
                }
        }

    EndMode3D ();
    BeginMode2D (plug->camera2d);
    for (int i = 0; i < plug->no_of_buttons[FREE_SCREEN]; i++)
        DrawMenuButton (plug->buttons[FREE_SCREEN][i]);

    AlignScreenButtons (
        MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH, GetRenderWidth () * 3 / 4.0,
        GetRenderHeight (), GetRenderHeight () / 20.0,
        plug->no_of_buttons[PAUSE_SCREEN], plug->buttons[PAUSE_SCREEN]);

    Texture2D image = plug->images[1];
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
    for (int i = 0; i < plug->no_of_buttons[PAUSE_SCREEN]; i++)
        DrawMenuButton (plug->buttons[PAUSE_SCREEN][i]);
    DrawTextEx (
        plug->MenuFont,
        TextFormat ("Currently Playing %s - %s", Title, Artist),
        (Vector2){ GetRenderWidth () * 1 / 100.0,
                   GetRenderHeight () + GetRenderHeight () * 1 / 100.0 },
        24 * GetRenderHeight () / 600.0, 1, DARKBROWN);

    DrawFPS (10, 10);
    EndMode2D ();
    EndDrawing ();
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

void
ScreenTransitionPauseToInt (Plug *plug)
{
    float target = GetRenderHeight ();
    plug->camera2d.offset.y = Lerp (plug->camera2d.offset.y, target, 0.1);
    target -= 5;
    if (plug->camera2d.offset.y >= target)
        {
            plug->screen_transition = false;
        }
    BeginDrawing ();
    ClearBackground (BROWN);

    BeginMode3D (plug->camera);

    DrawPlaneModel (plug);
    DrawPiano (plug);

    DrawModel (plug->models[LIGHT_CUBE], lightPosition, 0.5f, YELLOW);
    for (int i = 0; i < POOL_SIZE; i++)
        {
            if (note_pool[i] > -1)
                {
                    DrawBoundingBoxAsCube (plug->keyBoxes[note_pool[i]], BLUE);
                }
        }

    EndMode3D ();
    AlignScreenButtons (
        MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH, GetRenderWidth () * 3 / 4.0, 0,
        GetRenderHeight () / 20.0, plug->no_of_buttons[INTERVAL_SCREEN],
        plug->buttons[INTERVAL_SCREEN]);
    for (int i = 0; i < plug->no_of_buttons[INTERVAL_SCREEN]; i++)
        DrawMenuButton (plug->buttons[INTERVAL_SCREEN][i]);
    BeginMode2D (plug->camera2d);

    AlignScreenButtons (
        MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH, GetRenderWidth () * 3 / 4.0, 0,
        GetRenderHeight () / 20.0, plug->no_of_buttons[PAUSE_SCREEN],
        plug->buttons[PAUSE_SCREEN]);

    DrawBackgroundImage (plug->images[1]);
    for (int i = 0; i < plug->no_of_buttons[PAUSE_SCREEN]; i++)
        DrawMenuButton (plug->buttons[PAUSE_SCREEN][i]);
    DrawTextEx (plug->MenuFont,
                TextFormat ("Currently Playing %s - %s", Title, Artist),
                (Vector2){ GetRenderWidth () * 1 / 100.0,
                           GetRenderHeight () * 1 / 100.0 },
                24 * GetRenderHeight () / 600.0, 1, DARKBROWN);

    DrawFPS (10, 10);
    EndMode2D ();
    EndDrawing ();
}

int
GenNote (int key, Scale scale)
{
    const int note_n = 6 * SCALE_LENGTH;
    Notes note[note_n] = { 0 };
    int current = key;
    note[0] = current;
    if (scale == MAJOR)
        {
            for (int i = 1; i < note_n; i++)
                {
                    if (i % SCALE_LENGTH == 3 || i % SCALE_LENGTH == 0)
                        current += MINOR_SECOND;
                    else
                        current += MAJOR_SECOND;
                    note[i] = current;
                }
        }
    return note[GetRandomValue (0, note_n - 1)];
}

const char *
Key_to_text (int key)
{
    switch (key)
        {
        case 0:
            return "C";
        case 1:
            return "C#";
        case 2:
            return "D";
        case 3:
            return "D#";
        case 4:
            return "E";
        case 5:
            return "F";
        case 6:
            return "F#";
        case 7:
            return "G";
        case 8:
            return "G#";
        case 9:
            return "A";
        case 10:
            return "A#";
        case 11:
            return "B";
        default:
            return "";
        }
}
