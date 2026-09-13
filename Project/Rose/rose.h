
/*
 *   Header File [rose.h]
 */

#ifndef ROSE_ROSE_HEADER_GUARD
#define ROSE_ROSE_HEADER_GUARD

#include "core.h"

typedef struct ROSE_Vertex {
	float pos[2];
	float uv[2];
} ROSE_Vertex;

typedef struct ROSE_Color {
	float r, g, b, a;
} ROSE_Color;

typedef struct ROSE_Image ROSE_Image;
typedef struct ROSE_Sprite ROSE_Sprite;
typedef struct ROSE_Text ROSE_Text;

void ROSE_Init(const char*, size_t, size_t, bool);
void ROSE_Quit(void);

ROSE_Image* ROSE_LoadPNGImage(const char*);
ROSE_Image* ROSE_CreateImage(size_t, size_t);
ROSE_Color ROSE_GetImagePixel(ROSE_Image*, size_t, size_t);
void ROSE_SetImagePixel(ROSE_Image*, size_t, size_t, ROSE_Color);
void ROSE_ImageSize(ROSE_Image*, size_t*, size_t*);
void ROSE_DestroyImage(ROSE_Image*);

ROSE_Sprite* ROSE_CreateSprite(ROSE_Image*);
void ROSE_UploadSpriteTexture(ROSE_Sprite*, ROSE_Image*);
void ROSE_DestroySprite(ROSE_Sprite*);

ROSE_Text* ROSE_CreateText(const char*);
void ROSE_DestroyText(ROSE_Text*);

void ROSE_ToggleVSync(bool);
void ROSE_GetScreenSize(size_t*, size_t*);
bool ROSE_PollEvents(void);
void ROSE_ClearScreen(ROSE_Color);
void ROSE_DrawSprite(ROSE_Sprite*, size_t, size_t, double, ROSE_Color);
void ROSE_DrawText(ROSE_Text*, size_t, size_t, double, ROSE_Color);
void ROSE_SwapBuffers(void);

bool ROSE_HideMouseCursor(void);
bool ROSE_RevealMouseCursor(void);
bool ROSE_PressingMouseButton(int);
bool ROSE_PressedMouseButton(int);
bool ROSE_ReleasedMouseButton(int);
void ROSE_GetMousePosition(float*, float*);
void ROSE_GetMouseVelocity(float*, float*);
float ROSE_GetMouseScroll(void);

bool ROSE_PressingKeyboardButton(int);
bool ROSE_PressedKeyboardButton(int);
bool ROSE_ReleasedKeyboardButton(int);

SDL_GPUTexture* ROSE_INTERNAL_CreateDepthTexture(void);
SDL_GPUTexture* ROSE_INTERNAL_CreateRenderTexture(size_t, size_t);
void ROSE_INTERNAL_UploadImageToRenderTexture(uint8_t*, size_t, size_t, SDL_GPUTexture*);
SDL_GPUBuffer* ROSE_INTERNAL_CreateVertexBuffer(ROSE_Vertex*, size_t);

#endif /* ROSE_ROSE_HEADER_GUARD */

