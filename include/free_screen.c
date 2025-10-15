// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Abraham Kopiam Cruz (@La-cruz-kun)


#include "ear_trainer.h"

static unsigned int currentOctave = 0;
static int finish = 0;
static void CheckKeyPress (void);
void
UpdateFreeScreen (void)
{
    finish = 0;
    CheckKeyPress ();

    // Update light position and view position
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[VIEW_POS_LOC],
                    &camera.position, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[VIEW_POS_LOC],
                    &camera.position, SHADER_UNIFORM_VEC3);
    AlignScreenButtons (MENU_BUTTON_HEIGHT, MENU_BUTTON_WIDTH,
                        GetRenderWidth () * 3 / 4.0, 0,
                        GetRenderHeight () / 20.0, no_of_buttons[FREE_SCREEN],
                        buttons[FREE_SCREEN]);

    if (IsMenuButtonPressed (&buttons[FREE_SCREEN][0]))
        {
            finish = 1;
        }
}
void
DrawFreeScreen (void)
{
    BeginMode3D (camera);

    DrawPlaneModel ();
    DrawPiano ();

    // Draw light cube
    DrawModel (models[LIGHT_CUBE], lightPosition, 0.5f, YELLOW);
    for (int i = 0; i < MAX_CHORD_SIZE; i++)
        {
            if (note_pool[i] > -1)
                {
                    DrawBoundingBoxAsCube (keyBoxes[note_pool[i]], BLUE);
                }
        }
    DrawGrid (10, 1);
    EndMode3D ();
    for (int i = 0; i < no_of_buttons[FREE_SCREEN]; i++)
        DrawMenuButton (buttons[FREE_SCREEN][i]);
    DrawFPS (10, 10);
}

static void
CheckKeyPress (void)
{
    int key;
    while ((key = GetKeyPressed ()) != 0)
        {
            if (hmget (Key_to_note, key) != -1)
                {
                    int note
                        = hmget (Key_to_note, key) + currentOctave * OCTAVE;
                    for (int i = 0; i < MAX_CHORD_SIZE; i++)
                        {
                            if (note_pool[i] == -1)
                                {
                                    note_pool[i] = note;
                                    PlaySound (piano_sound[note]);
                                    break;
                                }
                        }
                    continue;
                }
            if (key == KEY_RIGHT_SHIFT)
                {
                    if (currentOctave >= 7)
                        continue;
                    currentOctave++;
                    continue;
                }
            if (key == KEY_LEFT_SHIFT)
                {
                    if (currentOctave <= 0)
                        continue;
                    currentOctave--;
                    continue;
                }
        }
    for (int i = 0; i < hmlen (Key_to_note); i++)
        {
            KeyboardKey key = Key_to_note[i].key;
            if (IsKeyReleased (key))
                {
                    int note
                        = hmget (Key_to_note, key) + currentOctave * OCTAVE;
                    for (int i = 0; i < MAX_CHORD_SIZE; i++)
                        {
                            if (note_pool[i] == note)
                                {
                                    StopSound (piano_sound[note]);
                                    note_pool[i] = -1;
                                }
                        }
                }
        }
}

void
UpdateHomeToFree (void)
{
    Vector3 target = { 0, 2, 2 };
    camera.position.x = Lerp (camera.position.x, target.x, 0.04);
    camera.position.y = Lerp (camera.position.y, target.y, 0.04);
    camera.position.z = Lerp (camera.position.z, target.z, 0.04);
    float tolerance = 0.1;
    target.x += tolerance;
    target.y += tolerance;
    target.z += tolerance;
    if (camera.position.x <= target.x && camera.position.y <= target.y
        && camera.position.z <= target.z)
        {
            screen_transition = false;
            printf ("done\n");
        }
}

int
FinishFreeScreen (void)
{
    return finish;
}
