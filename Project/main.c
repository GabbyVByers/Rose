
/*
 *   Source File [main.c]
 */

#include "rose.h"

int main(void) {

	ROSE_Init("App Title", 500, 500, true);
	ROSE_ToggleVSync(false);
	srand(67);

	ROSE_Image* image = ROSE_LoadPNGImage("Textures/Board.png");
	ROSE_Sprite* sprite = ROSE_CreateSprite(image);

	while (ROSE_PollEvents()) {
		ROSE_ClearScreen(ROSE_COLOR_BLACK);
		ROSE_DrawSprite(sprite, 50, 50, 1.0, ROSE_COLOR_WHITE);

		size_t w, h;
		ROSE_ScreenSize(&w, &h);
		for (size_t i = 0; i < 10000; i++) {
			size_t x = rand() % w;
			size_t y = rand() % h;
			ROSE_PutPixel(ROSE_COLOR_WHITE, x, y);
		}

		ROSE_SwapBuffers();
	}

	ROSE_DestroyImage(image);
	ROSE_DestroySprite(sprite);

	ROSE_Quit();
	return EXIT_SUCCESS;
}

