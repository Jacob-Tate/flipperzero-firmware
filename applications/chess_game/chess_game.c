#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/gui_i.h>
#include <input/input.h>
#include <notification/notification_messages.h>

#include "fast_chess.h"

static bool flag = true;
static bool should_exit = false;
// static bool ai_should_make_move = false;
static bool thinking = false;
static bool should_update_screen = true;
static uint32_t anim = 0;
static char white_move_str[8] = "", black_move_str[8] = ""; // last moves

static NotificationApp* notification;

typedef struct {
    uint8_t col, row;
} _Position;

typedef struct {
    enum {
        None = 0,
        Pawn,
        King,
        Queen,
        Bishop,
        Knight,
        Rook,
    } type;
    enum { White, Black } side;
} Piece;

static const _Position PosNone = {.col = 255, .row = 255};
// static Piece board[8][8]; // col, row
static _Position sel, move_from = PosNone, move_to = PosNone;

Game* game;

// uint8_t sel_col = 0, sel_row = 0;

// static enum {
//     SelectingFrom,
//     SelectingTo
// } state = SelectingFrom;

// static void reset_board() {
//     memset(board, 0, sizeof(board));

//     board[0][0].type = Rook;
//     board[1][0].type = Knight;
//     board[2][0].type = Bishop;
//     board[3][0].type = Queen;
//     board[4][0].type = King;
//     board[5][0].type = Bishop;
//     board[6][0].type = Knight;
//     board[7][0].type = Rook;

//     board[0][1].type = Pawn;
//     board[1][1].type = Pawn;
//     board[2][1].type = Pawn;
//     board[3][1].type = Pawn;
//     board[4][1].type = Pawn;
//     board[5][1].type = Pawn;
//     board[6][1].type = Pawn;
//     board[7][1].type = Pawn;

//     board[0][7].type = Rook;    board[0][7].side = Black;
//     board[1][7].type = Knight;  board[1][7].side = Black;
//     board[2][7].type = Bishop;  board[2][7].side = Black;
//     board[3][7].type = Queen;   board[3][7].side = Black;
//     board[4][7].type = King;    board[4][7].side = Black;
//     board[5][7].type = Bishop;  board[5][7].side = Black;
//     board[6][7].type = Knight;  board[6][7].side = Black;
//     board[7][7].type = Rook;    board[7][7].side = Black;

//     board[0][6].type = Pawn;    board[0][6].side = Black;
//     board[1][6].type = Pawn;    board[1][6].side = Black;
//     board[2][6].type = Pawn;    board[2][6].side = Black;
//     board[3][6].type = Pawn;    board[3][6].side = Black;
//     board[4][6].type = Pawn;    board[4][6].side = Black;
//     board[5][6].type = Pawn;    board[5][6].side = Black;
//     board[6][6].type = Pawn;    board[6][6].side = Black;
//     board[7][6].type = Pawn;    board[7][6].side = Black;
// }

// static const Icon* get_icon(const Piece* piece) {
//     if (piece->side == White) {
//         switch (piece->type) {
//             case Pawn: return &I_Chess_pw;
//             case King: return &I_Chess_kw;
//             case Queen: return &I_Chess_qw;
//             case Bishop: return &I_Chess_bw;
//             case Knight: return &I_Chess_nw;
//             case Rook: return &I_Chess_rw;
//             default: return NULL;
//         }
//     } else {
//         switch (piece->type) {
//             case Pawn: return &I_Chess_pb;
//             case King: return &I_Chess_kb;
//             case Queen: return &I_Chess_qb;
//             case Bishop: return &I_Chess_bb;
//             case Knight: return &I_Chess_nb;
//             case Rook: return &I_Chess_rb;
//             default: return NULL;
//         }
//     }
// }

static void notify_click() {
    // static const NotificationSequence sequence = {
    //     &message_click,
    //     &message_furi_hal_delay_ms_1,
    //     &message_sound_off,
    //     NULL,
    // };

    // notification_message_block(notification, &sequence);
    notification_message(notification, &sequence_single_vibro);
}
static const Icon* _get_icon(uint8_t file, uint8_t rank) {
    char piece = getPieceChar( (FILES_BB[file] & RANKS_BB[7-rank]), &(game->position.board));
    switch(piece) {
        case 'P': return &I_Chess_pw;
        case 'K': return &I_Chess_kw;
        case 'Q': return &I_Chess_qw;
        case 'B': return &I_Chess_bw;
        case 'N': return &I_Chess_nw;
        case 'R': return &I_Chess_rw;
        case 'p': return &I_Chess_pb;
        case 'k': return &I_Chess_kb;
        case 'q': return &I_Chess_qb;
        case 'b': return &I_Chess_bb;
        case 'n': return &I_Chess_nb;
        case 'r': return &I_Chess_rb;
        default: return NULL;
    }
}

static int get_position(uint8_t file, uint8_t rank) {
    return 8 * rank + file;
}

static int get_rank(int position) {
    return (int) (position/8);
}

static int get_file(int position) {
    return position % 8;
}

static void make_move(uint8_t file1, uint8_t rank1, uint8_t file2, uint8_t rank2) {
    int from = get_position(file1, rank1);
    int to = get_position(file2, rank2);
    Move move = generateMove(from, to);
    if (!isLegalMove(&game->position, move)) {
        return;
    }
    makeMove(game, move);
    move2str(white_move_str, game, game->moveListLen - 1);
    notify_click();
    black_move_str[0] = 0;
    anim = furi_hal_get_tick();
    thinking = true;
}

static int32_t make_ai_move(void* context) {
    // thinking = true;
    int depth = 1;
    Move move;
    Node node = iterativeDeepeningAlphaBeta(&(game->position), (char) depth, INT32_MIN, INT32_MAX, FALSE);
    move = node.move;
    makeMove(game, move);
    move2str(black_move_str, game, game->moveListLen - 1);
    notify_click();
    thinking = false;
    anim = furi_hal_get_tick();
    return 0;
}

static FuriThread* worker_thread = NULL;

static int32_t ai_thread(void* context) {
    while(true) {
        if (should_exit) break;
        if (thinking) make_ai_move(context);
        furi_hal_delay_ms(100);
    }
    return 0;
}

static void run_ai_thread() {

    if (worker_thread == NULL) {
        worker_thread = furi_thread_alloc();
    }

    furi_thread_set_name(worker_thread, "ChessEngine");
    furi_thread_set_stack_size(worker_thread, 7000);
    // furi_thread_set_context(thread, bad_usb);
    furi_thread_set_callback(worker_thread, ai_thread);
    furi_thread_start(worker_thread);

    // furi_thread_join(worker_thread);
    // furi_thread_free(worker_thread);
}

static void chess_draw_callback(Canvas* canvas, void* ctx) {
    should_update_screen = false;
    canvas_clear(canvas);

    // canvas_set_color(canvas, flag ? ColorBlack : ColorWhite);

    canvas_draw_icon(canvas, 0, 0, &I_Chess);

    if (!thinking) {
        canvas_set_color(canvas, (sel.col + sel.row) % 2 != 0 ? ColorBlack : ColorWhite);
        canvas_draw_icon(canvas, sel.col * 8, (7 - sel.row) * 8, flag ? &I_Chess_Selection1 : &I_Chess_Selection2);

        if (move_from.col != 255) {
            canvas_set_color(canvas, (move_from.col + move_from.row) % 2 != 0 ? ColorBlack : ColorWhite);
            canvas_draw_icon(canvas, move_from.col * 8, (7 - move_from.row) * 8, flag ? &I_Chess_Selection1 : &I_Chess_Selection2);
        }
    }

    // print moves
    if (game->moveListLen > 0) {
        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontSecondary);
        
        // int num = game->moveListLen;

        // char white_str[8], black_str[8] = "...";

        // if (num == 0) {
        // } else if (num % 2 == 0) {
        //     // white move
        //     move2str(white_str, game, game->moveListLen - 2);
        //     move2str(black_str, game, game->moveListLen - 1);
        // } else {
        //     move2str(white_str, game, game->moveListLen - 1);
        // }

        char str[28];
        sprintf(str, "%d. %s %s", (game->moveListLen + 1) / 2, white_move_str, black_move_str);
        canvas_draw_str(canvas, 75, 12, str);
    }

    Move last_move = getLastMove(game);

    for (uint8_t row = 0; row < 8; row++) {
        for (uint8_t col = 0; col < 8; col++) {

            bool white_field = (row + col) % 2 != 0;

            // if (!white_field) {
            //     canvas_draw_box(canvas, col * 8, row * 8, 8, 8);
            // }
            const Icon* icon = _get_icon(col, row);
            if (icon != NULL) {

                int x = col * 8;
                int y = row * 8;

                int dt = furi_hal_get_tick() - anim;
                if (anim && dt >= 300) {
                    anim = 0;
                }

                if (anim && last_move && get_file(getTo(last_move)) == col && get_rank(getTo(last_move)) == (7 - row)) {
                    // moving piece
                    uint8_t from_x = get_file(getFrom(last_move)) * 8;
                    uint8_t from_y = (7 - get_rank(getFrom(last_move))) * 8;
                    x = from_x + (x - from_x) * dt / 300;
                    y = from_y + (y - from_y) * dt / 300;
                }

                canvas_set_color(canvas, white_field ? ColorWhite : ColorBlack);
                canvas_draw_icon(canvas, x + 1, y + 1, icon);
            }

            // if (board[col][7 - row].type != None) {
            //     canvas_set_color(canvas, white_field ? ColorWhite : ColorBlack);
            //     canvas_draw_icon(canvas, col * 8 + 1, row * 8 + 1, get_icon(&board[col][7 - row]));
            // }
        }
    }


    // for (uint8_t i = 0; i < 4; i++) {
    //     canvas_draw_dot(canvas, sel_col * 8, sel_row * 8);
    //     canvas_draw_dot(canvas, sel_col * 8 + 2, sel_row * 8);
    //     canvas_draw_dot(canvas, sel_col * 8, sel_row * 8);
    //     canvas_draw_dot(canvas, sel_col * 8, sel_row * 8);
    // }


    // canvas_draw_disc(canvas, GUI_DISPLAY_WIDTH / 2 - 40, GUI_DISPLAY_HEIGHT / 2, 15);
    // canvas_set_color(canvas, flag ? ColorBlack : ColorWhite);
    // canvas_draw_disc(canvas, GUI_DISPLAY_WIDTH / 2, GUI_DISPLAY_HEIGHT / 2, 15);
}

static void chess_input_callback(InputEvent* event, void* ctx) {
    if(event->type == InputTypeShort) {
        if (event->key == InputKeyLeft) {
            sel.col = (sel.col == 0) ? 0 : sel.col - 1;
        } else if (event->key == InputKeyRight) {
            sel.col++;
        } else if (event->key == InputKeyDown) {
            sel.row = (sel.row == 0) ? 0 : sel.row - 1;
        } else if (event->key == InputKeyUp) {
            sel.row++;
        } else if (event->key == InputKeyOk) {
            if (move_from.col == 255) {
                move_from = sel;
            } else if (move_to.col == 255) {
                move_to = sel;
                make_move(move_from.col, move_from.row, move_to.col, move_to.row);
                // thinking = true;
                // ai_should_make_move = true;
                // make_ai_move_threaded();
                // Piece piece = board[move_from.col][move_from.row];
                // board[move_from.col][move_from.row].type = None;
                // board[move_to.col][move_to.row] = piece;
                move_from = PosNone;
                move_to = PosNone;
            }
        } else if (event->key == InputKeyBack) {
            should_exit = true;
        }
        sel.col = CLAMP(sel.col, 7, 0);
        sel.row = CLAMP(sel.row, 7, 0);
    }
}

static void setup_engine() {
    // int depth = 1; // DEFAULT_AI_DEPTH;

    getInitialGame(game);

    // Move move;
    // Node node = iterativeDeepeningAlphaBeta(&(game.position), (char) depth, INT32_MIN, INT32_MAX, FALSE);
    // move = node.move;

    // node = iterativeDeepeningAlphaBeta(&(game.position), (char) 2, INT32_MIN, INT32_MAX, FALSE);

    // node = iterativeDeepeningAlphaBeta(&(game.position), (char) 3, INT32_MIN, INT32_MAX, FALSE);

    // printf("%d\n", move);
}

//  void test_engine() {
//     FuriThread* thread; //

//     thread = furi_thread_alloc();
//     furi_thread_set_name(thread, "ChessEngine");
//     furi_thread_set_stack_size(thread, 20000);
//     // furi_thread_set_context(thread, bad_usb);
//     furi_thread_set_callback(thread, setup_engine);

//     furi_thread_start(thread);

//     furi_thread_join(thread);

//     furi_thread_free(thread);
// }


int32_t chess_game_app(void* p) {
    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, chess_draw_callback, NULL);
    view_port_input_callback_set(view_port, chess_input_callback, NULL);

    // Register view port in GUI
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    notification = furi_record_open("notification");

    should_exit = false;

    game = malloc(sizeof(Game));

    setup_engine();
    run_ai_thread();

    // test_engine();

    while(!should_exit) {
        furi_hal_delay_ms(100);
        if (!thinking) {
            flag = !flag;
            should_update_screen = true;
        }
        if (anim) {
            should_update_screen = true;
        }
        if (should_update_screen) {
            view_port_update(view_port);
        }
        // flag = true;
        // furi_hal_delay_ms(40);
        // flag = false;
        // view_port_update(view_port);
        // furi_hal_delay_ms(80);
    }

    furi_thread_join(worker_thread);
    furi_thread_free(worker_thread);
    worker_thread = NULL;

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);

    furi_record_close("gui");

    free(game);

    return 0;
}
