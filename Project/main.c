
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

		if (ROSE_PressedMouseButton(ROSE_MOUSE_LEFT)) {
			fprintf(stdout, "Pressed Mouse Left!\n");
		}

		if (ROSE_ReleasedMouseButton(ROSE_MOUSE_MIDDLE)) {
			fprintf(stdout, "Released Mouse Middle!\n");
		}

		fprintf(stdout, "Scroll: %f\n", ROSE_GetMouseScroll());

		ROSE_SwapBuffers();
	}

	ROSE_DestroyText(text);
	ROSE_DestroyImage(image);
	ROSE_DestroySprite(sprite);

	ROSE_Quit();
	return EXIT_SUCCESS;
}

