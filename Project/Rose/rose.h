
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

void ROSE_Init(const char*, intmax, intmax, bool);
void ROSE_Quit(void);

typedef struct ROSE_Image ROSE_Image;
typedef struct ROSE_Sprite ROSE_Sprite;
typedef struct ROSE_Text ROSE_Text;

ROSE_Image* ROSE_LoadPNGImage(const char*);
ROSE_Image* ROSE_CreateImage(intmax, intmax);
ROSE_Color ROSE_GetImagePixel(ROSE_Image*, intmax, intmax);
void ROSE_SetImagePixel(ROSE_Image*, intmax, intmax, ROSE_Color);
void ROSE_GetImageSize(ROSE_Image*, intmax*, intmax*);
void ROSE_DestroyImage(ROSE_Image*);

ROSE_Sprite* ROSE_CreateSprite(ROSE_Image*);
void ROSE_UploadSpriteTexture(ROSE_Sprite*, ROSE_Image*);
void ROSE_GetSpriteSize(ROSE_Sprite*, intmax*, intmax*);
void ROSE_DestroySprite(ROSE_Sprite*);

ROSE_Text* ROSE_CreateText(const char*);
void ROSE_DestroyText(ROSE_Text*);

void ROSE_ToggleVSync(bool);
void ROSE_SetMinScreenSize(intmax, intmax);
void ROSE_GetScreenSize(intmax*, intmax*);
bool ROSE_PollEvents(void);
void ROSE_ClearScreen(ROSE_Color);
void ROSE_DrawSprite(ROSE_Sprite*, intmax, intmax, double, ROSE_Color);
void ROSE_DrawText(ROSE_Text*, intmax, intmax, double, ROSE_Color);
void ROSE_SwapBuffers(void);

bool ROSE_HideMouseCursor(void);
bool ROSE_RevealMouseCursor(void);
bool ROSE_PressingMouseButton(int);
bool ROSE_PressedMouseButton(int);
bool ROSE_ReleasedMouseButton(int);
void ROSE_GetMousePosition(intmax*, intmax*);
void ROSE_GetMouseVelocity(intmax*, intmax*);
intmax ROSE_GetMouseScroll(void);

bool ROSE_PressingKeyboardButton(int);
bool ROSE_PressedKeyboardButton(int);
bool ROSE_ReleasedKeyboardButton(int);

SDL_GPUTexture* ROSE_INTERNAL_CreateDepthTexture(void);
SDL_GPUTexture* ROSE_INTERNAL_CreateRenderTexture(intmax, intmax);
void ROSE_INTERNAL_UploadImageToRenderTexture(uint8_t*, intmax, intmax, SDL_GPUTexture*);
SDL_GPUBuffer* ROSE_INTERNAL_CreateVertexBuffer(ROSE_Vertex*, intmax);

#endif /* ROSE_ROSE_HEADER_GUARD */

