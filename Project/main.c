
/*
 *   Source File [main.c]
 */

#include "rose.h"

int main(void) {
	ROSE_Init("App Title", 1200, 700, true);
	ROSE_ToggleVSync(true);
	srand(67);

	ROSE_Text* text = ROSE_CreateText("Text Rendering!");
	ROSE_Image* image = ROSE_CreateImage(300, 300);
	ROSE_Sprite* sprite = ROSE_CreateSprite(image);

	while (ROSE_PollEvents()) {
		ROSE_ClearScreen(ROSE_COLOR_BLACK);
		ROSE_DrawSprite(sprite, 150, 150, 1.0, ROSE_COLOR_WHITE);
		ROSE_DrawText(text, 200, 30, 3.0, ROSE_COLOR_WHITE);
		ROSE_SwapBuffers();
	}

	ROSE_DestroyText(text);
	ROSE_DestroyImage(image);
	ROSE_DestroySprite(sprite);

	ROSE_Quit();
	return EXIT_SUCCESS;
}

