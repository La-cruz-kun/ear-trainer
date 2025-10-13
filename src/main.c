#define STB_DS_IMPLEMENTATION
#include <chord_parser.h>
#include <ear_trainer.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h>

#include <stdatomic.h> // C11 atomic data types

#define begin                                                                 \
    static int _state = 0;                                                    \
    if (_state == -1)                                                         \
        return 1;                                                             \
    switch (_state)                                                           \
        {                                                                     \
        case 0:

#define yield()                                                               \
    do                                                                        \
        {                                                                     \
            _state = __LINE__;                                                \
            return 0;                                                         \
        case __LINE__:;                                                       \
        }                                                                     \
    while (0)

#define finish()                                                              \
    do                                                                        \
        {                                                                     \
            _state = -1;                                                      \
            return 1;                                                         \
        }                                                                     \
    while (0)
#define end }

Music drone[OCTAVE] = { 0 };
BoundingBox keyBoxes[NO_OF_NOTES] = { 0 };
Sound sound[NO_OF_NOTES] = { 0 };
bool key_pressed;
KeyboardHashMap *Key_to_note = NULL;
Font font1 = { 0 };
Font font2 = { 0 };
Font font3 = { 0 };

Model models[NO_OF_MODELS] = { 0 };
Light lights;
Shader shaders[NO_OF_SHADERS];
int shaderLoc[NO_OF_SHADER_LOC] = { 0 };
Texture2D maps[NO_OF_MAPS];

enum Screen current_screen = { 0 };
enum Screen after_int_set_screen = NONE;
enum Screen former_screen = { 0 };
Texture images[NO_OF_IMAGES] = { 0 };
Music music[NO_OF_MUSIC] = { 0 };
const char *music_file[NO_OF_MUSIC] = { 0 };
int current_music_index = 0;
MenuButton buttons[NO_OF_SCREEN][MAX_NUM_OF_BUTTON] = { 0 };
int no_of_buttons[NO_OF_SCREEN] = { 0 };
Camera camera = { 0 };
Camera2D camera2d = { 0 };
bool screen_transition = false;
bool quit = false;
bool looped_once = false;

int note_pool[MAX_CHORD_SIZE] = { [0 ... MAX_CHORD_SIZE - 1] = -1 };
int key = -1;
int scale = -1;

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

int framesCounter = 0;

Chord_Progs progressions = { 0 };

int
main (void)
{
    SetConfigFlags (FLAG_FULLSCREEN_MODE);
    SetConfigFlags (FLAG_MSAA_4X_HINT);
    // window aspect ratio 16:9
    InitWindow (16, 9, "Ear Trainer");
    InitAudioDevice ();
    current_screen = LOADING_SCREEN;
    InitLoadingScreen ();

    SetTargetFPS (60);
    while (!WindowShouldClose () && !quit)
        {
            update ();
        }
    CloseWindow ();
    return 0;
}

int
init (void)
{
    begin;
    int codepointCount = 0;
    int *codepoints = LoadCodepoints (text, &codepointCount);
    font1 = LoadFontEx ("resources/fonts/0xProtoNerdFontMono-Italic.ttf", 64,
                        codepoints, codepointCount);
    font2
        = LoadFontEx ("resources/fonts/OpenDyslexicMNerdFontPropo-Regular.otf",
                      64, codepoints, codepointCount);
    font3 = LoadFontEx ("resources/fonts/Junicode-Regular.ttf", 128,
                        codepoints, codepointCount);
    yield ();
    camera.position = (Vector3){ 0, 2, 2 };
    camera.target = (Vector3){ 0, 0, -0.2 };
    camera.up = (Vector3){ 0, 1, 0 };
    camera.projection = CAMERA_PERSPECTIVE;
    camera.fovy = 45.0;

    camera2d.target = (Vector2){ 0, 0 };
    camera2d.offset = (Vector2){ 0, 0 };
    camera2d.rotation = 0.0f;
    camera2d.zoom = 1.0f;

    hmdefault (Key_to_note, -1);
    hmput (Key_to_note, KEY_A, C1);
    hmput (Key_to_note, KEY_S, D1);
    hmput (Key_to_note, KEY_D, E1);
    hmput (Key_to_note, KEY_F, F1);
    hmput (Key_to_note, KEY_G, G1);
    hmput (Key_to_note, KEY_H, A1);
    hmput (Key_to_note, KEY_J, B1);
    hmput (Key_to_note, KEY_K, C2);
    hmput (Key_to_note, KEY_L, D2);
    hmput (Key_to_note, KEY_SEMICOLON, E2);
    hmput (Key_to_note, KEY_APOSTROPHE, F2);
    hmput (Key_to_note, KEY_W, CS1);
    hmput (Key_to_note, KEY_E, DS1);
    hmput (Key_to_note, KEY_T, FS1);
    hmput (Key_to_note, KEY_Y, GS1);
    hmput (Key_to_note, KEY_U, AS1);
    hmput (Key_to_note, KEY_O, CS2);
    hmput (Key_to_note, KEY_P, DS2);

    yield ();

    current_music_index = GetRandomValue (0, NO_OF_MUSIC - 1);
    drone[C] = LoadMusicStream ("resources/audio/drone/C.mp3");
    drone[CS] = LoadMusicStream ("resources/audio/drone/C#.mp3");
    drone[D] = LoadMusicStream ("resources/audio/drone/D.mp3");
    drone[DS] = LoadMusicStream ("resources/audio/drone/D#.mp3");
    drone[E] = LoadMusicStream ("resources/audio/drone/E.mp3");
    drone[F] = LoadMusicStream ("resources/audio/drone/F.mp3");
    yield ();
    drone[FS] = LoadMusicStream ("resources/audio/drone/F#.mp3");
    drone[G] = LoadMusicStream ("resources/audio/drone/G.mp3");
    drone[GS] = LoadMusicStream ("resources/audio/drone/G#.mp3");
    drone[A] = LoadMusicStream ("resources/audio/drone/A.mp3");
    drone[AS] = LoadMusicStream ("resources/audio/drone/A#.mp3");
    drone[B] = LoadMusicStream ("resources/audio/drone/B.mp3");
    yield ();
    parse_chord_file ("resources/chordprogression.txt");

    finish ();

    /* PlayMusicStream (music[current_music_index]); */
    /* read_id3v2 (music_file[current_music_index]); */

    // When data has finished loading, we set global variable
    end;
    return 1;
}

void
update (void)
{
    if (!screen_transition)
        {
            switch (current_screen)
                {
                case LOADING_SCREEN:
                    {
                        UpdateLoadingScreen ();
                        if (FinishLoadingScreen ())
                            ChangeToScreen (HOME_SCREEN);
                        break;
                    }
                case HOME_SCREEN:
                    {
                        UpdateHomeScreen ();
                        if (FinishHomeScreen () == 1)
                            {
                                after_int_set_screen = INTERVAL_SCREEN;
                                TransitionToScreen (INTERVAL_SETTING_SCREEN);
                            }
                        else if (FinishHomeScreen () == 2)
                            TransitionToScreen (FREE_SCREEN);
                        else if (FinishHomeScreen () == 3)
                            {
                                after_int_set_screen = CHORD_SCREEN;
                                TransitionToScreen (INTERVAL_SETTING_SCREEN);
                            }
                        else if (FinishHomeScreen () == 5)
                            quit = true;
                        break;
                    }
                case FREE_SCREEN:
                    {
                        UpdateFreeScreen ();
                        if (FinishFreeScreen () == 1)
                            TransitionToScreen (PAUSE_SCREEN);
                        break;
                    }
                case INTERVAL_SETTING_SCREEN:
                    {
                        UpdateIntervalSettingScreen ();
                        if (FinishIntSettingScreen () == 1)
                            TransitionToScreen (INTERVAL_SCREEN);
                        else if (FinishIntSettingScreen () == 2)
                            TransitionToScreen (CHORD_SCREEN);
                        break;
                    }
                case INTERVAL_SCREEN:
                    {
                        UpdateIntervalScreen ();
                        if (FinishIntScreen () == 1)
                            TransitionToScreen (PAUSE_SCREEN);
                        break;
                    }
                case CHORD_SCREEN:
                    {
                        UpdateChordScreen ();
                        break;
                    }
                case PAUSE_SCREEN:
                    {
                        UpdatePauseScreen ();
                        if (FinishPauseScreen () == 1)
                            TransitionToScreen (INTERVAL_SCREEN);
                        else if (FinishPauseScreen () == 2)
                            TransitionToScreen (FREE_SCREEN);
                        else if (FinishPauseScreen () == 3)
                            TransitionToScreen (HOME_SCREEN);
                        break;
                    }
                case SETTING_SCREEN:
                    {
                        UpdateSettingScreen ();
                        if (FinishSettingScreen () == 1)
                            TransitionToScreen (HOME_SCREEN);
                        else if (FinishSettingScreen () == 2)
                            TransitionToScreen (PAUSE_SCREEN);
                        break;
                    }
                default:
                    break;
                }
        }
    else
        {
            UpdateTransition ();
        }

    BeginDrawing ();
    ClearBackground (BLACK);
    if (!screen_transition)
        {
            switch (current_screen)
                {
                case LOADING_SCREEN:
                    DrawLoadingScreen ();
                    break;
                case HOME_SCREEN:
                    DrawHomeScreen ();
                    break;
                case FREE_SCREEN:
                    DrawFreeScreen ();
                    break;
                case INTERVAL_SETTING_SCREEN:
                    DrawIntervalSettingScreen ();
                    break;
                case INTERVAL_SCREEN:
                    DrawIntervalScreen ();
                    break;
                case CHORD_SCREEN:
                    DrawChordScreen ();
                    break;
                case PAUSE_SCREEN:
                    DrawPauseScreen ();
                    break;
                case SETTING_SCREEN:
                    DrawSettingScreen ();
                    break;
                default:
                    break;
                }
        }
    else
        {
            DrawTransition ();
        }
    DrawFPS (10, 10);
    EndDrawing ();
}

int
MapPianoNotes (void)
{

    begin;
    keyBoxes[GS1] = GetMeshBoundingBox (models[PIANO].meshes[2]);
    keyBoxes[AS1] = GetMeshBoundingBox (models[PIANO].meshes[3]);
    keyBoxes[FS1] = GetMeshBoundingBox (models[PIANO].meshes[4]);
    keyBoxes[FS2] = GetMeshBoundingBox (models[PIANO].meshes[5]);
    keyBoxes[GS2] = GetMeshBoundingBox (models[PIANO].meshes[6]);
    keyBoxes[AS2] = GetMeshBoundingBox (models[PIANO].meshes[7]);
    keyBoxes[FS3] = GetMeshBoundingBox (models[PIANO].meshes[8]);
    keyBoxes[GS3] = GetMeshBoundingBox (models[PIANO].meshes[9]);
    keyBoxes[AS3] = GetMeshBoundingBox (models[PIANO].meshes[10]);
    keyBoxes[FS4] = GetMeshBoundingBox (models[PIANO].meshes[11]);
    keyBoxes[GS4] = GetMeshBoundingBox (models[PIANO].meshes[12]);
    keyBoxes[AS4] = GetMeshBoundingBox (models[PIANO].meshes[13]);
    keyBoxes[FS5] = GetMeshBoundingBox (models[PIANO].meshes[14]);
    keyBoxes[GS5] = GetMeshBoundingBox (models[PIANO].meshes[15]);
    keyBoxes[AS5] = GetMeshBoundingBox (models[PIANO].meshes[16]);
    keyBoxes[FS6] = GetMeshBoundingBox (models[PIANO].meshes[17]);
    keyBoxes[GS6] = GetMeshBoundingBox (models[PIANO].meshes[18]);
    keyBoxes[AS6] = GetMeshBoundingBox (models[PIANO].meshes[19]);
    keyBoxes[FS7] = GetMeshBoundingBox (models[PIANO].meshes[20]);
    keyBoxes[GS7] = GetMeshBoundingBox (models[PIANO].meshes[21]);
    keyBoxes[AS7] = GetMeshBoundingBox (models[PIANO].meshes[22]);
    keyBoxes[CS1] = GetMeshBoundingBox (models[PIANO].meshes[24]);
    keyBoxes[DS1] = GetMeshBoundingBox (models[PIANO].meshes[25]);
    keyBoxes[CS2] = GetMeshBoundingBox (models[PIANO].meshes[26]);
    keyBoxes[DS2] = GetMeshBoundingBox (models[PIANO].meshes[27]);
    keyBoxes[CS3] = GetMeshBoundingBox (models[PIANO].meshes[28]);
    keyBoxes[DS3] = GetMeshBoundingBox (models[PIANO].meshes[29]);
    keyBoxes[CS4] = GetMeshBoundingBox (models[PIANO].meshes[30]);
    keyBoxes[DS4] = GetMeshBoundingBox (models[PIANO].meshes[31]);
    keyBoxes[CS5] = GetMeshBoundingBox (models[PIANO].meshes[32]);
    keyBoxes[DS5] = GetMeshBoundingBox (models[PIANO].meshes[33]);
    keyBoxes[CS6] = GetMeshBoundingBox (models[PIANO].meshes[34]);
    yield ();

    keyBoxes[DS6] = GetMeshBoundingBox (models[PIANO].meshes[35]);
    keyBoxes[CS7] = GetMeshBoundingBox (models[PIANO].meshes[36]);
    keyBoxes[DS7] = GetMeshBoundingBox (models[PIANO].meshes[37]);
    keyBoxes[C1] = GetMeshBoundingBox (models[PIANO].meshes[40]);
    keyBoxes[D1] = GetMeshBoundingBox (models[PIANO].meshes[41]);
    keyBoxes[E1] = GetMeshBoundingBox (models[PIANO].meshes[42]);
    keyBoxes[F1] = GetMeshBoundingBox (models[PIANO].meshes[43]);
    keyBoxes[G1] = GetMeshBoundingBox (models[PIANO].meshes[44]);
    keyBoxes[A1] = GetMeshBoundingBox (models[PIANO].meshes[45]);
    keyBoxes[B1] = GetMeshBoundingBox (models[PIANO].meshes[46]);
    keyBoxes[C2] = GetMeshBoundingBox (models[PIANO].meshes[47]);
    keyBoxes[D2] = GetMeshBoundingBox (models[PIANO].meshes[48]);
    keyBoxes[E2] = GetMeshBoundingBox (models[PIANO].meshes[49]);
    keyBoxes[F2] = GetMeshBoundingBox (models[PIANO].meshes[50]);
    keyBoxes[G2] = GetMeshBoundingBox (models[PIANO].meshes[51]);
    keyBoxes[A2] = GetMeshBoundingBox (models[PIANO].meshes[52]);
    keyBoxes[B2] = GetMeshBoundingBox (models[PIANO].meshes[53]);
    keyBoxes[C3] = GetMeshBoundingBox (models[PIANO].meshes[54]);
    keyBoxes[D3] = GetMeshBoundingBox (models[PIANO].meshes[55]);
    keyBoxes[E3] = GetMeshBoundingBox (models[PIANO].meshes[56]);
    keyBoxes[F3] = GetMeshBoundingBox (models[PIANO].meshes[57]);
    keyBoxes[G3] = GetMeshBoundingBox (models[PIANO].meshes[58]);
    keyBoxes[A3] = GetMeshBoundingBox (models[PIANO].meshes[59]);
    keyBoxes[B3] = GetMeshBoundingBox (models[PIANO].meshes[60]);
    keyBoxes[C4] = GetMeshBoundingBox (models[PIANO].meshes[61]);
    yield ();

    keyBoxes[D4] = GetMeshBoundingBox (models[PIANO].meshes[62]);
    keyBoxes[E4] = GetMeshBoundingBox (models[PIANO].meshes[63]);
    keyBoxes[F4] = GetMeshBoundingBox (models[PIANO].meshes[64]);
    keyBoxes[G4] = GetMeshBoundingBox (models[PIANO].meshes[65]);
    keyBoxes[A4] = GetMeshBoundingBox (models[PIANO].meshes[66]);
    keyBoxes[B4] = GetMeshBoundingBox (models[PIANO].meshes[67]);
    keyBoxes[C5] = GetMeshBoundingBox (models[PIANO].meshes[68]);
    keyBoxes[D5] = GetMeshBoundingBox (models[PIANO].meshes[69]);
    keyBoxes[E5] = GetMeshBoundingBox (models[PIANO].meshes[70]);
    keyBoxes[F5] = GetMeshBoundingBox (models[PIANO].meshes[71]);
    keyBoxes[G5] = GetMeshBoundingBox (models[PIANO].meshes[72]);
    keyBoxes[A5] = GetMeshBoundingBox (models[PIANO].meshes[73]);
    keyBoxes[B5] = GetMeshBoundingBox (models[PIANO].meshes[74]);
    keyBoxes[C6] = GetMeshBoundingBox (models[PIANO].meshes[75]);
    keyBoxes[D6] = GetMeshBoundingBox (models[PIANO].meshes[76]);
    keyBoxes[E6] = GetMeshBoundingBox (models[PIANO].meshes[77]);
    keyBoxes[F6] = GetMeshBoundingBox (models[PIANO].meshes[78]);
    keyBoxes[G6] = GetMeshBoundingBox (models[PIANO].meshes[79]);
    keyBoxes[A6] = GetMeshBoundingBox (models[PIANO].meshes[80]);
    keyBoxes[B6] = GetMeshBoundingBox (models[PIANO].meshes[81]);
    keyBoxes[C7] = GetMeshBoundingBox (models[PIANO].meshes[82]);
    keyBoxes[D7] = GetMeshBoundingBox (models[PIANO].meshes[83]);
    keyBoxes[E7] = GetMeshBoundingBox (models[PIANO].meshes[84]);
    keyBoxes[F7] = GetMeshBoundingBox (models[PIANO].meshes[85]);
    keyBoxes[G7] = GetMeshBoundingBox (models[PIANO].meshes[86]);
    keyBoxes[A7] = GetMeshBoundingBox (models[PIANO].meshes[87]);
    keyBoxes[B7] = GetMeshBoundingBox (models[PIANO].meshes[88]);
    keyBoxes[C8] = GetMeshBoundingBox (models[PIANO].meshes[89]);
    finish ();
    end;
    return 1;
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

int
LoadPlaneModel (void)
{
    begin;
    models[PLANE] = LoadModelFromMesh (GenMeshPlane (10, 10, 1, 1));
    yield ();

    shaders[PLANE_SHADER] = LoadShader ("resources/shaders/main_vs.glsl",
                                        "resources/shaders/plane_fs.glsl");
    yield ();

    // Texture uniform locations
    int matDiffLoc
        = GetShaderLocation (shaders[PLANE_SHADER], "material.diffuse");
    int matRoughnessLoc
        = GetShaderLocation (shaders[PLANE_SHADER], "material.roughness");
    int matNormalLoc
        = GetShaderLocation (shaders[PLANE_SHADER], "material.normal");
    int matMetallicLoc
        = GetShaderLocation (shaders[PLANE_SHADER], "material.metallic");
    int matAOLoc = GetShaderLocation (shaders[PLANE_SHADER], "material.ao");
    int tileLoc = GetShaderLocation (shaders[PLANE_SHADER], "tile");

    // Set texture units
    int diffuseUnit = 0;
    int roughnessUnit = 1;
    int normalUnit = 2;
    int metallicUnit = 3;
    int aoUnit = 4;
    Vector2 tile = (Vector2){ 5, 5 };
    SetShaderValue (shaders[PLANE_SHADER], matDiffLoc, &diffuseUnit,
                    SHADER_UNIFORM_INT);
    SetShaderValue (shaders[PLANE_SHADER], matRoughnessLoc, &roughnessUnit,
                    SHADER_UNIFORM_INT);
    SetShaderValue (shaders[PLANE_SHADER], matMetallicLoc, &metallicUnit,
                    SHADER_UNIFORM_INT);
    SetShaderValue (shaders[PLANE_SHADER], matAOLoc, &aoUnit,
                    SHADER_UNIFORM_INT);
    SetShaderValue (shaders[PLANE_SHADER], matNormalLoc, &normalUnit,
                    SHADER_UNIFORM_INT);
    SetShaderValue (shaders[PLANE_SHADER], tileLoc, &tile,
                    SHADER_UNIFORM_VEC2);

    printf ("Number of materials: %d\n", models[PLANE].materialCount);
    printf ("Number of meshes: %d\n", models[PLANE].meshCount);

    // Set light uniforms
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_AMB_LOC],
                    &lightAmbient, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_DIFF_LOC],
                    &lightDiffuse, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_SPEC_LOC],
                    &lightSpecular, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_CONST_LOC],
                    &lightConstant, SHADER_UNIFORM_FLOAT);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_LINEAR_LOC],
                    &lightLinear, SHADER_UNIFORM_FLOAT);
    SetShaderValue (shaders[PLANE_SHADER], shaderLoc[LIGHT_QUADRATIC_LOC],
                    &lightQuadratic, SHADER_UNIFORM_FLOAT);

    // Prepare textures for both materials
    // Material 0: Piano body

    // Extract textures from materials if they exist
    maps[PLANE_ALBEDO] = LoadTexture ("resources/textures/stoneBrick/COL.jpg");
    yield ();
    maps[PLANE_ROUGHNESS]
        = LoadTexture ("resources/textures/stoneBrick/ROUGHNESS.jpg");
    yield ();
    maps[PLANE_METALLIC]
        = LoadTexture ("resources/textures/stoneBrick/METALLIC.jpg");
    yield ();
    maps[PLANE_AO] = LoadTexture ("resources/textures/stoneBrick/AO.jpg");
    yield ();
    maps[PLANE_NORMAL]
        = LoadTexture ("resources/textures/stoneBrick/NORMAL.png");
    yield ();
    models[PLANE].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture
        = maps[PLANE_ALBEDO];
    models[PLANE].materials[0].maps[MATERIAL_MAP_ROUGHNESS].texture
        = maps[PLANE_ROUGHNESS];
    models[PLANE].materials[0].maps[MATERIAL_MAP_METALNESS].texture
        = maps[PLANE_METALLIC];
    models[PLANE].materials[0].maps[MATERIAL_MAP_OCCLUSION].texture
        = maps[PLANE_AO];
    models[PLANE].materials[0].maps[MATERIAL_MAP_NORMAL].texture
        = maps[PLANE_NORMAL];

    // Assign shader to material
    models[PLANE].materials[0].shader = shaders[PLANE_SHADER];
    finish ();
    end;
    return 1;
}

int
LoadPianoModel (void)
{
    begin;
    models[PIANO] = LoadModel ("resources/models/edited.gltf");
    yield ();

    shaders[PBR_SHADER] = LoadShader ("resources/shaders/main_vs.glsl",
                                      "resources/shaders/main_fs.glsl");
    yield ();
    shaders[LIGHT_SHADER] = LoadShader ("resources/shaders/main_vs.glsl",
                                        "resources/shaders/light.glsl");
    yield ();

    // Load models
    models[LIGHT_CUBE] = LoadModelFromMesh (GenMeshCube (0.5f, 0.5f, 0.5f));

    yield ();
    // Get shader locations
    shaderLoc[LIGHT_POS_LOC]
        = GetShaderLocation (shaders[PBR_SHADER], "light.position");
    shaderLoc[LIGHT_AMB_LOC]
        = GetShaderLocation (shaders[PBR_SHADER], "light.ambient");
    shaderLoc[LIGHT_DIFF_LOC]
        = GetShaderLocation (shaders[PBR_SHADER], "light.diffuse");
    shaderLoc[LIGHT_SPEC_LOC]
        = GetShaderLocation (shaders[PBR_SHADER], "light.specular");
    shaderLoc[LIGHT_CONST_LOC]
        = GetShaderLocation (shaders[PBR_SHADER], "light.constant");
    shaderLoc[LIGHT_LINEAR_LOC]
        = GetShaderLocation (shaders[PBR_SHADER], "light.linear");
    shaderLoc[LIGHT_QUADRATIC_LOC]
        = GetShaderLocation (shaders[PBR_SHADER], "light.quadratic");

    shaderLoc[VIEW_POS_LOC]
        = GetShaderLocation (shaders[PBR_SHADER], "viewPos");

    // Texture uniform locations
    int matDiffLoc
        = GetShaderLocation (shaders[PBR_SHADER], "material.diffuse");
    int matMetallicRoughnessLoc = GetShaderLocation (
        shaders[PBR_SHADER], "material.metallicRoughness");
    int matNormalLoc
        = GetShaderLocation (shaders[PBR_SHADER], "material.normal");

    // Set texture units
    int diffuseUnit = 0;
    int metallicRoughnessUnit = 1;
    int normalUnit = 2;
    SetShaderValue (shaders[PBR_SHADER], matDiffLoc, &diffuseUnit,
                    SHADER_UNIFORM_INT);
    SetShaderValue (shaders[PBR_SHADER], matMetallicRoughnessLoc,
                    &metallicRoughnessUnit, SHADER_UNIFORM_INT);
    SetShaderValue (shaders[PBR_SHADER], matNormalLoc, &normalUnit,
                    SHADER_UNIFORM_INT);

    // Check how many materials we have
    printf ("Number of materials: %d\n", models[PIANO].materialCount);
    printf ("Number of meshes: %d\n", models[PIANO].meshCount);

    // Set up light properties

    yield ();
    // Set light uniforms
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_POS_LOC],
                    &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_AMB_LOC],
                    &lightAmbient, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_DIFF_LOC],
                    &lightDiffuse, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_SPEC_LOC],
                    &lightSpecular, SHADER_UNIFORM_VEC3);
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_CONST_LOC],
                    &lightConstant, SHADER_UNIFORM_FLOAT);
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_LINEAR_LOC],
                    &lightLinear, SHADER_UNIFORM_FLOAT);
    SetShaderValue (shaders[PBR_SHADER], shaderLoc[LIGHT_QUADRATIC_LOC],
                    &lightQuadratic, SHADER_UNIFORM_FLOAT);

    // Prepare textures for both materials
    // Material 0: Piano body

    // Extract textures from materials if they exist
    if (models[PIANO].materialCount > 1)
        {
            // First material (index 0)
            maps[PIANO_ALBEDO] = models[PIANO]
                                     .materials[1]
                                     .maps[MATERIAL_MAP_DIFFUSE]
                                     .texture;
            maps[PIANO_METALLIC_ROUGHNESS] = models[PIANO]
                                                 .materials[1]
                                                 .maps[MATERIAL_MAP_METALNESS]
                                                 .texture;
            maps[PIANO_NORMAL]
                = models[PIANO].materials[1].maps[MATERIAL_MAP_NORMAL].texture;

            // Assign shader to material
            models[PIANO].materials[1].shader = shaders[PBR_SHADER];
        }

    if (models[PIANO].materialCount > 2)
        {
            // Second material (index 1)
            maps[BENCH_ALBEDO] = models[PIANO]
                                     .materials[2]
                                     .maps[MATERIAL_MAP_DIFFUSE]
                                     .texture;
            maps[BENCH_METALLIC_ROUGHNESS] = models[PIANO]
                                                 .materials[2]
                                                 .maps[MATERIAL_MAP_METALNESS]
                                                 .texture;
            maps[BENCH_NORMAL]
                = models[PIANO].materials[2].maps[MATERIAL_MAP_NORMAL].texture;

            // Assign shader to material
            models[PIANO].materials[2].shader = shaders[PBR_SHADER];
        }

    yield ();
    // Create default textures if needed
    if (maps[PIANO_ALBEDO].id == 0)
        {
            Image img = GenImageColor (1, 1, WHITE);
            maps[PIANO_ALBEDO] = LoadTextureFromImage (img);
            UnloadImage (img);
        }
    if (maps[PIANO_METALLIC_ROUGHNESS].id == 0)
        {
            Image img = GenImageColor (
                1, 1,
                (Color){ 0, 128, 255, 255 }); // Default roughness and metallic
            maps[PIANO_METALLIC_ROUGHNESS] = LoadTextureFromImage (img);
            UnloadImage (img);
        }
    if (maps[PIANO_NORMAL].id == 0)
        {
            Image img = GenImageColor (
                1, 1,
                (Color){ 128, 128, 255, 255 }); // Default normal (pointing up)
            maps[PIANO_NORMAL] = LoadTextureFromImage (img);
            UnloadImage (img);
        }
    finish ();
    end;
    return 1;
}

void
DrawPiano (void)
{
    for (int i = 0; i < models[PIANO].meshCount; i++)
        {
            // Get material index for this mesh
            int materialIndex = 0;
            if (models[PIANO].meshMaterial != NULL)
                {
                    materialIndex = models[PIANO].meshMaterial[i];
                }

            // Bind appropriate textures based on material
            if (materialIndex == 1)
                {
                    // Piano material
                    rlActiveTextureSlot (0);
                    rlEnableTexture (maps[PIANO_ALBEDO].id);
                    rlActiveTextureSlot (1);
                    rlEnableTexture (maps[PIANO_METALLIC_ROUGHNESS].id);
                    rlActiveTextureSlot (2);
                    rlEnableTexture (maps[PIANO_NORMAL].id);
                }
            else if (materialIndex == 2)
                {
                    // Bench material
                    rlActiveTextureSlot (0);
                    rlEnableTexture (maps[BENCH_ALBEDO].id);
                    rlActiveTextureSlot (1);
                    rlEnableTexture (maps[BENCH_METALLIC_ROUGHNESS].id);
                    rlActiveTextureSlot (2);
                    rlEnableTexture (maps[BENCH_NORMAL].id);
                }

            // Draw mesh with the PBR shader
            if (materialIndex < models[PIANO].materialCount)
                {
                    DrawMesh (models[PIANO].meshes[i],
                              models[PIANO].materials[materialIndex],
                              MatrixIdentity ());
                }
        }
}

void
DrawPlaneModel (void)
{
    rlActiveTextureSlot (0);
    rlEnableTexture (maps[PLANE_ALBEDO].id);
    rlActiveTextureSlot (1);
    rlEnableTexture (maps[PLANE_ROUGHNESS].id);
    rlActiveTextureSlot (2);
    rlEnableTexture (maps[PLANE_NORMAL].id);
    rlActiveTextureSlot (3);
    rlEnableTexture (maps[PLANE_METALLIC].id);
    rlActiveTextureSlot (4);
    rlEnableTexture (maps[PLANE_AO].id);
    DrawMesh (models[PLANE].meshes[0], models[PLANE].materials[0],
              MatrixIdentity ());
}

int
LoadSoundInstrument (void)
{
    begin;
    // Octave 1
    sound[C1] = LoadSound ("resources/instrument/piano/1-c.mp3");
    sound[CS1] = LoadSound ("resources/instrument/piano/1-cs.mp3");
    yield ();
    sound[D1] = LoadSound ("resources/instrument/piano/1-d.mp3");
    yield ();
    sound[DS1] = LoadSound ("resources/instrument/piano/1-ds.mp3");
    sound[E1] = LoadSound ("resources/instrument/piano/1-e.mp3");
    yield ();
    sound[F1] = LoadSound ("resources/instrument/piano/1-f.mp3");
    sound[FS1] = LoadSound ("resources/instrument/piano/1-fs.mp3");
    yield ();
    sound[G1] = LoadSound ("resources/instrument/piano/1-g.mp3");
    sound[GS1] = LoadSound ("resources/instrument/piano/1-gs.mp3");
    yield ();
    sound[A1] = LoadSound ("resources/instrument/piano/1-a.mp3");
    sound[AS1] = LoadSound ("resources/instrument/piano/1-as.mp3");
    yield ();
    sound[B1] = LoadSound ("resources/instrument/piano/1-b.mp3");

    yield ();
    // Octave 2
    sound[C2] = LoadSound ("resources/instrument/piano/2-c.mp3");
    sound[CS2] = LoadSound ("resources/instrument/piano/2-cs.mp3");
    yield ();
    sound[D2] = LoadSound ("resources/instrument/piano/2-d.mp3");
    yield ();
    sound[DS2] = LoadSound ("resources/instrument/piano/2-ds.mp3");
    sound[E2] = LoadSound ("resources/instrument/piano/2-e.mp3");
    yield ();
    sound[F2] = LoadSound ("resources/instrument/piano/2-f.mp3");
    sound[FS2] = LoadSound ("resources/instrument/piano/2-fs.mp3");
    yield ();
    sound[G2] = LoadSound ("resources/instrument/piano/2-g.mp3");
    yield ();
    sound[GS2] = LoadSound ("resources/instrument/piano/2-gs.mp3");
    yield ();
    sound[A2] = LoadSound ("resources/instrument/piano/2-a.mp3");
    yield ();
    sound[AS2] = LoadSound ("resources/instrument/piano/2-as.mp3");
    sound[B2] = LoadSound ("resources/instrument/piano/2-b.mp3");
    yield ();

    // Octave 3
    sound[C3] = LoadSound ("resources/instrument/piano/3-c.mp3");
    yield ();
    sound[CS3] = LoadSound ("resources/instrument/piano/3-cs.mp3");
    yield ();
    sound[D3] = LoadSound ("resources/instrument/piano/3-d.mp3");
    sound[DS3] = LoadSound ("resources/instrument/piano/3-ds.mp3");
    yield ();
    sound[E3] = LoadSound ("resources/instrument/piano/3-e.mp3");
    sound[F3] = LoadSound ("resources/instrument/piano/3-f.mp3");
    yield ();
    sound[FS3] = LoadSound ("resources/instrument/piano/3-fs.mp3");
    sound[G3] = LoadSound ("resources/instrument/piano/3-g.mp3");
    yield ();
    sound[GS3] = LoadSound ("resources/instrument/piano/3-gs.mp3");
    sound[A3] = LoadSound ("resources/instrument/piano/3-a.mp3");
    yield ();
    sound[AS3] = LoadSound ("resources/instrument/piano/3-as.mp3");
    sound[B3] = LoadSound ("resources/instrument/piano/3-b.mp3");
    yield ();

    // Octave 4
    sound[C4] = LoadSound ("resources/instrument/piano/4-c.mp3");
    yield ();
    sound[CS4] = LoadSound ("resources/instrument/piano/4-cs.mp3");
    yield ();
    sound[D4] = LoadSound ("resources/instrument/piano/4-d.mp3");
    sound[DS4] = LoadSound ("resources/instrument/piano/4-ds.mp3");
    yield ();
    sound[E4] = LoadSound ("resources/instrument/piano/4-e.mp3");
    sound[F4] = LoadSound ("resources/instrument/piano/4-f.mp3");
    yield ();
    sound[FS4] = LoadSound ("resources/instrument/piano/4-fs.mp3");
    yield ();
    sound[G4] = LoadSound ("resources/instrument/piano/4-g.mp3");
    sound[GS4] = LoadSound ("resources/instrument/piano/4-gs.mp3");
    yield ();
    sound[A4] = LoadSound ("resources/instrument/piano/4-a.mp3");
    sound[AS4] = LoadSound ("resources/instrument/piano/4-as.mp3");
    yield ();
    sound[B4] = LoadSound ("resources/instrument/piano/4-b.mp3");

    // Octave 5
    sound[C5] = LoadSound ("resources/instrument/piano/5-c.mp3");
    yield ();
    sound[CS5] = LoadSound ("resources/instrument/piano/5-cs.mp3");
    sound[D5] = LoadSound ("resources/instrument/piano/5-d.mp3");
    yield ();
    sound[DS5] = LoadSound ("resources/instrument/piano/5-ds.mp3");
    sound[E5] = LoadSound ("resources/instrument/piano/5-e.mp3");
    yield ();
    sound[F5] = LoadSound ("resources/instrument/piano/5-f.mp3");
    yield ();
    sound[FS5] = LoadSound ("resources/instrument/piano/5-fs.mp3");
    yield ();
    sound[G5] = LoadSound ("resources/instrument/piano/5-g.mp3");
    yield ();
    sound[GS5] = LoadSound ("resources/instrument/piano/5-gs.mp3");
    yield ();
    sound[A5] = LoadSound ("resources/instrument/piano/5-a.mp3");
    sound[AS5] = LoadSound ("resources/instrument/piano/5-as.mp3");
    yield ();
    sound[B5] = LoadSound ("resources/instrument/piano/5-b.mp3");

    yield ();
    // Octave 6
    sound[C6] = LoadSound ("resources/instrument/piano/6-c.mp3");
    yield ();
    sound[CS6] = LoadSound ("resources/instrument/piano/6-cs.mp3");
    sound[D6] = LoadSound ("resources/instrument/piano/6-d.mp3");
    yield ();
    sound[DS6] = LoadSound ("resources/instrument/piano/6-ds.mp3");
    sound[E6] = LoadSound ("resources/instrument/piano/6-e.mp3");
    yield ();
    sound[F6] = LoadSound ("resources/instrument/piano/6-f.mp3");
    sound[FS6] = LoadSound ("resources/instrument/piano/6-fs.mp3");
    yield ();
    sound[G6] = LoadSound ("resources/instrument/piano/6-g.mp3");
    sound[GS6] = LoadSound ("resources/instrument/piano/6-gs.mp3");
    yield ();
    sound[A6] = LoadSound ("resources/instrument/piano/6-a.mp3");
    yield ();
    sound[AS6] = LoadSound ("resources/instrument/piano/6-as.mp3");
    yield ();
    sound[B6] = LoadSound ("resources/instrument/piano/6-b.mp3");

    yield ();
    // Octave 7
    sound[C7] = LoadSound ("resources/instrument/piano/7-c.mp3");
    yield ();
    sound[CS7] = LoadSound ("resources/instrument/piano/7-cs.mp3");
    sound[D7] = LoadSound ("resources/instrument/piano/7-d.mp3");
    yield ();
    sound[DS7] = LoadSound ("resources/instrument/piano/7-ds.mp3");
    sound[E7] = LoadSound ("resources/instrument/piano/7-e.mp3");
    yield ();
    sound[F7] = LoadSound ("resources/instrument/piano/7-f.mp3");
    sound[FS7] = LoadSound ("resources/instrument/piano/7-fs.mp3");
    yield ();
    sound[G7] = LoadSound ("resources/instrument/piano/7-g.mp3");
    sound[GS7] = LoadSound ("resources/instrument/piano/7-gs.mp3");
    yield ();
    sound[A7] = LoadSound ("resources/instrument/piano/7-a.mp3");
    sound[AS7] = LoadSound ("resources/instrument/piano/7-as.mp3");
    yield ();
    sound[B7] = LoadSound ("resources/instrument/piano/7-b.mp3");

    yield ();
    // Top C (octave 8)
    sound[C8] = LoadSound ("resources/instrument/piano/8-c.mp3");
    finish ();
    end;
    return 1;
}

int
LoadResources (void)
{
    begin;
    images[0] = LoadTexture ("resources/images/background1.jpg");
    images[1] = LoadTexture ("resources/images/background2.jpg");
    yield ();
    // home screen buttons
    images[2] = LoadTexture ("resources/images/button1.png");
    images[3] = LoadTexture ("resources/images/button2.png");
    images[4] = LoadTexture ("resources/images/button3.png");
    images[5] = LoadTexture ("resources/images/button4.png");
    yield ();

    // pause screen buttons
    images[6] = LoadTexture ("resources/images/button5.png");
    images[7] = LoadTexture ("resources/images/button6.png");
    images[8] = LoadTexture ("resources/images/button7.png");
    images[9] = LoadTexture ("resources/images/button8.png");
    yield ();

    // interval settin
    // white button C - B
    images[10] = LoadTexture ("resources/images/button9.png");
    images[11] = LoadTexture ("resources/images/button10.png");
    images[12] = LoadTexture ("resources/images/button11.png");
    images[13] = LoadTexture ("resources/images/button12.png");
    images[14] = LoadTexture ("resources/images/button13.png");
    yield ();
    images[15] = LoadTexture ("resources/images/button14.png");
    images[16] = LoadTexture ("resources/images/button15.png");
    images[17] = LoadTexture ("resources/images/button16.png");
    images[18] = LoadTexture ("resources/images/button17.png");
    images[19] = LoadTexture ("resources/images/button18.png");
    images[20] = LoadTexture ("resources/images/button19.png");
    images[21] = LoadTexture ("resources/images/button20.png");
    yield ();

    // green button C - B
    images[22] = LoadTexture ("resources/images/button21.png");
    images[23] = LoadTexture ("resources/images/button22.png");
    images[24] = LoadTexture ("resources/images/button23.png");
    images[25] = LoadTexture ("resources/images/button24.png");
    images[26] = LoadTexture ("resources/images/button25.png");
    images[27] = LoadTexture ("resources/images/button26.png");
    images[28] = LoadTexture ("resources/images/button27.png");
    yield ();
    images[29] = LoadTexture ("resources/images/button28.png");
    images[30] = LoadTexture ("resources/images/button29.png");
    images[31] = LoadTexture ("resources/images/button30.png");
    images[32] = LoadTexture ("resources/images/button31.png");
    images[33] = LoadTexture ("resources/images/button32.png");
    yield ();

    // scale
    images[34] = LoadTexture ("resources/images/button33.png");
    images[35] = LoadTexture ("resources/images/button34.png");
    images[36] = LoadTexture ("resources/images/button35.png");
    images[37] = LoadTexture ("resources/images/button36.png");
    images[38] = LoadTexture ("resources/images/button37.png");
    images[39] = LoadTexture ("resources/images/button38.png");
    images[40] = LoadTexture ("resources/images/button39.png");
    yield ();

    music_file[0] = "resources/music/4AM.mp3";
    music_file[1] = "resources/music/Bohemian.mp3";
    music_file[2] = "resources/music/Duvet.mp3";
    music_file[3] = "resources/music/Just A Joke.mp3";
    yield ();
    music_file[4] = "resources/music/Machi No Dorufin.mp3";

    for (int i = 0; i < NO_OF_MUSIC; i++)
        {
            music[i] = LoadMusicStream (music_file[i]);
        }
    finish ();
    end;
    return 1;
}

int
LoadUi (void)
{
    /***************** HOME SCREEN UI ******************/

    begin;
    buttons[HOME_SCREEN][0]
        = CreateMenuButton (images[2], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[HOME_SCREEN][1]
        = CreateMenuButton (images[39], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[HOME_SCREEN][2]
        = CreateMenuButton (images[3], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[HOME_SCREEN][3]
        = CreateMenuButton (images[4], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[HOME_SCREEN][4]
        = CreateMenuButton (images[5], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    for (no_of_buttons[HOME_SCREEN] = 0;
         buttons[HOME_SCREEN][no_of_buttons[HOME_SCREEN]].is == true;
         no_of_buttons[HOME_SCREEN]++)
        ;
    yield ();

    /***************************************************/

    /***************** INTERVAL SCREEN UI ******************/
    buttons[INTERVAL_SCREEN][0]
        = CreateMenuButton (images[40], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    for (no_of_buttons[INTERVAL_SCREEN] = 0;
         buttons[INTERVAL_SCREEN][no_of_buttons[INTERVAL_SCREEN]].is == true;
         no_of_buttons[INTERVAL_SCREEN]++)
        ;

    yield ();
    /***************************************************/

    /***************** FREE SCREEN UI ******************/
    buttons[FREE_SCREEN][0] = buttons[INTERVAL_SCREEN][0];
    for (no_of_buttons[FREE_SCREEN] = 0;
         buttons[FREE_SCREEN][no_of_buttons[FREE_SCREEN]].is == true;
         no_of_buttons[FREE_SCREEN]++)
        ;

    /***************************************************/

    /***************** PAUSE SCREEN UI ******************/

    buttons[PAUSE_SCREEN][0]
        = CreateMenuButton (images[6], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 4.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[PAUSE_SCREEN][1]
        = CreateMenuButton (images[7], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[PAUSE_SCREEN][2]
        = CreateMenuButton (images[8], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[PAUSE_SCREEN][3]
        = CreateMenuButton (images[9], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    for (no_of_buttons[PAUSE_SCREEN] = 0;
         buttons[PAUSE_SCREEN][no_of_buttons[PAUSE_SCREEN]].is == true;
         no_of_buttons[PAUSE_SCREEN]++)
        ;
    yield ();
    /***************************************************/

    /***************** INTERVAL SETTING SCREEN UI ******************/

    // key button
    buttons[INTERVAL_SETTING_SCREEN][0]
        = CreateMenuButton (images[10], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[INTERVAL_SETTING_SCREEN][1]
        = CreateMenuButton (images[11], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[INTERVAL_SETTING_SCREEN][2]
        = CreateMenuButton (images[12], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[INTERVAL_SETTING_SCREEN][3]
        = CreateMenuButton (images[13], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[INTERVAL_SETTING_SCREEN][4]
        = CreateMenuButton (images[14], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[INTERVAL_SETTING_SCREEN][5]
        = CreateMenuButton (images[15], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[INTERVAL_SETTING_SCREEN][6]
        = CreateMenuButton (images[16], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[INTERVAL_SETTING_SCREEN][7]
        = CreateMenuButton (images[17], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[INTERVAL_SETTING_SCREEN][8]
        = CreateMenuButton (images[18], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[INTERVAL_SETTING_SCREEN][9]
        = CreateMenuButton (images[19], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[INTERVAL_SETTING_SCREEN][10]
        = CreateMenuButton (images[20], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[INTERVAL_SETTING_SCREEN][11]
        = CreateMenuButton (images[21], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    yield ();

    // scale button
    buttons[INTERVAL_SETTING_SCREEN][12]
        = CreateMenuButton (images[34], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });
    buttons[INTERVAL_SETTING_SCREEN][13]
        = CreateMenuButton (images[35], "", GREEN,
                            (Rectangle){ .width = 600 / 2.0,
                                         .height = 220 / 2.0,
                                         .x = GetRenderWidth () * 3 / 4.0,
                                         .y = 200 });

    for (no_of_buttons[INTERVAL_SETTING_SCREEN] = 0;
         buttons[INTERVAL_SETTING_SCREEN]
                [no_of_buttons[INTERVAL_SETTING_SCREEN]]
                    .is
         == true;
         no_of_buttons[INTERVAL_SETTING_SCREEN]++)
        ;
    yield ();
    finish ();
    end;
    return 1;
    /***************************************************/
}

void
ChangeToScreen (enum Screen screen)
{
    current_screen = screen;
}
void
UpdateTransition (void)
{
    switch (former_screen)
        {
        case HOME_SCREEN:
            switch (current_screen)
                {
                case INTERVAL_SETTING_SCREEN:
                    UpdateHomeToIntSetting ();
                    break;
                case FREE_SCREEN:
                    camera2d.offset = (Vector2){ 0, 0 };
                    UpdateHomeToFree ();
                    break;
                default:
                    break;
                }

            break;
        case INTERVAL_SETTING_SCREEN:
            switch (current_screen)
                {
                case INTERVAL_SCREEN:
                    camera2d.offset = (Vector2){ 0, 0 };
                    UpdateIntSettingToInt ();
                    break;
                case CHORD_SCREEN:
                    camera2d.offset = (Vector2){ 0, 0 };
                    UpdateIntSettingToChord ();
                    break;

                default:
                    break;
                }
            break;
        case PAUSE_SCREEN:
            switch (current_screen)
                {
                case INTERVAL_SCREEN:
                    UpdatePauseToInt ();
                    break;
                case FREE_SCREEN:
                    UpdatePauseToFree ();
                    break;
                case HOME_SCREEN:
                    UpdatePauseToHome ();
                    break;
                default:
                    break;
                }
            break;
        case INTERVAL_SCREEN:
            switch (current_screen)
                {
                case PAUSE_SCREEN:
                    UpdateIntToPause ();
                    break;
                default:
                    break;
                }
            break;
        case FREE_SCREEN:
            switch (current_screen)
                {
                case PAUSE_SCREEN:
                    UpdateFreeToPause ();
                    break;
                default:
                    break;
                }
            break;
        default:
            break;
        }
}
void
DrawTransition (void)
{
    switch (former_screen)
        {
        case HOME_SCREEN:
            switch (current_screen)
                {
                case INTERVAL_SETTING_SCREEN:
                    DrawHomeToIntSetting ();
                    break;
                case FREE_SCREEN:
                    DrawFreeScreen ();
                    break;
                default:
                    break;
                }
            break;
        case INTERVAL_SETTING_SCREEN:
            switch (current_screen)
                {
                case INTERVAL_SCREEN:
                    DrawIntervalScreen ();
                    break;
                case CHORD_SCREEN:
                    DrawChordScreen ();
                    break;

                default:
                    break;
                }
            break;
        case PAUSE_SCREEN:
            switch (current_screen)
                {
                case INTERVAL_SCREEN:
                    DrawPauseToInt ();
                    break;
                case FREE_SCREEN:
                    DrawPauseToFree ();
                    break;
                case HOME_SCREEN:
                    DrawPauseToHome ();
                    break;

                default:
                    break;
                }
            break;
        case INTERVAL_SCREEN:
            switch (current_screen)
                {
                case PAUSE_SCREEN:
                    DrawIntToPause ();
                    break;
                default:
                    break;
                }
            break;
        case FREE_SCREEN:
            switch (current_screen)
                {
                case PAUSE_SCREEN:
                    DrawFreeToPause ();
                    break;
                default:
                    break;
                }
            break;
        default:
            break;
        }
}

void
TransitionToScreen (enum Screen screen)
{
    screen_transition = true;
    former_screen = current_screen;
    looped_once = false;
    current_screen = screen;
}
