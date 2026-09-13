
/*
 *   Source File [main.c]
 */

#include "rose.h"

#define CHESS_BOARD_WIDTH  480
#define CHESS_SQUARE_WIDTH 60

#define CHESS_NULL_PIECE   0b0000000000000000

#define CHESS_WHITE_KING   0b0000000000000001
#define CHESS_WHITE_QUEEN  0b0000000000000010
#define CHESS_WHITE_ROOK   0b0000000000000011
#define CHESS_WHITE_HORSE  0b0000000000000100
#define CHESS_WHITE_BISHOP 0b0000000000000101
#define CHESS_WHITE_PAWN   0b0000000000000110

#define CHESS_BLACK_KING   0b0000000000001001
#define CHESS_BLACK_QUEEN  0b0000000000001010
#define CHESS_BLACK_ROOK   0b0000000000001011
#define CHESS_BLACK_HORSE  0b0000000000001100
#define CHESS_BLACK_BISHOP 0b0000000000001101
#define CHESS_BLACK_PAWN   0b0000000000001110

ROSE_Image* chess_board_image = NULL;
ROSE_Image* highlight_square_image = NULL;
ROSE_Sprite* chess_board_sprite = NULL;
ROSE_Sprite* highlight_square_sprite = NULL;
ROSE_Sprite* chess_pieces[15] = { 0 };

static void CHESS_InitAssets(void) {
	chess_board_image = ROSE_LoadPNGImage("Textures/Board.png");
	highlight_square_image = ROSE_LoadPNGImage("Textures/Highlight.png");
	chess_board_sprite = ROSE_CreateSprite(chess_board_image);
	highlight_square_sprite = ROSE_CreateSprite(highlight_square_image);

	ROSE_Image* white_king_image   = ROSE_LoadPNGImage("Textures/White_King.png");
	ROSE_Image* white_queen_image  = ROSE_LoadPNGImage("Textures/White_Queen.png");
	ROSE_Image* white_rook_image   = ROSE_LoadPNGImage("Textures/White_Rook.png");
	ROSE_Image* white_horse_image  = ROSE_LoadPNGImage("Textures/White_Horse.png");
	ROSE_Image* white_bishop_image = ROSE_LoadPNGImage("Textures/White_Bishop.png");
	ROSE_Image* white_pawn_image   = ROSE_LoadPNGImage("Textures/White_Pawn.png");
	
	ROSE_Image* black_king_image   = ROSE_LoadPNGImage("Textures/Black_King.png");
	ROSE_Image* black_queen_image  = ROSE_LoadPNGImage("Textures/Black_Queen.png");
	ROSE_Image* black_rook_image   = ROSE_LoadPNGImage("Textures/Black_Rook.png");
	ROSE_Image* black_horse_image  = ROSE_LoadPNGImage("Textures/Black_Horse.png");
	ROSE_Image* black_bishop_image = ROSE_LoadPNGImage("Textures/Black_Bishop.png");
	ROSE_Image* black_pawn_image   = ROSE_LoadPNGImage("Textures/Black_Pawn.png");

	ROSE_Sprite* white_king_sprite   = ROSE_CreateSprite(white_king_image);
	ROSE_Sprite* white_queen_sprite  = ROSE_CreateSprite(white_queen_image);
	ROSE_Sprite* white_rook_sprite   = ROSE_CreateSprite(white_rook_image);
	ROSE_Sprite* white_horse_sprite  = ROSE_CreateSprite(white_horse_image);
	ROSE_Sprite* white_bishop_sprite = ROSE_CreateSprite(white_bishop_image);
	ROSE_Sprite* white_pawn_sprite   = ROSE_CreateSprite(white_pawn_image);

	ROSE_Sprite* black_king_sprite   = ROSE_CreateSprite(black_king_image);
	ROSE_Sprite* black_queen_sprite  = ROSE_CreateSprite(black_queen_image);
	ROSE_Sprite* black_rook_sprite   = ROSE_CreateSprite(black_rook_image);
	ROSE_Sprite* black_horse_sprite  = ROSE_CreateSprite(black_horse_image);
	ROSE_Sprite* black_bishop_sprite = ROSE_CreateSprite(black_bishop_image);
	ROSE_Sprite* black_pawn_sprite   = ROSE_CreateSprite(black_pawn_image);

	chess_pieces[0] = NULL;
	chess_pieces[7] = NULL;
	chess_pieces[8] = NULL;

	chess_pieces[CHESS_WHITE_KING]   = white_king_sprite;
	chess_pieces[CHESS_WHITE_QUEEN]  = white_queen_sprite;
	chess_pieces[CHESS_WHITE_ROOK]   = white_rook_sprite;
	chess_pieces[CHESS_WHITE_HORSE]  = white_horse_sprite;
	chess_pieces[CHESS_WHITE_BISHOP] = white_bishop_sprite;
	chess_pieces[CHESS_WHITE_PAWN]   = white_pawn_sprite;

	chess_pieces[CHESS_BLACK_KING]   = black_king_sprite;
	chess_pieces[CHESS_BLACK_QUEEN]  = black_queen_sprite;
	chess_pieces[CHESS_BLACK_ROOK]   = black_rook_sprite;
	chess_pieces[CHESS_BLACK_HORSE]  = black_horse_sprite;
	chess_pieces[CHESS_BLACK_BISHOP] = black_bishop_sprite;
	chess_pieces[CHESS_BLACK_PAWN]   = black_pawn_sprite;

	ROSE_DestroyImage(white_king_image);
	ROSE_DestroyImage(white_queen_image);
	ROSE_DestroyImage(white_rook_image);
	ROSE_DestroyImage(white_horse_image);
	ROSE_DestroyImage(white_bishop_image);
	ROSE_DestroyImage(white_pawn_image);

	ROSE_DestroyImage(black_king_image);
	ROSE_DestroyImage(black_queen_image);
	ROSE_DestroyImage(black_rook_image);
	ROSE_DestroyImage(black_horse_image);
	ROSE_DestroyImage(black_bishop_image);
	ROSE_DestroyImage(black_pawn_image);

	ROSE_DestroyImage(chess_board_image);
	ROSE_DestroyImage(highlight_square_image);
}

static void CHESS_CleanUpAssets(void) {
	ROSE_DestroySprite(chess_board_sprite);
	ROSE_DestroySprite(highlight_square_sprite);

	for (size_t index = 0; index < 15; index++) {
		ROSE_Sprite* sprite = chess_pieces[index];
		if (sprite) { ROSE_DestroySprite(sprite); };
	}
}

typedef enum CHESS_Player {
	CHESS_WHITE,
	CHESS_BLACK,
} CHESS_Player;

typedef struct CHESS_ChessBoardState {
	uint16_t grid[64];
	CHESS_Player turn;
} CHESS_ChessBoardState;

static CHESS_ChessBoardState CHESS_InitChessBoard(void) {
	CHESS_ChessBoardState chess_state = { 0 };
	chess_state.turn = CHESS_WHITE;

	for (size_t index = 0; index < 64; index++) {
		chess_state.grid[index] = CHESS_NULL_PIECE;
	}

	chess_state.grid[0]  = CHESS_BLACK_ROOK;
	chess_state.grid[1]  = CHESS_BLACK_HORSE;
	chess_state.grid[2]  = CHESS_BLACK_BISHOP;
	chess_state.grid[3]  = CHESS_BLACK_QUEEN;
	chess_state.grid[4]  = CHESS_BLACK_KING;
	chess_state.grid[5]  = CHESS_BLACK_BISHOP;
	chess_state.grid[6]  = CHESS_BLACK_HORSE;
	chess_state.grid[7]  = CHESS_BLACK_ROOK;

	chess_state.grid[8]  = CHESS_BLACK_PAWN;
	chess_state.grid[9]  = CHESS_BLACK_PAWN;
	chess_state.grid[10] = CHESS_BLACK_PAWN;
	chess_state.grid[11] = CHESS_BLACK_PAWN;
	chess_state.grid[12] = CHESS_BLACK_PAWN;
	chess_state.grid[13] = CHESS_BLACK_PAWN;
	chess_state.grid[14] = CHESS_BLACK_PAWN;
	chess_state.grid[15] = CHESS_BLACK_PAWN;

	chess_state.grid[48] = CHESS_WHITE_PAWN;
	chess_state.grid[49] = CHESS_WHITE_PAWN;
	chess_state.grid[50] = CHESS_WHITE_PAWN;
	chess_state.grid[51] = CHESS_WHITE_PAWN;
	chess_state.grid[52] = CHESS_WHITE_PAWN;
	chess_state.grid[53] = CHESS_WHITE_PAWN;
	chess_state.grid[54] = CHESS_WHITE_PAWN;
	chess_state.grid[55] = CHESS_WHITE_PAWN;

	chess_state.grid[56] = CHESS_WHITE_ROOK;
	chess_state.grid[57] = CHESS_WHITE_HORSE;
	chess_state.grid[58] = CHESS_WHITE_BISHOP;
	chess_state.grid[59] = CHESS_WHITE_QUEEN;
	chess_state.grid[60] = CHESS_WHITE_KING;
	chess_state.grid[61] = CHESS_WHITE_BISHOP;
	chess_state.grid[62] = CHESS_WHITE_HORSE;
	chess_state.grid[63] = CHESS_WHITE_ROOK;

	return chess_state;
}

static void CHESS_RenderChessBoard(CHESS_ChessBoardState* chess_state) {
	size_t screen_width, screen_height;
	ROSE_GetScreenSize(&screen_width, &screen_height);

	size_t board_px = (screen_width - CHESS_BOARD_WIDTH) / 2;
	size_t board_py = (screen_height - CHESS_BOARD_WIDTH) / 2;
	ROSE_DrawSprite(chess_board_sprite, board_px, board_py, 1.0, ROSE_COLOR_WHITE);

	for (size_t index = 0; index < 64; index++) {
		uint16_t piece = chess_state->grid[index];
		if (piece == CHESS_NULL_PIECE) { continue; }
		size_t i = index % 8;
		size_t j = index / 8;
		size_t sprite_index = piece & 0b0000000000001111;
		ROSE_Sprite* sprite = chess_pieces[sprite_index];
		size_t sprite_px = board_px + (i * CHESS_SQUARE_WIDTH);
		size_t sprite_py = board_py + (j * CHESS_SQUARE_WIDTH);
		ROSE_DrawSprite(sprite, sprite_px, sprite_py, 1.0, ROSE_COLOR_WHITE);
	}

	size_t mouse_square = SIZE_MAX;
	size_t mouse_px, mouse_py;
	ROSE_GetMousePosition(&mouse_px, &mouse_py);
	size_t board_px_max = board_px + CHESS_BOARD_WIDTH;
	size_t board_py_max = board_py + CHESS_BOARD_WIDTH;
	bool mouse_over_board = ((mouse_px > board_px) && (mouse_px < board_px_max));
	mouse_over_board = mouse_over_board && ((mouse_py > board_py) && (mouse_py < board_py_max));
	if (mouse_over_board) {
		size_t mouse_board_px = mouse_px - board_px;
		size_t mouse_board_py = mouse_py - board_py;
		size_t i = mouse_board_px / CHESS_SQUARE_WIDTH;
		size_t j = mouse_board_py / CHESS_SQUARE_WIDTH;
		mouse_square = (j * (size_t)8) + i;
	}

	static uint16_t hand_piece = CHESS_NULL_PIECE;
	static size_t hand_piece_index = SIZE_MAX;

	if (ROSE_PressedMouseButton(ROSE_MOUSE_LEFT)) {
		if (mouse_square != SIZE_MAX) {
			uint16_t piece = chess_state->grid[mouse_square];
			if (piece != CHESS_NULL_PIECE) {
				hand_piece = piece;
				chess_state->grid[mouse_square] = CHESS_NULL_PIECE;
				hand_piece_index = mouse_square;
			}
		}
	}

	if (ROSE_ReleasedMouseButton(ROSE_MOUSE_LEFT)) {
		for (;;) {
			if (hand_piece == CHESS_NULL_PIECE) {
				break;
			}

			if (mouse_square != SIZE_MAX) {
				chess_state->grid[mouse_square] = hand_piece;
				hand_piece = CHESS_NULL_PIECE;
				hand_piece_index = SIZE_MAX;
				break;
			}
			
			chess_state->grid[hand_piece_index] = hand_piece;
			hand_piece = CHESS_NULL_PIECE;
			hand_piece_index = SIZE_MAX;
			break;
		}
	}

	if (hand_piece != CHESS_NULL_PIECE) {
		size_t sprite_index = hand_piece & 0b0000000000001111;
		ROSE_Sprite* sprite = chess_pieces[sprite_index];
		size_t x = mouse_px - (CHESS_SQUARE_WIDTH / 2);
		size_t y = mouse_py - (CHESS_SQUARE_WIDTH / 2);
		ROSE_DrawSprite(sprite, x, y, 1.0, ROSE_COLOR_WHITE);
	}
}

int main(void) {
	ROSE_Init("Chess Engine", 1200, 700, true);
	ROSE_SetMinScreenSize(CHESS_BOARD_WIDTH, CHESS_BOARD_WIDTH);
	ROSE_ToggleVSync(true);

	CHESS_InitAssets();
	CHESS_ChessBoardState chess_state = CHESS_InitChessBoard();

	while (ROSE_PollEvents()) {
		ROSE_ClearScreen(ROSE_COLOR_BLACK);
		CHESS_RenderChessBoard(&chess_state);
		ROSE_SwapBuffers();
	}

	CHESS_CleanUpAssets();
	ROSE_Quit();
	return EXIT_SUCCESS;
}

