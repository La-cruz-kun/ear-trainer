#include "chord_parser.h"
#include "ear_trainer.h"
#include <raylib.h>

static int finish = 0;
static int progression_counter = 0;
static ChordNotes chord_notes = { .chord = { -1, -1, -1, -1 } };

void
InitChordScreen (void)
{
}

void
UpdateChordScreen (void)
{
    if (IsKeyReleased (KEY_ENTER))
        {
            GenChord (key);
        }
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[VIEW_POS_LOC],
                    &camera.position, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[VIEW_POS_LOC],
                    &camera.position, SHADER_UNIFORM_VEC3);
}

void
DrawChordScreen (void)
{

    BeginMode3D (camera);

    DrawPlaneModel ();
    DrawPiano ();

    DrawModel (models[LIGHT_CUBE], lightPosition, 0.5f, YELLOW);
    /* if (note_pool[0] > -1) */
    /*     { */
    /*         DrawBoundingBoxAsCube (keyBoxes[note_pool[0]], note_color); */
    /*     } */
    for (int i = 0; chord_notes.chord[i] > -1; i++)
        {
            DrawBoundingBoxAsCube (keyBoxes[(int)chord_notes.chord[i]], BLUE);
        }
    DrawGrid (10, 1);
    EndMode3D ();
    /* for (int i = 0; i < no_of_buttons[INTERVAL_SCREEN]; i++) */
    /*     DrawMenuButton (buttons[INTERVAL_SCREEN][i]); */
    DrawText (TextFormat ("Current Key is %s", Key_to_text (key)), 30, 30, 24,
              RAYWHITE);
}

void
UpdateIntSettingToChord (void)
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

void
GenChord (int key)
{
    int octave = GetRandomValue (3, 6);
    key = key + OCTAVE * octave;
    if (progression_counter > 3)
        progression_counter = 0;
    chord_notes = convert_chordT_to_chordN (
        progressions.data[0][progression_counter], key);
    progression_counter++;
}

int
FinishChordScreen (void)
{
    return finish;
}
