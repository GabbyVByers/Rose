
/*
 *   Source File [main.c]
 */

#include "rose.h"

#define CHESS_BOARD_WIDTH  ((intmax)480)
#define CHESS_SQUARE_WIDTH ((intmax)60)

#define CHESS_NULL_PIECE   0b00000000

#define CHESS_KING         0b00000001
#define CHESS_QUEEN        0b00000010
#define CHESS_ROOK         0b00000011
#define CHESS_HORSE        0b00000100
#define CHESS_BISHOP       0b00000101
#define CHESS_PAWN         0b00000110

#define CHESS_WHITE_KING   0b00000001
#define CHESS_WHITE_QUEEN  0b00000010
#define CHESS_WHITE_ROOK   0b00000011
#define CHESS_WHITE_HORSE  0b00000100
#define CHESS_WHITE_BISHOP 0b00000101
#define CHESS_WHITE_PAWN   0b00000110

#define CHESS_BLACK_KING   0b00001001
#define CHESS_BLACK_QUEEN  0b00001010
#define CHESS_BLACK_ROOK   0b00001011
#define CHESS_BLACK_HORSE  0b00001100
#define CHESS_BLACK_BISHOP 0b00001101
#define CHESS_BLACK_PAWN   0b00001110

#define CHESS_SPRITE_MASK  0b00001111
#define CHESS_TYPE_MASK    0b00000111
#define CHESS_PLAYER_MASK  0b00001000
#define CHESS_MOVED_MASK   0b00010000

ROSE_Image* board_image = NULL;
ROSE_Image* highlight_image = NULL;

ROSE_Sprite* board_sprite = NULL;
ROSE_Sprite* highlight_sprite = NULL;
ROSE_Sprite* chess_pieces[15] = { NULL };

ROSE_Text* rank_labels[8] = { NULL };
ROSE_Text* file_labels[8] = { NULL };

static void CHESS_InitAssets(void) {

	rank_labels[0] = ROSE_CreateText("8");
	rank_labels[1] = ROSE_CreateText("6");
	rank_labels[2] = ROSE_CreateText("7");
	rank_labels[3] = ROSE_CreateText("5");
	rank_labels[4] = ROSE_CreateText("4");
	rank_labels[5] = ROSE_CreateText("3");
	rank_labels[6] = ROSE_CreateText("2");
	rank_labels[7] = ROSE_CreateText("1");

	file_labels[0] = ROSE_CreateText("A");
	file_labels[1] = ROSE_CreateText("B");
	file_labels[2] = ROSE_CreateText("C");
	file_labels[3] = ROSE_CreateText("D");
	file_labels[4] = ROSE_CreateText("E");
	file_labels[5] = ROSE_CreateText("F");
	file_labels[6] = ROSE_CreateText("G");
	file_labels[7] = ROSE_CreateText("H");

	board_image     = ROSE_LoadPNGImage("Textures/Board.png");
	highlight_image = ROSE_LoadPNGImage("Textures/Highlight.png");

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

	board_sprite     = ROSE_CreateSprite(board_image);
	highlight_sprite = ROSE_CreateSprite(highlight_image);

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
	chess_pieces[CHESS_WHITE_KING]   = white_king_sprite;
	chess_pieces[CHESS_WHITE_QUEEN]  = white_queen_sprite;
	chess_pieces[CHESS_WHITE_ROOK]   = white_rook_sprite;
	chess_pieces[CHESS_WHITE_HORSE]  = white_horse_sprite;
	chess_pieces[CHESS_WHITE_BISHOP] = white_bishop_sprite;
	chess_pieces[CHESS_WHITE_PAWN]   = white_pawn_sprite;
	chess_pieces[7] = NULL;
	chess_pieces[8] = NULL;
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

	ROSE_DestroyImage(board_image);
	ROSE_DestroyImage(highlight_image);
}

static void CHESS_CleanUpAssets(void) {
	ROSE_DestroySprite(board_sprite);
	ROSE_DestroySprite(highlight_sprite);

	for (intmax i = 0; i < 15; i++) {
		ROSE_Sprite* sprite = chess_pieces[i];
		if (sprite) { ROSE_DestroySprite(sprite); };
	}

	for (intmax i = 0; i < 8; i++) {
		ROSE_DestroyText(rank_labels[i]);
		ROSE_DestroyText(file_labels[i]);
	}
}

typedef struct CHESS_ChessBoardState {
	uint8_t grid[64];
	bool turn; // TRUE = WHITE TO PLAY
} CHESS_ChessBoardState;

static CHESS_ChessBoardState CHESS_InitChessBoard(void) {
	CHESS_ChessBoardState state = { 0 };
	state.turn = true;

	for (intmax index = 0; index < 64; index++) {
		state.grid[index] = CHESS_NULL_PIECE;
	}

	state.grid[0]  = CHESS_BLACK_ROOK;
	state.grid[1]  = CHESS_BLACK_HORSE;
	state.grid[2]  = CHESS_BLACK_BISHOP;
	state.grid[3]  = CHESS_BLACK_QUEEN;
	state.grid[4]  = CHESS_BLACK_KING;
	state.grid[5]  = CHESS_BLACK_BISHOP;
	state.grid[6]  = CHESS_BLACK_HORSE;
	state.grid[7]  = CHESS_BLACK_ROOK;

	state.grid[8]  = CHESS_BLACK_PAWN;
	state.grid[9]  = CHESS_BLACK_PAWN;
	state.grid[10] = CHESS_BLACK_PAWN;
	state.grid[11] = CHESS_BLACK_PAWN;
	state.grid[12] = CHESS_BLACK_PAWN;
	state.grid[13] = CHESS_BLACK_PAWN;
	state.grid[14] = CHESS_BLACK_PAWN;
	state.grid[15] = CHESS_BLACK_PAWN;

	state.grid[48] = CHESS_WHITE_PAWN;
	state.grid[49] = CHESS_WHITE_PAWN;
	state.grid[50] = CHESS_WHITE_PAWN;
	state.grid[51] = CHESS_WHITE_PAWN;
	state.grid[52] = CHESS_WHITE_PAWN;
	state.grid[53] = CHESS_WHITE_PAWN;
	state.grid[54] = CHESS_WHITE_PAWN;
	state.grid[55] = CHESS_WHITE_PAWN;

	state.grid[56] = CHESS_WHITE_ROOK;
	state.grid[57] = CHESS_WHITE_HORSE;
	state.grid[58] = CHESS_WHITE_BISHOP;
	state.grid[59] = CHESS_WHITE_QUEEN;
	state.grid[60] = CHESS_WHITE_KING;
	state.grid[61] = CHESS_WHITE_BISHOP;
	state.grid[62] = CHESS_WHITE_HORSE;
	state.grid[63] = CHESS_WHITE_ROOK;

	return state;
}

static intmax* CHESS_EnumerateLegalMoves(CHESS_ChessBoardState* state) {
	intmax* legal_moves = malloc(sizeof(intmax) * 4096);
	if (!legal_moves) {
		const char* message = "OOM!";
		fprintf(stderr, "CHESS_EnumerateLegalMoves() Failed: %s", message);
		exit(EXIT_FAILURE);
	}

	for (intmax i = 0; i < 4069; i++) {
		legal_moves[i] = INT64_MAX;
	}

	for (intmax start_square = 0; start_square < 64; start_square++) {
		intmax num_moves = 0;
		intmax* moves = &legal_moves[start_square * 64];
		uint8_t element = state->grid[start_square];
		if (element == CHESS_NULL_PIECE) { continue; }
		uint8_t type = element & CHESS_TYPE_MASK;
		bool rooky = (type == CHESS_ROOK) || (type == CHESS_QUEEN);
		bool bishopy = (type == CHESS_BISHOP) || (type == CHESS_QUEEN);
		intmax start_i = start_square % 8;
		intmax start_j = start_square / 8;
		if (rooky) {
			if (start_i < 7) {
				for (intmax end_i = start_i + 1; end_i < 8; end_i++) {
					intmax end_square = (start_j * 8) + end_i;
					moves[num_moves] = end_square;
					num_moves++;
				}
			}
			if (start_i > 0) {
				for (intmax end_i = start_i - 1; end_i >= 0; end_i--) {
					intmax end_square = (start_j * 8) + end_i;
					moves[num_moves] = end_square;
					num_moves++;
				}
			}
			if (start_j > 0) {
				for (intmax end_j = start_j - 1; end_j >= 0; end_j--) {
					intmax end_square = start_i + (end_j * 8);
					moves[num_moves] = end_square;
					num_moves++;
				}
			}
			if (start_j < 8) {
				for (intmax end_j = start_j + 1; end_j < 8; end_j++) {
					intmax end_square = start_i + (end_j * 8);
					moves[num_moves] = end_square;
					num_moves++;
				}
			}
		}
		if (bishopy) {
			if ((start_i < 7) && (start_j < 7)) {
				intmax end_i = start_i + 1;
				intmax end_j = start_j + 1;
				for (;;) {
					intmax end_square = end_i + (end_j * 8);
					moves[num_moves] = end_square;
					num_moves++;
					end_i += 1;
					end_j += 1;
					if (end_i == 8) { break; }
					if (end_j == 8) { break; }
				}
			}
			if ((start_i > 0) && (start_j > 0)) {
				intmax end_i = start_i - 1;
				intmax end_j = start_j - 1;
				for (;;) {
					intmax end_square = end_i + (end_j * 8);
					moves[num_moves] = end_square;
					num_moves++;
					end_i -= 1;
					end_j -= 1;
					if (end_i == -1) { break; }
					if (end_j == -1) { break; }
				}
			}


			if ((start_i < 7) && (start_j > 0)) {
				intmax end_i = start_i + 1;
				intmax end_j = start_j - 1;
				for (;;) {
					intmax end_square = end_i + (end_j * 8);
					moves[num_moves] = end_square;
					num_moves++;
					end_i += 1;
					end_j -= 1;
					if (end_i == 8) { break; }
					if (end_j == -1) { break; }
				}
			}
			if ((start_i > 0) && (start_j < 7)) {
				intmax end_i = start_i - 1;
				intmax end_j = start_j + 1;
				for (;;) {
					intmax end_square = end_i + (end_j * 8);
					moves[num_moves] = end_square;
					num_moves++;
					end_i -= 1;
					end_j += 1;
					if (end_i == -1) { break; }
					if (end_j == 8) { break; }
				}
			}
		}
	}

	return legal_moves;
}

static bool CHESS_RenderChessBoard(CHESS_ChessBoardState* state, intmax* legal_moves) {
	intmax screen_width, screen_height;
	ROSE_GetScreenSize(&screen_width, &screen_height);

	intmax board_px = (screen_width - CHESS_BOARD_WIDTH) / 2;
	intmax board_py = (screen_height - CHESS_BOARD_WIDTH) / 2;
	ROSE_DrawSprite(board_sprite, board_px, board_py, 1.0, ROSE_COLOR_WHITE);

	for (intmax i = 0; i < 8; i++) {
		intmax rx = board_px - 20;
		intmax ry = board_py + (CHESS_SQUARE_WIDTH * i) + 19;
		intmax fx = board_px + (CHESS_SQUARE_WIDTH * i) + 23;
		intmax fy = board_py + CHESS_BOARD_WIDTH + 5;
		ROSE_DrawText(rank_labels[i], rx, ry, 2.0, ROSE_COLOR_WHITE);
		ROSE_DrawText(file_labels[i], fx, fy, 2.0, ROSE_COLOR_WHITE);
	}

	for (intmax index = 0; index < 64; index++) {
		uint8_t piece = state->grid[index];
		if (piece == CHESS_NULL_PIECE) { continue; }
		intmax sprite_index = piece & 0b00001111;
		ROSE_Sprite* sprite = chess_pieces[sprite_index];
		intmax x = board_px + ((index % 8) * CHESS_SQUARE_WIDTH);
		intmax y = board_py + ((index / 8) * CHESS_SQUARE_WIDTH);
		ROSE_DrawSprite(sprite, x, y, 1.0, ROSE_COLOR_WHITE);
	}

	intmax mouse_square = INT64_MAX;
	intmax mouse_px, mouse_py;
	ROSE_GetMousePosition(&mouse_px, &mouse_py);
	intmax board_px_max = board_px + CHESS_BOARD_WIDTH;
	intmax board_py_max = board_py + CHESS_BOARD_WIDTH;
	bool mouse_over_board = ((mouse_px > board_px) && (mouse_px < board_px_max));
	mouse_over_board = mouse_over_board && ((mouse_py > board_py) && (mouse_py < board_py_max));
	if (mouse_over_board) {
		intmax mouse_board_px = mouse_px - board_px;
		intmax mouse_board_py = mouse_py - board_py;
		intmax i = mouse_board_px / CHESS_SQUARE_WIDTH;
		intmax j = mouse_board_py / CHESS_SQUARE_WIDTH;
		mouse_square = (j * 8) + i;
	}

	static uint8_t hand_piece = CHESS_NULL_PIECE;
	static intmax hand_square = INT64_MAX;

	if (ROSE_PressedMouseButton(ROSE_MOUSE_LEFT)) {
		if (mouse_square != INT64_MAX) {
			uint8_t piece = state->grid[mouse_square];
			if (piece != CHESS_NULL_PIECE) {
				hand_piece = piece;
				hand_square = mouse_square;
				state->grid[mouse_square] = CHESS_NULL_PIECE;
			}
		}
	}

	bool new_board_position = false;

	if (ROSE_ReleasedMouseButton(ROSE_MOUSE_LEFT)) {
		for (;;) {
			if (hand_piece == CHESS_NULL_PIECE) {
				break;
			}

			if (mouse_square != INT64_MAX) {
				state->grid[mouse_square] = hand_piece;
				hand_piece = CHESS_NULL_PIECE;
				hand_square = INT64_MAX;
				new_board_position = true;
				break;
			}
			
			state->grid[hand_square] = hand_piece;
			hand_piece = CHESS_NULL_PIECE;
			hand_square = INT64_MAX;
			break;
		}
	}

	if (hand_square != INT64_MAX) {
		intmax* moves = &legal_moves[hand_square * 64];
		intmax move_index = 0;
		for (;;) {
			intmax end_square = moves[move_index];
			move_index++;
			if (end_square == INT64_MAX) { break; }
			intmax x = board_px + ((end_square % 8) * CHESS_SQUARE_WIDTH);
			intmax y = board_py + ((end_square / 8) * CHESS_SQUARE_WIDTH);
			ROSE_DrawSprite(highlight_sprite, x, y, 1.0, ROSE_COLOR_WHITE);
		}

		intmax sprite_index = hand_piece & CHESS_SPRITE_MASK;
		ROSE_Sprite* sprite = chess_pieces[sprite_index];
		intmax x = mouse_px - (CHESS_SQUARE_WIDTH / 2);
		intmax y = mouse_py - (CHESS_SQUARE_WIDTH / 2);
		ROSE_DrawSprite(sprite, x, y, 1.0, ROSE_COLOR_WHITE);
	}

	return new_board_position;
}

int main(void) {
	ROSE_Init("Chess Engine", 1200, 700, true);
	ROSE_SetMinScreenSize(CHESS_BOARD_WIDTH, CHESS_BOARD_WIDTH);
	ROSE_ToggleVSync(true);

	CHESS_InitAssets();
	CHESS_ChessBoardState chess_state = CHESS_InitChessBoard();
	intmax* legal_moves = CHESS_EnumerateLegalMoves(&chess_state);

	while (ROSE_PollEvents()) {
		ROSE_ClearScreen(ROSE_COLOR_BLACK);
		if (CHESS_RenderChessBoard(&chess_state, legal_moves)) {
			free(legal_moves);
			legal_moves = NULL;
			legal_moves = CHESS_EnumerateLegalMoves(&chess_state);
		} ROSE_SwapBuffers();
	}

	if (legal_moves) { free(legal_moves); }
	CHESS_CleanUpAssets();
	ROSE_Quit();
	return EXIT_SUCCESS;
}

