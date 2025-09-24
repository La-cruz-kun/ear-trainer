#include "ear_trainer.h"
#include <raylib.h>
#include <rlgl.h>
#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>
#include <raymath.h>
#include <stdio.h>
#include <plug.h>

void DrawBoundingBoxAsCube(BoundingBox box, Color color);
void MapPianoNotes(void);
void LoadPianoModel(void);
void DrawPiano(void);
void DrawPlaneModel(void);
void LoadPlaneModel(void);
void LoadUi(Plug *plug);
void LoadResources(Plug *plug);
void LoadSoundInstrument(void);

typedef struct {
    KeyboardKey key;
    int value;
} KeyboardHashMap;


BoundingBox keyBoxes[NO_OF_NOTES] = {0};
Sound sound[NO_OF_NOTES] = {0};
bool key_pressed = false;
KeyboardHashMap *Key_to_note = NULL;
int note_pool[POOL_SIZE] = {-1, -1, -1, -1};
unsigned int currentOctave = 0;




Vector3 lightPosition = (Vector3) {
    2, 4, 2
};
Vector3 lightAmbient = (Vector3) {
    0.2f, 0.2f, 0.2f
};
Vector3 lightDiffuse = (Vector3) {
    5.0f, 5.0f, 5.0f
}; // Bright light
Vector3 lightSpecular = (Vector3) {
    1.0f, 1.0f, 1.0f
};
Model piano_model = {0};
Model plane_model = {0};
Model lightCube;
float lightConstant = 1.0f;
float lightLinear = 0.09f;
float lightQuadratic = 0.032f;
Shader pbrShader;
Shader planeShader;
Shader lightShader;
int lightPosLoc;
int lightAmbLoc;
int lightDiffLoc;
int lightSpecLoc;
int lightConstLoc;
int lightLinearLoc;
int lightQuadraticLoc;

int viewPosLoc;
Texture2D pianoAlbedo = { 0 };
Texture2D pianoMetallicRoughness = { 0 };
Texture2D pianoNormal = { 0 };

Texture2D planeAlbedo = { 0 };
Texture2D planeRoughness = { 0 };
Texture2D planeNormal = { 0 };
Texture2D planeMetallic = {0};
Texture2D planeAO = {0};

// Material 1: Bench
Texture2D benchAlbedo = { 0 };
Texture2D benchMetallicRoughness = { 0 };
Texture2D benchNormal = { 0 };


void plug_init(Plug *plug) {
    plug->camera.position = (Vector3) {
        0, 2, 2
    };
    plug->camera.target = (Vector3) {
        0,0,-0.2
    };
    plug->camera.up = (Vector3) {
        0, 1, 0
    };
    plug->camera.fovy = 45;
    plug->camera.projection = CAMERA_PERSPECTIVE;
    hmdefault(Key_to_note, -1);
    hmput(Key_to_note, KEY_A, C1);
    hmput(Key_to_note, KEY_S, D1);
    hmput(Key_to_note, KEY_D, E1);
    hmput(Key_to_note, KEY_F, F1);
    hmput(Key_to_note, KEY_G, G1);
    hmput(Key_to_note, KEY_H, A1);
    hmput(Key_to_note, KEY_J, B1);
    hmput(Key_to_note, KEY_K, C2);
    hmput(Key_to_note, KEY_L, D2);
    hmput(Key_to_note, KEY_SEMICOLON, E2);
    hmput(Key_to_note, KEY_APOSTROPHE, F2);
    hmput(Key_to_note, KEY_W, CS1);
    hmput(Key_to_note, KEY_E, DS1);
    hmput(Key_to_note, KEY_T, FS1);
    hmput(Key_to_note, KEY_Y, GS1);
    hmput(Key_to_note, KEY_U, AS1);
    hmput(Key_to_note, KEY_O, CS2);
    hmput(Key_to_note, KEY_P, DS2);

    LoadResources(plug);

    plug->screen = HOME_SCREEN;
    plug->current_music_index = GetRandomValue(0, NO_OF_MUSIC - 1);

    LoadUi(plug);

    LoadPianoModel();
    LoadPlaneModel();
    LoadSoundInstrument();

    MapPianoNotes();


    SetTargetFPS(60);
    PlayMusicStream(plug->music[plug->current_music_index]);

}
void plug_update(Plug *plug) {
    switch(plug->screen) {
    case HOME_SCREEN:
        if (!IsMusicStreamPlaying(plug->music[plug->current_music_index]))
            PlayMusicStream(plug->music[plug->current_music_index]);
        UpdateMusicStream(plug->music[plug->current_music_index]);
        for (int i = 0; i < plug->no_of_buttons[HOME_SCREEN]; i++) {
            float height = plug->images[2].height * GetRenderHeight() / 2000.0;
            float padding = GetRenderHeight() / 20.0;
            plug->buttons[HOME_SCREEN][i].bound = (Rectangle) {
                .width = plug->images[2].width * GetRenderHeight()/2000.0,
                .height = height,
                .x = GetRenderWidth() * 3/4.0,
                .y = (float)GetRenderHeight()/(plug->no_of_buttons[HOME_SCREEN]+1) - height/2 + (i*(height + padding))
            };
        }
        if (IsMenuButtonPressed(&plug->buttons[HOME_SCREEN][0])) {
            plug->screen = INTERVAL_SCREEN;
        }
        if (IsMenuButtonPressed(&plug->buttons[HOME_SCREEN][1])) {}
        if (IsMenuButtonPressed(&plug->buttons[HOME_SCREEN][2])) {}
        if (IsMenuButtonPressed(&plug->buttons[HOME_SCREEN][3])) {
            plug->quit = true;
        }
        calcMenuButtonPosition(&plug->buttons[HOME_SCREEN][0], &plug->buttons[HOME_SCREEN][1], &plug->buttons[HOME_SCREEN][2], &plug->buttons[HOME_SCREEN][3], NULL);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawBackgroundImage(plug->images[0]);
        for (int i = 0; i < plug->no_of_buttons[HOME_SCREEN]; i++)
            DrawMenuButton(plug->buttons[HOME_SCREEN][i]);
        /* printf("%d\n", plug->no_of_buttons[HOME_SCREEN]); */
        DrawFPS(10, 10);
        EndDrawing();
        break;

    case INTERVAL_SCREEN:
    {
        plug->time = GetTime();
        int key;
        while ((key = GetKeyPressed()) != 0) {
            if (hmget(Key_to_note, key) != -1) {
                int note = hmget(Key_to_note, key) + currentOctave * OCTAVE;
                for (int i = 0; i < 4; i++) {
                    if (note_pool[i] == -1) {
                        note_pool[i] = note;
                        PlaySound(sound[note]);
                        break;
                    }
                }
                continue;
            }
            if ( key == KEY_RIGHT_SHIFT) {
                if (currentOctave >=7)
                    continue;
                currentOctave++;
                continue;
            }
            if (key == KEY_LEFT_SHIFT) {
                if (currentOctave <= 0)
                    continue;
                currentOctave--;
                continue;
            }

        }
        for (int i = 0; i < hmlen(Key_to_note); i++) {
            KeyboardKey key = Key_to_note[i].key;
            if (IsKeyReleased(key)) {
                int note = hmget(Key_to_note, key) + currentOctave * OCTAVE;
                for (int i = 0; i < 4; i++) {
                    if (note_pool[i] == note) {
                        StopSound(sound[note]);
                        note_pool[i] = -1;
                    }
                }
            }
        }
        lightPosition.x = cosf(plug->time) * 3;
        lightPosition.z = sinf(plug->time) * 3;
        lightPosition.y = 2 + sinf(plug->time * 0.5f) * 1;

        // Update light position and view position
        SetShaderValue(pbrShader, lightPosLoc, &lightPosition, SHADER_UNIFORM_VEC3);
        SetShaderValue(pbrShader, viewPosLoc, &plug->camera.position, SHADER_UNIFORM_VEC3);
        SetShaderValue(planeShader, lightPosLoc, &lightPosition, SHADER_UNIFORM_VEC3);
        SetShaderValue(planeShader, viewPosLoc, &plug->camera.position, SHADER_UNIFORM_VEC3);
        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(plug->camera);

        DrawPlaneModel();
        DrawPiano();

        // Draw light cube
        DrawModel(lightCube, lightPosition, 0.5f, YELLOW);
        for (int i = 0; i < 4; i++) {
            if (note_pool[i] > -1) {
                DrawBoundingBoxAsCube(keyBoxes[note_pool[i]], BLUE);
            }
        }
        /* DrawGrid(10, 1); */
        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();
        break;
    }
    default:
        break;

    }

}
void MapPianoNotes(void) {

    keyBoxes[GS1] = GetMeshBoundingBox(piano_model.meshes[2]);
    keyBoxes[AS1] = GetMeshBoundingBox(piano_model.meshes[3]);
    keyBoxes[FS1] = GetMeshBoundingBox(piano_model.meshes[4]);
    keyBoxes[FS2] = GetMeshBoundingBox(piano_model.meshes[5]);
    keyBoxes[GS2] = GetMeshBoundingBox(piano_model.meshes[6]);
    keyBoxes[AS2] = GetMeshBoundingBox(piano_model.meshes[7]);
    keyBoxes[FS3] = GetMeshBoundingBox(piano_model.meshes[8]);
    keyBoxes[GS3] = GetMeshBoundingBox(piano_model.meshes[9]);
    keyBoxes[AS3] = GetMeshBoundingBox(piano_model.meshes[10]);
    keyBoxes[FS4] = GetMeshBoundingBox(piano_model.meshes[11]);
    keyBoxes[GS4] = GetMeshBoundingBox(piano_model.meshes[12]);
    keyBoxes[AS4] = GetMeshBoundingBox(piano_model.meshes[13]);
    keyBoxes[FS5] = GetMeshBoundingBox(piano_model.meshes[14]);
    keyBoxes[GS5] = GetMeshBoundingBox(piano_model.meshes[15]);
    keyBoxes[AS5] = GetMeshBoundingBox(piano_model.meshes[16]);
    keyBoxes[FS6] = GetMeshBoundingBox(piano_model.meshes[17]);
    keyBoxes[GS6] = GetMeshBoundingBox(piano_model.meshes[18]);
    keyBoxes[AS6] = GetMeshBoundingBox(piano_model.meshes[19]);
    keyBoxes[FS7] = GetMeshBoundingBox(piano_model.meshes[20]);
    keyBoxes[GS7] = GetMeshBoundingBox(piano_model.meshes[21]);
    keyBoxes[AS7] = GetMeshBoundingBox(piano_model.meshes[22]);
    keyBoxes[CS1] = GetMeshBoundingBox(piano_model.meshes[24]);
    keyBoxes[DS1] = GetMeshBoundingBox(piano_model.meshes[25]);
    keyBoxes[CS2] = GetMeshBoundingBox(piano_model.meshes[26]);
    keyBoxes[DS2] = GetMeshBoundingBox(piano_model.meshes[27]);
    keyBoxes[CS3] = GetMeshBoundingBox(piano_model.meshes[28]);
    keyBoxes[DS3] = GetMeshBoundingBox(piano_model.meshes[29]);
    keyBoxes[CS4] = GetMeshBoundingBox(piano_model.meshes[30]);
    keyBoxes[DS4] = GetMeshBoundingBox(piano_model.meshes[31]);
    keyBoxes[CS5] = GetMeshBoundingBox(piano_model.meshes[32]);
    keyBoxes[DS5] = GetMeshBoundingBox(piano_model.meshes[33]);
    keyBoxes[CS6] = GetMeshBoundingBox(piano_model.meshes[34]);

    keyBoxes[DS6] = GetMeshBoundingBox(piano_model.meshes[35]);
    keyBoxes[CS7] = GetMeshBoundingBox(piano_model.meshes[36]);
    keyBoxes[DS7] = GetMeshBoundingBox(piano_model.meshes[37]);
    keyBoxes[C1]  = GetMeshBoundingBox(piano_model.meshes[40]);
    keyBoxes[D1]  = GetMeshBoundingBox(piano_model.meshes[41]);
    keyBoxes[E1]  = GetMeshBoundingBox(piano_model.meshes[42]);
    keyBoxes[F1]  = GetMeshBoundingBox(piano_model.meshes[43]);
    keyBoxes[G1]  = GetMeshBoundingBox(piano_model.meshes[44]);
    keyBoxes[A1]  = GetMeshBoundingBox(piano_model.meshes[45]);
    keyBoxes[B1]  = GetMeshBoundingBox(piano_model.meshes[46]);
    keyBoxes[C2]  = GetMeshBoundingBox(piano_model.meshes[47]);
    keyBoxes[D2]  = GetMeshBoundingBox(piano_model.meshes[48]);
    keyBoxes[E2]  = GetMeshBoundingBox(piano_model.meshes[49]);
    keyBoxes[F2]  = GetMeshBoundingBox(piano_model.meshes[50]);
    keyBoxes[G2]  = GetMeshBoundingBox(piano_model.meshes[51]);
    keyBoxes[A2]  = GetMeshBoundingBox(piano_model.meshes[52]);
    keyBoxes[B2]  = GetMeshBoundingBox(piano_model.meshes[53]);
    keyBoxes[C3]  = GetMeshBoundingBox(piano_model.meshes[54]);
    keyBoxes[D3]  = GetMeshBoundingBox(piano_model.meshes[55]);
    keyBoxes[E3]  = GetMeshBoundingBox(piano_model.meshes[56]);
    keyBoxes[F3]  = GetMeshBoundingBox(piano_model.meshes[57]);
    keyBoxes[G3]  = GetMeshBoundingBox(piano_model.meshes[58]);
    keyBoxes[A3]  = GetMeshBoundingBox(piano_model.meshes[59]);
    keyBoxes[B3]  = GetMeshBoundingBox(piano_model.meshes[60]);
    keyBoxes[C4]  = GetMeshBoundingBox(piano_model.meshes[61]);


    keyBoxes[D4] = GetMeshBoundingBox(piano_model.meshes[62]);
    keyBoxes[E4] = GetMeshBoundingBox(piano_model.meshes[63]);
    keyBoxes[F4] = GetMeshBoundingBox(piano_model.meshes[64]);
    keyBoxes[G4] = GetMeshBoundingBox(piano_model.meshes[65]);
    keyBoxes[A4] = GetMeshBoundingBox(piano_model.meshes[66]);
    keyBoxes[B4] = GetMeshBoundingBox(piano_model.meshes[67]);
    keyBoxes[C5] = GetMeshBoundingBox(piano_model.meshes[68]);
    keyBoxes[D5] = GetMeshBoundingBox(piano_model.meshes[69]);
    keyBoxes[E5] = GetMeshBoundingBox(piano_model.meshes[70]);
    keyBoxes[F5] = GetMeshBoundingBox(piano_model.meshes[71]);
    keyBoxes[G5] = GetMeshBoundingBox(piano_model.meshes[72]);
    keyBoxes[A5] = GetMeshBoundingBox(piano_model.meshes[73]);
    keyBoxes[B5] = GetMeshBoundingBox(piano_model.meshes[74]);
    keyBoxes[C6] = GetMeshBoundingBox(piano_model.meshes[75]);
    keyBoxes[D6] = GetMeshBoundingBox(piano_model.meshes[76]);
    keyBoxes[E6] = GetMeshBoundingBox(piano_model.meshes[77]);
    keyBoxes[F6] = GetMeshBoundingBox(piano_model.meshes[78]);
    keyBoxes[G6] = GetMeshBoundingBox(piano_model.meshes[79]);
    keyBoxes[A6] = GetMeshBoundingBox(piano_model.meshes[80]);
    keyBoxes[B6] = GetMeshBoundingBox(piano_model.meshes[81]);
    keyBoxes[C7] = GetMeshBoundingBox(piano_model.meshes[82]);
    keyBoxes[D7] = GetMeshBoundingBox(piano_model.meshes[83]);
    keyBoxes[E7] = GetMeshBoundingBox(piano_model.meshes[84]);
    keyBoxes[F7] = GetMeshBoundingBox(piano_model.meshes[85]);
    keyBoxes[G7] = GetMeshBoundingBox(piano_model.meshes[86]);
    keyBoxes[A7] = GetMeshBoundingBox(piano_model.meshes[87]);
    keyBoxes[B7] = GetMeshBoundingBox(piano_model.meshes[88]);
    keyBoxes[C8] = GetMeshBoundingBox(piano_model.meshes[89]);
}

void DrawBoundingBoxAsCube(BoundingBox box, Color color)
{
    // Find center
    //
    float margin = 0.001;
    box.min = (Vector3) {
        box.min.x - margin, box.min.y - margin, box.min.z - margin
    };
    box.max = (Vector3) {
        box.max.x + margin, box.max.y + margin, box.max.z + margin
    };
    Vector3 center = {
        (box.min.x + box.max.x) * 0.5f,
        (box.min.y + box.max.y) * 0.5f,
        (box.min.z + box.max.z) * 0.5f
    };

    // Find size (extent along each axis)
    Vector3 size = {
        (box.max.x - box.min.x),
        (box.max.y - box.min.y),
        (box.max.z - box.min.z)
    };

    // Draw wireframe cube for clarity
    /* DrawCubeWires(center, size.x, size.y, size.z, color); */

    // Or if you want it filled:
    DrawCube(center, size.x, size.y, size.z, color);
}



void LoadPlaneModel(void) {
    plane_model = LoadModelFromMesh(GenMeshPlane(8, 8, 1, 1));

    planeShader = LoadShader("resources/shaders/main_vs.glsl", "resources/shaders/plane_fs.glsl");


    // Texture uniform locations
    int matDiffLoc = GetShaderLocation(planeShader, "material.diffuse");
    int matRoughnessLoc = GetShaderLocation(planeShader, "material.roughness");
    int matNormalLoc = GetShaderLocation(planeShader, "material.normal");
    int matMetallicLoc = GetShaderLocation(planeShader, "material.metallic");
    int matAOLoc = GetShaderLocation(planeShader, "material.ao");
    int tileLoc = GetShaderLocation(planeShader, "tile");

    // Set texture units
    int diffuseUnit = 0;
    int roughnessUnit = 1;
    int normalUnit = 2;
    int metallicUnit = 3;
    int aoUnit = 4;
    Vector2 tile = (Vector2) {
        2.5, 2.5
    };
    SetShaderValue(planeShader, matDiffLoc, &diffuseUnit, SHADER_UNIFORM_INT);
    SetShaderValue(planeShader, matRoughnessLoc, &roughnessUnit, SHADER_UNIFORM_INT);
    SetShaderValue(planeShader, matMetallicLoc, &metallicUnit, SHADER_UNIFORM_INT);
    SetShaderValue(planeShader, matAOLoc, &aoUnit, SHADER_UNIFORM_INT);
    SetShaderValue(planeShader, matNormalLoc, &normalUnit, SHADER_UNIFORM_INT);
    SetShaderValue(planeShader, tileLoc, &tile, SHADER_UNIFORM_VEC2);

    printf("Number of materials: %d\n", plane_model.materialCount);
    printf("Number of meshes: %d\n", plane_model.meshCount);


    // Set light uniforms
    SetShaderValue(planeShader, lightPosLoc, &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue(planeShader, lightAmbLoc, &lightAmbient, SHADER_UNIFORM_VEC3);
    SetShaderValue(planeShader, lightDiffLoc, &lightDiffuse, SHADER_UNIFORM_VEC3);
    SetShaderValue(planeShader, lightSpecLoc, &lightSpecular, SHADER_UNIFORM_VEC3);
    SetShaderValue(planeShader, lightConstLoc, &lightConstant, SHADER_UNIFORM_FLOAT);
    SetShaderValue(planeShader, lightLinearLoc, &lightLinear, SHADER_UNIFORM_FLOAT);
    SetShaderValue(planeShader, lightQuadraticLoc, &lightQuadratic, SHADER_UNIFORM_FLOAT);

    // Prepare textures for both materials
    // Material 0: Piano body

    // Extract textures from materials if they exist
    planeAlbedo = LoadTexture("resources/textures/stoneBrick/COL.jpg");
    planeRoughness = LoadTexture("resources/textures/stoneBrick/ROUGHNESS.jpg");
    planeMetallic = LoadTexture("resources/textures/stoneBrick/METALLIC.jpg");
    planeAO = LoadTexture("resources/textures/stoneBrick/AO.jpg");
    planeNormal = LoadTexture("resources/textures/stoneBrick/NORMAL.png");
    plane_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = planeAlbedo;
    plane_model.materials[0].maps[MATERIAL_MAP_ROUGHNESS].texture = planeRoughness;
    plane_model.materials[0].maps[MATERIAL_MAP_METALNESS].texture = planeMetallic;
    plane_model.materials[0].maps[MATERIAL_MAP_OCCLUSION].texture = planeAO;
    plane_model.materials[0].maps[MATERIAL_MAP_NORMAL].texture = planeNormal;

    // Assign shader to material
    plane_model.materials[0].shader = planeShader;
}




void LoadPianoModel(void) {
    piano_model = LoadModel("resources/models/edited.gltf");

    pbrShader = LoadShader("resources/shaders/main_vs.glsl", "resources/shaders/main_fs.glsl");
    lightShader = LoadShader("resources/shaders/main_vs.glsl", "resources/shaders/light.glsl");

    // Load models
    lightCube = LoadModelFromMesh(GenMeshCube(0.5f, 0.5f, 0.5f));

    // Get shader locations
    lightPosLoc = GetShaderLocation(pbrShader, "light.position");
    lightAmbLoc = GetShaderLocation(pbrShader, "light.ambient");
    lightDiffLoc = GetShaderLocation(pbrShader, "light.diffuse");
    lightSpecLoc = GetShaderLocation(pbrShader, "light.specular");
    lightConstLoc = GetShaderLocation(pbrShader, "light.constant");
    lightLinearLoc = GetShaderLocation(pbrShader, "light.linear");
    lightQuadraticLoc = GetShaderLocation(pbrShader, "light.quadratic");

    viewPosLoc = GetShaderLocation(pbrShader, "viewPos");

    // Texture uniform locations
    int matDiffLoc = GetShaderLocation(pbrShader, "material.diffuse");
    int matMetallicRoughnessLoc = GetShaderLocation(pbrShader, "material.metallicRoughness");
    int matNormalLoc = GetShaderLocation(pbrShader, "material.normal");

    // Set texture units
    int diffuseUnit = 0;
    int metallicRoughnessUnit = 1;
    int normalUnit = 2;
    SetShaderValue(pbrShader, matDiffLoc, &diffuseUnit, SHADER_UNIFORM_INT);
    SetShaderValue(pbrShader, matMetallicRoughnessLoc, &metallicRoughnessUnit, SHADER_UNIFORM_INT);
    SetShaderValue(pbrShader, matNormalLoc, &normalUnit, SHADER_UNIFORM_INT);

    // Check how many materials we have
    printf("Number of materials: %d\n", piano_model.materialCount);
    printf("Number of meshes: %d\n", piano_model.meshCount);

    // Set up light properties

    // Set light uniforms
    SetShaderValue(pbrShader, lightPosLoc, &lightPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue(pbrShader, lightAmbLoc, &lightAmbient, SHADER_UNIFORM_VEC3);
    SetShaderValue(pbrShader, lightDiffLoc, &lightDiffuse, SHADER_UNIFORM_VEC3);
    SetShaderValue(pbrShader, lightSpecLoc, &lightSpecular, SHADER_UNIFORM_VEC3);
    SetShaderValue(pbrShader, lightConstLoc, &lightConstant, SHADER_UNIFORM_FLOAT);
    SetShaderValue(pbrShader, lightLinearLoc, &lightLinear, SHADER_UNIFORM_FLOAT);
    SetShaderValue(pbrShader, lightQuadraticLoc, &lightQuadratic, SHADER_UNIFORM_FLOAT);

    // Prepare textures for both materials
    // Material 0: Piano body

    // Extract textures from materials if they exist
    if (piano_model.materialCount > 1) {
        // First material (index 0)
        pianoAlbedo = piano_model.materials[1].maps[MATERIAL_MAP_DIFFUSE].texture;
        pianoMetallicRoughness = piano_model.materials[1].maps[MATERIAL_MAP_METALNESS].texture;
        pianoNormal = piano_model.materials[1].maps[MATERIAL_MAP_NORMAL].texture;

        // Assign shader to material
        piano_model.materials[1].shader = pbrShader;
    }

    if (piano_model.materialCount > 2) {
        // Second material (index 1)
        benchAlbedo = piano_model.materials[2].maps[MATERIAL_MAP_DIFFUSE].texture;
        benchMetallicRoughness = piano_model.materials[2].maps[MATERIAL_MAP_METALNESS].texture;
        benchNormal = piano_model.materials[2].maps[MATERIAL_MAP_NORMAL].texture;

        // Assign shader to material
        piano_model.materials[2].shader = pbrShader;
    }

    // Create default textures if needed
    if (pianoAlbedo.id == 0) {
        Image img = GenImageColor(1, 1, WHITE);
        pianoAlbedo = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    if (pianoMetallicRoughness.id == 0) {
        Image img = GenImageColor(1, 1, (Color) {
            0, 128, 255, 255
        }); // Default roughness and metallic
        pianoMetallicRoughness = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    if (pianoNormal.id == 0) {
        Image img = GenImageColor(1, 1, (Color) {
            128, 128, 255, 255
        }); // Default normal (pointing up)
        pianoNormal = LoadTextureFromImage(img);
        UnloadImage(img);
    }
}

void DrawPiano(void) {
    for (int i = 0; i < piano_model.meshCount; i++) {
        // Get material index for this mesh
        int materialIndex = 0;
        if (piano_model.meshMaterial != NULL) {
            materialIndex = piano_model.meshMaterial[i];
        }

        // Bind appropriate textures based on material
        if (materialIndex == 1) {
            // Piano material
            rlActiveTextureSlot(0);
            rlEnableTexture(pianoAlbedo.id);
            rlActiveTextureSlot(1);
            rlEnableTexture(pianoMetallicRoughness.id);
            rlActiveTextureSlot(2);
            rlEnableTexture(pianoNormal.id);
        } else if (materialIndex == 2) {
            // Bench material
            rlActiveTextureSlot(0);
            rlEnableTexture(benchAlbedo.id);
            rlActiveTextureSlot(1);
            rlEnableTexture(benchMetallicRoughness.id);
            rlActiveTextureSlot(2);
            rlEnableTexture(benchNormal.id);
        }

        // Draw mesh with the PBR shader
        if (materialIndex < piano_model.materialCount) {
            DrawMesh(piano_model.meshes[i], piano_model.materials[materialIndex], MatrixIdentity());
        }
    }
}

void DrawPlaneModel(void) {
    rlActiveTextureSlot(0);
    rlEnableTexture(planeAlbedo.id);
    rlActiveTextureSlot(1);
    rlEnableTexture(planeRoughness.id);
    rlActiveTextureSlot(2);
    rlEnableTexture(planeNormal.id);
    rlActiveTextureSlot(3);
    rlEnableTexture(planeMetallic.id);
    rlActiveTextureSlot(4);
    rlEnableTexture(planeAO.id);
    DrawMesh(plane_model.meshes[0], plane_model.materials[0], MatrixIdentity());

}

void LoadSoundInstrument(void) {
// Octave 1
    sound[C1]  = LoadSound("resources/instrument/piano/1-c.mp3");
    sound[CS1] = LoadSound("resources/instrument/piano/1-cs.mp3");
    sound[D1]  = LoadSound("resources/instrument/piano/1-d.mp3");
    sound[DS1] = LoadSound("resources/instrument/piano/1-ds.mp3");
    sound[E1]  = LoadSound("resources/instrument/piano/1-e.mp3");
    sound[F1]  = LoadSound("resources/instrument/piano/1-f.mp3");
    sound[FS1] = LoadSound("resources/instrument/piano/1-fs.mp3");
    sound[G1]  = LoadSound("resources/instrument/piano/1-g.mp3");
    sound[GS1] = LoadSound("resources/instrument/piano/1-gs.mp3");
    sound[A1]  = LoadSound("resources/instrument/piano/1-a.mp3");
    sound[AS1] = LoadSound("resources/instrument/piano/1-as.mp3");
    sound[B1]  = LoadSound("resources/instrument/piano/1-b.mp3");

// Octave 2
    sound[C2]  = LoadSound("resources/instrument/piano/2-c.mp3");
    sound[CS2] = LoadSound("resources/instrument/piano/2-cs.mp3");
    sound[D2]  = LoadSound("resources/instrument/piano/2-d.mp3");
    sound[DS2] = LoadSound("resources/instrument/piano/2-ds.mp3");
    sound[E2]  = LoadSound("resources/instrument/piano/2-e.mp3");
    sound[F2]  = LoadSound("resources/instrument/piano/2-f.mp3");
    sound[FS2] = LoadSound("resources/instrument/piano/2-fs.mp3");
    sound[G2]  = LoadSound("resources/instrument/piano/2-g.mp3");
    sound[GS2] = LoadSound("resources/instrument/piano/2-gs.mp3");
    sound[A2]  = LoadSound("resources/instrument/piano/2-a.mp3");
    sound[AS2] = LoadSound("resources/instrument/piano/2-as.mp3");
    sound[B2]  = LoadSound("resources/instrument/piano/2-b.mp3");

// Octave 3
    sound[C3]  = LoadSound("resources/instrument/piano/3-c.mp3");
    sound[CS3] = LoadSound("resources/instrument/piano/3-cs.mp3");
    sound[D3]  = LoadSound("resources/instrument/piano/3-d.mp3");
    sound[DS3] = LoadSound("resources/instrument/piano/3-ds.mp3");
    sound[E3]  = LoadSound("resources/instrument/piano/3-e.mp3");
    sound[F3]  = LoadSound("resources/instrument/piano/3-f.mp3");
    sound[FS3] = LoadSound("resources/instrument/piano/3-fs.mp3");
    sound[G3]  = LoadSound("resources/instrument/piano/3-g.mp3");
    sound[GS3] = LoadSound("resources/instrument/piano/3-gs.mp3");
    sound[A3]  = LoadSound("resources/instrument/piano/3-a.mp3");
    sound[AS3] = LoadSound("resources/instrument/piano/3-as.mp3");
    sound[B3]  = LoadSound("resources/instrument/piano/3-b.mp3");

// Octave 4
    sound[C4]  = LoadSound("resources/instrument/piano/4-c.mp3");
    sound[CS4] = LoadSound("resources/instrument/piano/4-cs.mp3");
    sound[D4]  = LoadSound("resources/instrument/piano/4-d.mp3");
    sound[DS4] = LoadSound("resources/instrument/piano/4-ds.mp3");
    sound[E4]  = LoadSound("resources/instrument/piano/4-e.mp3");
    sound[F4]  = LoadSound("resources/instrument/piano/4-f.mp3");
    sound[FS4] = LoadSound("resources/instrument/piano/4-fs.mp3");
    sound[G4]  = LoadSound("resources/instrument/piano/4-g.mp3");
    sound[GS4] = LoadSound("resources/instrument/piano/4-gs.mp3");
    sound[A4]  = LoadSound("resources/instrument/piano/4-a.mp3");
    sound[AS4] = LoadSound("resources/instrument/piano/4-as.mp3");
    sound[B4]  = LoadSound("resources/instrument/piano/4-b.mp3");

// Octave 5
    sound[C5]  = LoadSound("resources/instrument/piano/5-c.mp3");
    sound[CS5] = LoadSound("resources/instrument/piano/5-cs.mp3");
    sound[D5]  = LoadSound("resources/instrument/piano/5-d.mp3");
    sound[DS5] = LoadSound("resources/instrument/piano/5-ds.mp3");
    sound[E5]  = LoadSound("resources/instrument/piano/5-e.mp3");
    sound[F5]  = LoadSound("resources/instrument/piano/5-f.mp3");
    sound[FS5] = LoadSound("resources/instrument/piano/5-fs.mp3");
    sound[G5]  = LoadSound("resources/instrument/piano/5-g.mp3");
    sound[GS5] = LoadSound("resources/instrument/piano/5-gs.mp3");
    sound[A5]  = LoadSound("resources/instrument/piano/5-a.mp3");
    sound[AS5] = LoadSound("resources/instrument/piano/5-as.mp3");
    sound[B5]  = LoadSound("resources/instrument/piano/5-b.mp3");

// Octave 6
    sound[C6]  = LoadSound("resources/instrument/piano/6-c.mp3");
    sound[CS6] = LoadSound("resources/instrument/piano/6-cs.mp3");
    sound[D6]  = LoadSound("resources/instrument/piano/6-d.mp3");
    sound[DS6] = LoadSound("resources/instrument/piano/6-ds.mp3");
    sound[E6]  = LoadSound("resources/instrument/piano/6-e.mp3");
    sound[F6]  = LoadSound("resources/instrument/piano/6-f.mp3");
    sound[FS6] = LoadSound("resources/instrument/piano/6-fs.mp3");
    sound[G6]  = LoadSound("resources/instrument/piano/6-g.mp3");
    sound[GS6] = LoadSound("resources/instrument/piano/6-gs.mp3");
    sound[A6]  = LoadSound("resources/instrument/piano/6-a.mp3");
    sound[AS6] = LoadSound("resources/instrument/piano/6-as.mp3");
    sound[B6]  = LoadSound("resources/instrument/piano/6-b.mp3");

// Octave 7
    sound[C7]  = LoadSound("resources/instrument/piano/7-c.mp3");
    sound[CS7] = LoadSound("resources/instrument/piano/7-cs.mp3");
    sound[D7]  = LoadSound("resources/instrument/piano/7-d.mp3");
    sound[DS7] = LoadSound("resources/instrument/piano/7-ds.mp3");
    sound[E7]  = LoadSound("resources/instrument/piano/7-e.mp3");
    sound[F7]  = LoadSound("resources/instrument/piano/7-f.mp3");
    sound[FS7] = LoadSound("resources/instrument/piano/7-fs.mp3");
    sound[G7]  = LoadSound("resources/instrument/piano/7-g.mp3");
    sound[GS7] = LoadSound("resources/instrument/piano/7-gs.mp3");
    sound[A7]  = LoadSound("resources/instrument/piano/7-a.mp3");
    sound[AS7] = LoadSound("resources/instrument/piano/7-as.mp3");
    sound[B7]  = LoadSound("resources/instrument/piano/7-b.mp3");

// Top C (octave 8)
    sound[C8]  = LoadSound("resources/instrument/piano/8-c.mp3");
}
void LoadResources(Plug *plug) {
    plug->images[0] = LoadTexture("resources/images/background1.jpg");
    plug->images[1] = LoadTexture("resources/images/background2.jpg");
    plug->images[2] = LoadTexture("resources/images/button1.png");
    plug->images[3] = LoadTexture("resources/images/button2.png");

    plug->music[0] = LoadMusicStream("resources/music/4AM.mp3");
    plug->music[1] = LoadMusicStream("resources/music/Bohemian.mp3");
    plug->music[2] = LoadMusicStream("resources/music/Duvet.mp3");
    plug->music[3] = LoadMusicStream("resources/music/Just A Joke.mp3");
    plug->music[4] = LoadMusicStream("resources/music/Machi No Dorufin.mp3");
}

void LoadUi(Plug *plug) {
    /***************** HOME SCREEN UI ******************/

    plug->buttons[HOME_SCREEN][0] = CreateMenuButton(plug->images[2], "Interval", GREEN, (Rectangle) {
        .width = plug->images[2].width/2.0, .height = plug->images[2].height/2.0, .x = GetRenderWidth() * 3/4.0, .y = 200
    });
    plug->buttons[HOME_SCREEN][1] = CreateMenuButton(plug->images[2], "Chord", GREEN, (Rectangle) {
        .width = plug->images[2].width/2.0, .height = plug->images[2].height/2.0, .x = GetRenderWidth() * 3/4.0, .y = 200
    });
    plug->buttons[HOME_SCREEN][2] = CreateMenuButton(plug->images[2], "Settings", GREEN, (Rectangle) {
        .width = plug->images[2].width/2.0, .height = plug->images[2].height/2.0, .x = GetRenderWidth() * 3/4.0, .y = 200
    });
    plug->buttons[HOME_SCREEN][3] = CreateMenuButton(plug->images[2], "Quit", GREEN, (Rectangle) {
        .width = plug->images[2].width/2.0, .height = plug->images[2].height/2.0, .x = GetRenderWidth() * 3/4.0, .y = 200
    });
    for (plug->no_of_buttons[HOME_SCREEN] = 0; plug->buttons[HOME_SCREEN][plug->no_of_buttons[HOME_SCREEN]].is == true; plug->no_of_buttons[HOME_SCREEN]++);

    /***************************************************/
}
