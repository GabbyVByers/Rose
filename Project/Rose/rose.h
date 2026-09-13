
/*
 *   Header File [rose.h]
 */

#ifndef ROSE_HEADER_GUARD
#define ROSE_HEADER_GUARD

#include "SDL3/SDL.h"
#include "stb_image.h"

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"

#define ROSE_MIN_WIDTH  ((size_t)32)
#define ROSE_MIN_HEIGHT ((size_t)32)

typedef struct {
	float pos[2];
	float uv[2];
} ROSE_Vertex;

typedef struct {
	float r, g, b, a;
} ROSE_Color;

#define ROSE_COLOR_WHITE  ((ROSE_Color) { 1.0f, 1.0f, 1.0f, 1.0f })
#define ROSE_COLOR_BLACK  ((ROSE_Color) { 0.0f, 0.0f, 0.0f, 1.0f })
#define ROSE_COLOR_RED    ((ROSE_Color) { 1.0f, 0.0f, 0.0f, 1.0f })
#define ROSE_COLOR_GREEN  ((ROSE_Color) { 0.0f, 1.0f, 0.0f, 1.0f })
#define ROSE_COLOR_BLUE   ((ROSE_Color) { 0.0f, 0.0f, 1.0f, 1.0f })
#define ROSE_COLOR_PURPLE ((ROSE_Color) { 1.0f, 0.0f, 1.0f, 1.0f })
#define ROSE_COLOR_YELLOW ((ROSE_Color) { 1.0f, 1.0f, 0.0f, 1.0f })
#define ROSE_COLOR_CYAN   ((ROSE_Color) { 0.0f, 1.0f, 1.0f, 1.0f })

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
void ROSE_ScreenSize(size_t*, size_t*);
bool ROSE_PollEvents(void);
void ROSE_ClearScreen(ROSE_Color);
void ROSE_DrawSprite(ROSE_Sprite*, size_t, size_t, double, ROSE_Color);
void ROSE_DrawText(ROSE_Text*, size_t, size_t, double, ROSE_Color);
void ROSE_PutPixel(ROSE_Color, size_t, size_t);
void ROSE_SwapBuffers(void);

SDL_GPUTexture* ROSE_INTERNAL_CreateDepthTexture(void);
SDL_GPUTexture* ROSE_INTERNAL_CreateGPUTexture(size_t, size_t);
void ROSE_INTERNAL_UploadImageToGPUTexture(uint8_t*, size_t, size_t, SDL_GPUTexture*);
SDL_GPUBuffer* ROSE_INTERNAL_CreateGPUVertexBuffer(ROSE_Vertex*, size_t);

#endif /* ROSE_HEADER_GUARD */

