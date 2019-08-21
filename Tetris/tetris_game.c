#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "queue.h"
#include <time.h>

#define BLOCK_WIDTH 32
#define BLOCK_HEIGHT 32

#define BLOCKS_IN_ROW 10
#define BLOCKS_IN_COLUMN 20

#define BORDER_THICKNESS 2 //pixels

#define DISPLAY_WIDTH (BLOCK_WIDTH * (BLOCKS_IN_ROW + 4) + BORDER_THICKNESS * 3)
#define DISPLAY_HEIGHT (BLOCK_HEIGHT * BLOCKS_IN_COLUMN + BORDER_THICKNESS * 2)

#define LOOP_FREQ 0.01
#define INITIAL_SPEED 0.6
#define ACCELERATION 0.01
#define MAX_SPEED 0.2

enum FONTS {
	ARIAL_15,
	ARIAL_30,
	DIGITAL,
	DIGITAL_25,
	NFONTS
};

enum COLORS {
	WHITE,
	CYAN,
	DARK_BLUE,
	ORANGE,
	YELLOW,
	GREEN,
	VIOLET,
	RED,
	BLUE,
	BLACK,
	GREY,
	SILVER,
	NCOLORS
};

enum SOUNDS {
	PAUSE_SOUND,
	BELL_SOUND,
	FALL_SOUND,
	END_SOUND,
	NSOUNDS
};

typedef struct _DIV {
	int x, y;
	int w, h;
	int xc, yc;
	int xf, yf;
	int thickness;
	enum COLORS border_color, background_color;
	struct _DIV * parent;
} DIV;

typedef struct {
	char type; // I, J, L, O, S, T, Z
	enum COLORS color;
	int positions[8]; //4 posicoes: cada dois representam o par ordenado
} FIGURE;

typedef struct {
	int x, y;
} VEC2;
/*
	Esquerda para direita, conforme imagem
	I: (-2,1) (-1,1) (1,1) (2,1)
	J: (-1,1) (-1,0) (0,0) (1,0)
	L: (-1,0) (0,0) (1,0) (1,1)
	O: (-1,1) (-1,-1) (1,1) (1,-1)
	S: (-1,0) (0,0) (0,1) (1,1)
	T: (-1,0) (0,1) (0,0) (1,0)
	Z: (-1,1) (0,1) (0,0) (1,0)
*/

typedef struct {
	int should_abort;
	int running;
	int paused;
	int state;

	int score;
	int highscore;
	int should_update_highscore;

	int orientation; // 0: direita, 1: cima, 2: esquerda, 3: baixo
	double speed;
	QUEUE next_moves;
	VEC2 pos;
	FIGURE curr_figure, next;
	unsigned char ** block_matrix; //guarda 0 se não está ocupado, e se está, guarda qual cor o ocupa.

	ALLEGRO_FONT * fonts[NFONTS];
	ALLEGRO_COLOR colors[NCOLORS];

	ALLEGRO_EVENT_QUEUE * timer_queue;
	ALLEGRO_EVENT_QUEUE * events_queue;	
	ALLEGRO_EVENT_QUEUE * input_queue;
	
	ALLEGRO_TIMER * loop_timer;
	ALLEGRO_TIMER * fall_timer;
	ALLEGRO_TIMER * regressive_timer;
	ALLEGRO_TIMER * another_timer;
	
	ALLEGRO_DISPLAY * display;
	ALLEGRO_BITMAP * display_bitmap;
	ALLEGRO_BITMAP * game_screen_bitmap;
	ALLEGRO_BITMAP * next_figure_bitmap;
	DIV game_screen;
	DIV next_figure;
	DIV info_div;
	DIV score_div;
	DIV highscore_div;

	struct {
		ALLEGRO_SAMPLE * sample;
		ALLEGRO_SAMPLE_INSTANCE * sample_instance;
		unsigned int sample_instance_position;
	} music;
	ALLEGRO_SAMPLE * sounds[NSOUNDS];

} GAME_MANAGER;

unsigned char color_rgbs[][3] = {{255, 255, 255}, {0, 255, 255}, {0, 0, 139}, {255, 165, 0}, {255, 255, 0}, {0, 255, 0}, {128, 0, 128}, {255, 0, 0}, {0, 0, 255}, {0, 0, 0}, {20, 20, 20}, {120, 120, 120}};
char font_paths[][PATH_MAX] = {"arial.ttf015", "arial.ttf030", "digital.ttf030", "digital.ttf025"};
char audio_paths[][PATH_MAX] = {"audio/botao.wav", "audio/sino.wav", "audio/fall.wav", "audio/end.wav"};
char exec_path[PATH_MAX];
ALLEGRO_BITMAP ** temporary_bitmaps = NULL;
int bitmaps_counter = 0;

/* As posicoes se referem à figura na orientacao para direita, sempre*/
/*	Esquerda para direita, conforme imagem
	I: (-2,1) (-1,1) (1,1) (2,1)
	J: (-1,1) (-1,0) (0,0) (1,0)
	L: (-1,0) (0,0) (1,0) (1,1)
	O: (-1,1) (-1,-1) (1,1) (1,-1)
	S: (-1,0) (0,0) (0,1) (1,1)
	T: (-1,0) (0,1) (0,0) (1,0)
	Z: (-1,1) (0,1) (0,0) (1,0)
*/
int positionsI[] = {-2, -1, -1, -1, 1, -1, 2, -1};
int positionsJ[] = {-1, -1, -1, 0, 0, 0, 1, 0};
int positionsL[] = {-1, 0, 0, 0, 1, 0, 1, -1};
int positionsO[] = {-1, -1, -1, 1, 1, -1, 1, 1};
int positionsS[] = {-1, 0, 0, 0, 0, -1, 1, -1};
int positionsT[] = {-1, 0, 0, -1, 0, 0, 1, 0};
int positionsZ[] = {-1, -1, 0, -1, 0, 0, 1, 0};

void set_vec2(VEC2 * vec, int x, int y) {
	vec->x = x;
	vec->y = y;
}

void copy_figures(FIGURE * f1, FIGURE f2) {
	int i;
	f1->type = f2.type;
	f1->color = f2.color;
	for(i = 0; i < 8; i++)
		f1->positions[i] = f2.positions[i];
}
void init_div(DIV * div, int x, int y, int w, int h, int thickness, enum COLORS border_color, enum COLORS background_color, DIV * parent) {
	div->x = x;
	div->y = y;
	div->w = w;
	div->h = h;
	div->thickness = thickness;
	div->xf = div->x + div->w;
	div->xc = (div->x + div->xf)/2;
	div->yc = (div->y + div->yf)/2;
	div->yf = div->y + div->h;
	div->border_color = border_color;
	div->background_color = background_color;
	div->parent = parent;
}

void draw_div_to_sub_bitmap(GAME_MANAGER * game, DIV div) {
	al_draw_filled_rectangle(0, 0, div.w, div.h, game->colors[div.background_color]);
	if(div.thickness) al_draw_rectangle(div.thickness/2, div.thickness/2, div.w - div.thickness/2, div.h - div.thickness/2, game->colors[div.border_color], div.thickness);
}

void draw_div(GAME_MANAGER * game, DIV div) {
	int x = 0, y = 0;
	DIV * d;
	
	for(d = &div; d->parent; d = d->parent) {
		x += d->parent->x;
		y += d->parent->y;
	}

	al_set_target_bitmap(game->display_bitmap);
	al_draw_filled_rectangle(x + div.x, y + div.y, x + div.x + div.w, y + div.y + div.h, game->colors[div.background_color]);
	if(div.thickness) al_draw_rectangle(x + div.x + div.thickness/2, y + div.y + div.thickness/2, x + div.x + div.w - div.thickness/2, y + div.y + div.h - div.thickness/2, game->colors[div.border_color], div.thickness);
}

void set_target_div(GAME_MANAGER * game, DIV div) {
	int x = 0, y = 0;
	DIV * d;

	for(d = &div; d->parent; d = d->parent) {
		x += d->parent->x;
		y += d->parent->y;
	}
	temporary_bitmaps = (ALLEGRO_BITMAP **) realloc(temporary_bitmaps, sizeof(ALLEGRO_BITMAP *) * (bitmaps_counter + 1));
	temporary_bitmaps[bitmaps_counter++] = al_create_sub_bitmap(game->display_bitmap, x + div.x, y + div.y, div.w, div.h);
	al_set_target_bitmap(temporary_bitmaps[bitmaps_counter - 1]);
}

void correct_positions(int * positions, int orientation) {
	int i;

	//0: canto inferior direito: todas os x tal que x < 0 devem ser somados em 1; todos os y tal que y > 0 devem ser reduzidos em 1
	//1: canto superior direito: todas os x tal que x < 0 devem ser somados em 1; todos os y tal que y < 0 devem ser somados em 1
	//2: canto superior esquerdo: todas os x tal que x > 0 devem ser reduzidos em 1; todos os y tal que y < 0 devem ser somados em 1
	//3: canto inferior esquerdo: todas os x tal que x > 0 devem ser reduzidos em 1; todos os y tal que y > 0 devem ser reduzidos em 1

	if(orientation < 2) { //aumenta o x
		for(i = 0; i < 4; ++i)
			if(positions[2*i] < 0) positions[2*i]++;
	}
	else { //reduz o x
		for(i = 0; i < 4; ++i)
			if(positions[2*i] > 0) positions[2*i]--;
	}
	if(orientation == 1 || orientation == 2) { //aumenta o y
		for(i = 0; i < 4; ++i)
			if(positions[2*i + 1] < 0) positions[2*i + 1]++;
	}
	else { //reduz o y
		for(i = 0; i < 4; ++i)
			if(positions[2*i + 1] > 0) positions[2*i + 1]--;
	}
}

void print_matrix(unsigned char ** matrix, int w, int h) {
	int i, j;
	for(i = 0; i < h; ++i) {
		for(j = 0; j < w; ++j){
			printf("%u\n", matrix[i][j]);
		}
	}
}

void draw_HUD(GAME_MANAGER * game) {

	al_set_target_bitmap(game->display_bitmap);
	al_clear_to_color(game->colors[GREY]);

	al_set_target_bitmap(game->game_screen_bitmap);
	draw_div_to_sub_bitmap(game, game->game_screen);
	al_set_target_bitmap(game->next_figure_bitmap);
	draw_div_to_sub_bitmap(game, game->next_figure);
	al_draw_text(game->fonts[ARIAL_15], game->colors[WHITE], game->next_figure.w/2, 0 + game->next_figure.thickness, ALLEGRO_ALIGN_CENTER, "Next");
}

void draw_block_matrix(GAME_MANAGER * game) {
	int i, j;

	al_set_target_bitmap(game->game_screen_bitmap);
	for(i = 0; i < BLOCKS_IN_COLUMN; ++i) {
		for(j = 0; j < BLOCKS_IN_ROW; ++j) {
			if(game->block_matrix[i][j])
				al_draw_filled_rectangle(j*BLOCK_WIDTH + game->game_screen.thickness, i*BLOCK_HEIGHT + game->game_screen.thickness, (j+1)*BLOCK_WIDTH + game->game_screen.thickness, (i+1)*BLOCK_HEIGHT + game->game_screen.thickness, game->colors[game->block_matrix[i][j]]);
		}
	}
}

void draw_next_figure(GAME_MANAGER * game) {
	VEC2 center;
	int positions[8], i;

	al_set_target_bitmap(game->next_figure_bitmap);

	memcpy(positions, game->next.positions, sizeof(int) * 8);
	set_vec2(&center, game->next_figure.w/2, al_get_font_line_height(game->fonts[DIGITAL]) + (al_get_font_line_height(game->fonts[DIGITAL]) + game->next_figure.h)/2);
	if(game->next.type == 'I') center.x -= BLOCK_WIDTH/2, center.y += BLOCK_HEIGHT/2;;
	if(game->next.type == 'O') center.x -= BLOCK_WIDTH/2;

	if(game->next.type == 'O' || game->next.type == 'I')
		correct_positions(positions, 0); //orientacao é sempre para a direita (0)
	
	/* Desenha as figuras tendo o centro da posicao delas*/
	for(i = 0; i < 4; ++i) {
		al_draw_filled_rectangle(center.x + positions[2*i]*BLOCK_WIDTH - BLOCK_WIDTH/2 , center.y + positions[2*i + 1]*BLOCK_HEIGHT - BLOCK_HEIGHT/2 ,
			center.x + positions[2*i]*BLOCK_WIDTH + BLOCK_WIDTH/2 , center.y + positions[2*i + 1]*BLOCK_HEIGHT + BLOCK_HEIGHT/2 , game->colors[game->next.color]);

	}
}

void draw_curr_figure(GAME_MANAGER * game) {
	int positions[8], i;
	al_set_target_bitmap(game->game_screen_bitmap);

	memcpy(positions, game->curr_figure.positions, sizeof(int) * 8);

	if(game->curr_figure.type == 'O' || game->curr_figure.type == 'I')
		correct_positions(positions, game->orientation);
	
	/* Desenha as figuras tendo o centro da posicao delas*/
	for(i = 0; i < 4; ++i) {
		al_draw_filled_rectangle((game->pos.x + positions[2*i])*BLOCK_WIDTH+ game->game_screen.thickness, (game->pos.y + positions[2*i + 1]) *BLOCK_HEIGHT+ game->game_screen.thickness,
			(game->pos.x + positions[2*i]+1)*BLOCK_WIDTH+ game->game_screen.thickness, (game->pos.y + positions[2*i + 1]+1)*BLOCK_HEIGHT+ game->game_screen.thickness, game->colors[game->curr_figure.color]);
	}
}

void update_display(GAME_MANAGER * game) {
	if(temporary_bitmaps) {
		free(temporary_bitmaps);
		temporary_bitmaps = NULL;
	}
	bitmaps_counter = 0;

	al_set_target_bitmap(al_get_backbuffer(game->display));
	al_draw_bitmap(game->display_bitmap, 0, 0, 0);
	al_flip_display();
}

void draw_info_stuff(GAME_MANAGER * game) {
	char score[14];
	char highscore[14];

	sprintf(score, "%d", game->score);
	sprintf(highscore, "%d", game->highscore);

	draw_div(game, game->info_div);
	draw_div(game, game->score_div);
	draw_div(game, game->highscore_div);
	set_target_div(game, game->score_div);
	al_draw_text(game->fonts[DIGITAL], game->colors[WHITE], game->score_div.w/2, game->score_div.h/2 - al_get_font_line_height(game->fonts[DIGITAL]), ALLEGRO_ALIGN_CENTER, score);
	set_target_div(game, game->highscore_div);
	al_draw_text(game->fonts[DIGITAL_25], game->colors[DARK_BLUE], game->highscore_div.w/2, (game->highscore_div.h)/2 - al_get_font_line_height(game->fonts[DIGITAL_25]) - 12, ALLEGRO_ALIGN_CENTER, "HIGH");
	al_draw_text(game->fonts[DIGITAL], game->colors[DARK_BLUE], game->highscore_div.w/2, (game->highscore_div.h)/2, ALLEGRO_ALIGN_CENTER, highscore);
}

void draw_game_over(GAME_MANAGER * game) {
	draw_HUD(game);
	draw_info_stuff(game);
	al_set_target_bitmap(game->game_screen_bitmap);
	al_draw_text(game->fonts[ARIAL_30], game->colors[WHITE], game->game_screen.w/2, DISPLAY_HEIGHT/2 - al_get_font_line_height(game->fonts[ARIAL_30]), ALLEGRO_ALIGN_CENTER, "GAME OVER");
	al_draw_text(game->fonts[ARIAL_15], game->colors[WHITE], game->game_screen.w/2, DISPLAY_HEIGHT/2, ALLEGRO_ALIGN_CENTER, "PRESS SPACE TO RESTART");

	update_display(game);
}

void draw_game(GAME_MANAGER * game) {
	draw_HUD(game);
	draw_block_matrix(game);
	draw_next_figure(game);
	draw_curr_figure(game);
	draw_info_stuff(game);
	update_display(game);
}

void splash_screen(GAME_MANAGER * game) {
	char title[] = "TETRIS";
	char subtitle[] = "Por Lucas Turci";

	float xc = DISPLAY_WIDTH/2;
	float yc = DISPLAY_HEIGHT/2;

	al_set_target_bitmap(al_get_backbuffer(game->display));
	al_clear_to_color(game->colors[WHITE]);
	al_draw_text(game->fonts[ARIAL_30], game->colors[BLACK], xc, yc - al_get_font_line_height(game->fonts[ARIAL_30]), ALLEGRO_ALIGN_CENTER, title);
	al_draw_text(game->fonts[ARIAL_15], game->colors[BLACK], xc, yc, ALLEGRO_ALIGN_CENTER, subtitle);
	al_flip_display();
	al_rest(3.0);
}

void main_menu(GAME_MANAGER * game) {
	char title[] = "PRESS SPACE TO START";
	char highscore[20];

	sprintf(highscore, "Highscore: %d", game->highscore);
	al_clear_to_color(game->colors[BLACK]);
	al_draw_text(game->fonts[ARIAL_30], game->colors[WHITE], DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 - al_get_font_line_height(game->fonts[ARIAL_30]), ALLEGRO_ALIGN_CENTER, title);
	al_draw_text(game->fonts[DIGITAL_25], game->colors[WHITE], DISPLAY_WIDTH/2, 10, ALLEGRO_ALIGN_CENTER, highscore);
	al_flip_display();

}

/* Posicoes das figuras: as posicoes - ou pivots - das figuras sao posicoes-de-blocos para todas as figuras exceto as dos tipos I e O. No caso destes, 
em que os pivots são pontos e nao blocos em si, refere-se sempre ao canto inferior direito de um bloco. Isto é, se eu falar que o pivot da figura
que é do tipo I é (4, 5), o pivot é na verdade o canto inferior direito do bloco das coordenadas (4, 5) */
void create_figure(FIGURE * f, char type) {
	f->type = type;

	switch(type) {
		case 'I':
			f->color = CYAN;
			memcpy(f->positions, positionsI, sizeof(int) * 8);
			break;
		case 'J':
			f->color = DARK_BLUE;
			memcpy(f->positions, positionsJ, sizeof(int) * 8);
			break;
		case 'L':
			f->color = ORANGE;
			memcpy(f->positions, positionsL, sizeof(int) * 8);
			break;
		case 'O':
			f->color = YELLOW;
			memcpy(f->positions, positionsO, sizeof(int) * 8);
			break;
		case 'S':
			f->color = GREEN;
			memcpy(f->positions, positionsS, sizeof(int) * 8);
			break;
		case 'T':
			f->color = VIOLET;
			memcpy(f->positions, positionsT, sizeof(int) * 8);
			break;
		case 'Z':
			f->color = RED;
			memcpy(f->positions, positionsZ, sizeof(int) * 8);
			break;
		default:
			break;	
	}
}

void random_figure(FIGURE * fig) {
	char figures[] = {'I', 'J', 'L', 'O', 'S', 'T', 'Z'};
	char figure_type;

	figure_type = figures[rand()%7];
	create_figure(fig, figure_type);
}


void start_game(GAME_MANAGER * game) {
	srand(time(NULL));
	rand();
	random_figure(&game->curr_figure);
	random_figure(&game->next);
	game->score = 0;
	al_set_timer_speed(game->fall_timer, (game->speed = INITIAL_SPEED));
	game->orientation = 0;
	set_vec2(&game->pos, (BLOCKS_IN_ROW - 1)/2, 1);

	al_start_timer(game->loop_timer);
	al_start_timer(game->fall_timer);
}

void restart_game(GAME_MANAGER * game) {
	int i;
	while(!queue_isEmpty(game->next_moves)) queue_pop(&game->next_moves, NULL);
	for(i = 0; i < BLOCKS_IN_COLUMN; ++i)
		memset(game->block_matrix[i], 0, sizeof(unsigned char) * BLOCKS_IN_ROW);
	start_game(game);
	game->running = 1;
}

GAME_MANAGER * create_game() {
	FILE * fp;
	int i, font_size;
	char path[PATH_MAX], *p;
	GAME_MANAGER * game = (GAME_MANAGER *) calloc(1, sizeof(GAME_MANAGER));

	
	sprintf(path, "%s/.highscore", exec_path);
	/* Highscore*/
	fp = fopen(path, "ra");
	if(fp) {
		fscanf(fp, "%d", &game->highscore);
		fclose(fp);
	}

	

	al_install_keyboard();
	game->display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);

	/* Fonts */
	al_init_font_addon();
	al_init_ttf_addon();
	for(i = 0; i < NFONTS; ++i) {
		sprintf(path, "%s/%s", exec_path, font_paths[i]);
		p = path + strlen(path) - 3;
		font_size = atoi(p);
		*p = 0;
		game->fonts[i] = al_load_font(path, font_size, 0); 
	}

	/* Colors */
	for(i = 0; i < NCOLORS; ++i)
		game->colors[i] = al_map_rgb(color_rgbs[i][0], color_rgbs[i][1], color_rgbs[i][2]);

	/* Timers and Events */
	game->timer_queue = al_create_event_queue();
	game->input_queue = al_create_event_queue();
	game->events_queue = al_create_event_queue();

	game->loop_timer = al_create_timer(LOOP_FREQ);
	game->fall_timer = al_create_timer(INITIAL_SPEED);
	game->regressive_timer = al_create_timer(1.0);
	game->another_timer = al_create_timer(3.0);

	game->next_moves = queue_create();

	al_register_event_source(game->input_queue, al_get_keyboard_event_source());
	al_register_event_source(game->input_queue, al_get_display_event_source(game->display));
	al_register_event_source(game->timer_queue, al_get_timer_event_source(game->loop_timer));
	al_register_event_source(game->events_queue, al_get_timer_event_source(game->fall_timer));
	al_register_event_source(game->events_queue, al_get_timer_event_source(game->regressive_timer));
	al_register_event_source(game->events_queue, al_get_timer_event_source(game->another_timer));

	/* Audio */
	al_install_audio();
	al_init_acodec_addon();
	al_init_primitives_addon();
	al_reserve_samples(5);

	sprintf(path, "%s/%s", exec_path, "/audio/music.wav");
	game->music.sample = al_load_sample(path);
	game->music.sample_instance = al_create_sample_instance(game->music.sample);
	al_attach_sample_instance_to_mixer(game->music.sample_instance, al_get_default_mixer());
	al_set_sample_instance_playmode(game->music.sample_instance, ALLEGRO_PLAYMODE_LOOP);

	for(i = 0; i < NSOUNDS; ++i) {
		sprintf(path, "%s/%s", exec_path, audio_paths[i]);
		game->sounds[i] = al_load_sample(path); 
	}


	/* Game */
	game->block_matrix = (unsigned char **) malloc(sizeof(unsigned char *) * BLOCKS_IN_COLUMN);
	for(i = 0; i < BLOCKS_IN_COLUMN; ++i)
		game->block_matrix[i] = (unsigned char *) calloc(BLOCKS_IN_ROW, sizeof(unsigned char));


	init_div(&game->game_screen, 0, 0, DISPLAY_WIDTH - BLOCK_WIDTH*4 - BORDER_THICKNESS, DISPLAY_HEIGHT, BORDER_THICKNESS, WHITE, GREY, NULL);
	init_div(&game->next_figure, DISPLAY_WIDTH - BLOCK_WIDTH*4 - BORDER_THICKNESS*2, 0, BLOCK_WIDTH*4 + 2*BORDER_THICKNESS, BLOCK_HEIGHT*4 + 2*BORDER_THICKNESS + al_get_font_line_height(game->fonts[DIGITAL]), BORDER_THICKNESS, WHITE, GREY, NULL);
	init_div(&game->info_div, game->game_screen.w, game->next_figure.h, BLOCK_WIDTH*4 + BORDER_THICKNESS, DISPLAY_HEIGHT - game->next_figure.h, 0, 0, GREY, NULL);
	init_div(&game->score_div, 0, 0, game->info_div.w, 60, /*espessura da borda*/0, /*cor da borda*/0, GREY, &game->info_div);
	init_div(&game->highscore_div, 0, game->score_div.h, game->info_div.w, al_get_font_line_height(game->fonts[DIGITAL_25]) + 60, /*espessura da borda*/0, /*cor da borda*/0, SILVER, &game->info_div);
	
	al_set_new_bitmap_flags(ALLEGRO_NO_PRESERVE_TEXTURE);
	game->display_bitmap = al_create_bitmap(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	game->game_screen_bitmap = al_create_sub_bitmap(game->display_bitmap, game->game_screen.x, game->game_screen.y, game->game_screen.w, game->game_screen.h);
	game->next_figure_bitmap = al_create_sub_bitmap(game->display_bitmap, game->next_figure.x, game->next_figure.y, game->next_figure.w, game->next_figure.h);

	splash_screen(game);

	return game;
}

void pause_game(GAME_MANAGER * game) {
	game->paused = !game->paused;
	al_play_sample(game->sounds[PAUSE_SOUND], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	if(game->paused) {
		game->running = 0;

		al_stop_timer(game->loop_timer);
		al_stop_timer(game->fall_timer);
		al_set_target_bitmap(game->game_screen_bitmap);
		al_draw_text(game->fonts[ARIAL_30], game->colors[WHITE], game->game_screen.w/2, DISPLAY_HEIGHT/2 - al_get_font_line_height(game->fonts[ARIAL_30])/2, ALLEGRO_ALIGN_CENTER, "PAUSED"); 
		update_display(game);

		game->music.sample_instance_position = al_get_sample_instance_position(game->music.sample_instance);
		al_stop_sample_instance(game->music.sample_instance);
	}
	else {
		game->running = 1;
		al_resume_timer(game->loop_timer);
		al_resume_timer(game->fall_timer);
		al_set_sample_instance_position(game->music.sample_instance, game->music.sample_instance_position);
		al_play_sample_instance(game->music.sample_instance);
	}
}

void process_input(GAME_MANAGER * game) {
	ALLEGRO_EVENT evt;

	while(al_get_next_event(game->input_queue, &evt)) {
		if(evt.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (evt.keyboard.keycode) {
				case ALLEGRO_KEY_RIGHT:
					if(game->running) queue_push(&game->next_moves, 0);
					break;
				case ALLEGRO_KEY_LEFT:
					if(game->running) queue_push(&game->next_moves, 1);
					break;
				case ALLEGRO_KEY_DOWN:
					if(game->running) queue_push(&game->next_moves, 2);
					break;	
				case ALLEGRO_KEY_A:
				case ALLEGRO_KEY_UP:
					if(game->running) queue_push(&game->next_moves, 3); //roda anti-horario
					break;
				case ALLEGRO_KEY_D:
					if(game->running) queue_push(&game->next_moves, 4); //roda horario
					break;
				case ALLEGRO_KEY_SPACE:
					switch(game->state) {
						case 1:
							game->running = 1;
							game->state = 2;
							break;
						case 4: //jogo rodando
							pause_game(game);
							break;
						default:
							break;
						case 7:
							game->state = 2;
							restart_game(game);
					}
				default:
					break;
			}

		}
		else if(evt.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			game->should_abort = 1;
	}
}

int check_collision(GAME_MANAGER * game) {
	int i, positions[8];
	VEC2 pos;
	memcpy(positions, game->curr_figure.positions, sizeof(int) * 8);
	if(game->curr_figure.type == 'I' || game->curr_figure.type == 'O')
		correct_positions(positions, game->orientation);
	for(i = 0; i < 4; ++i) {
		set_vec2(&pos, game->pos.x + positions[2*i], game->pos.y + positions[2*i + 1]);

		if(pos.x < 0 || pos.x >= BLOCKS_IN_ROW || pos.y < 0 || pos.y >= BLOCKS_IN_COLUMN || game->block_matrix[pos.y][pos.x]) return 1;
	}

	return 0;
}

void swap(int * array, int x, int y) {
	int a = array[x];
	array[x] = array[y];
	array[y] = a;
}

void rotate_figure(GAME_MANAGER * game, int direction) { //1 anti-horario, -1 horario
	VEC2 prev_pos = game->pos;
	int prev_positions[8], i;
	memcpy(prev_positions, game->curr_figure.positions, sizeof(int) * 8);

	for(i = 0; i < 4; ++i) {
		swap(game->curr_figure.positions, 2*i, 2*i + 1);
		direction == -1? (game->curr_figure.positions[2*i] = -game->curr_figure.positions[2*i]) : (game->curr_figure.positions[2*i + 1] = -game->curr_figure.positions[2*i + 1]);
	}

	if(game->curr_figure.type == 'O' || game->curr_figure.type == 'I') {
		switch(game->orientation) {
			case 0: 
				if(direction == 1) game->pos.y -= 1;
				else game->pos.x += 1;
				break;
			case 1: 
				if(direction == 1) game->pos.x += 1;
				else game->pos.y += 1;
				break;
			case 2: 
				if(direction == 1) game->pos.y += 1;
				else game->pos.x -= 1;
				break;
			case 3: 
				if(direction == 1) game->pos.x -= 1;
				else game->pos.y -= 1;
				break;
		}
	}
	game->orientation = (game->orientation + direction)%4;
	if(game->orientation == -1) game->orientation = 3;

	if(check_collision(game)) {//nao pode rodar
		game->orientation = (game->orientation - direction) % 4;
		game->pos = prev_pos;
		memcpy(game->curr_figure.positions, prev_positions, sizeof(int) * 8);
	}
}

int check_row_complete(GAME_MANAGER * game, int row) {
	int i;

	if(row < 0 || row >= BLOCKS_IN_COLUMN) return 0;
	for(i = 0; i < BLOCKS_IN_ROW; ++i)
		if(!game->block_matrix[row][i]) return 0;
	return 1;
}

int check_row_empty(GAME_MANAGER * game, int row) {
	int i;

	if(row < 0 || row >= BLOCKS_IN_COLUMN) return 0;
	for(i = 0; i < BLOCKS_IN_ROW; ++i)
		if(game->block_matrix[row][i]) return 0;
	return 1;
}

void game_loop(GAME_MANAGER * game) {
	int done = 0;
	ALLEGRO_EVENT evt;
	int countdown, rows_counter;
	while(1) {
		process_input(game);

		if(game->should_abort) return;

		switch(game->state) {
			case 0: // Imprime tela inicial
				main_menu(game);
				game->state = 1;
				break;
			case 1: // Nao faz nada: espera apertar espaço
				break;
			case 2: // Imprime o HUD			
				game->state = 3;
				countdown = 3;
				
				al_play_sample(game->sounds[BELL_SOUND], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				
				draw_HUD(game);
				draw_info_stuff(game);
				al_set_target_bitmap(game->game_screen_bitmap);
				al_draw_text(game->fonts[DIGITAL], game->colors[WHITE], game->game_screen.w/2, game->game_screen.h/2 - al_get_font_line_height(game->fonts[DIGITAL]), ALLEGRO_ALIGN_CENTER, "3");
				update_display(game);
				al_start_timer(game->regressive_timer);
				break;
			case 3: //3.. 2.. 1..
				done = 1;

				if(al_get_next_event(game->events_queue, &evt)) {
					if(evt.timer.source == game->regressive_timer) {
						--countdown;
						if(!countdown) { //vai comecar o jogo
							al_stop_timer(game->regressive_timer);
							game->state = 4;
							start_game(game);
							al_play_sample_instance(game->music.sample_instance);
							continue;
						}
						else {
							char string_counter[2];
							sprintf(string_counter, "%d", countdown);

							al_play_sample(game->sounds[BELL_SOUND], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

							draw_HUD(game);
							draw_info_stuff(game);
							al_set_target_bitmap(game->game_screen_bitmap);
							al_draw_text(game->fonts[DIGITAL], game->colors[WHITE], game->game_screen.w/2, game->game_screen.h/2 - al_get_font_line_height(game->fonts[DIGITAL]), ALLEGRO_ALIGN_CENTER, string_counter);
							update_display(game);
						}
					}
				}
				break;
			case 4: //rodando
				if(game->paused) continue;
				al_wait_for_event(game->timer_queue, &evt);

				if(evt.type == ALLEGRO_EVENT_TIMER && evt.timer.source == game->loop_timer) {
					int move, i, positions[8];
					VEC2 prev_pos;

					process_input(game);
					if(game->paused) continue;
					prev_pos = game->pos;

					if(al_get_next_event(game->events_queue, &evt)){
						if(evt.type == ALLEGRO_EVENT_TIMER && evt.timer.source == game->fall_timer) {
							queue_push(&game->next_moves, 5);
						}
					}

					if(!queue_pop(&game->next_moves, &move)) {
						switch(move) {
							case 0: //direita
								game->pos.x++;
								if(check_collision(game)) game->pos = prev_pos;
								break;
							case 1: //esquerda
								game->pos.x--;
								if(check_collision(game)) game->pos = prev_pos;
								break;
							case 2: //baixo
								do {
									prev_pos = game->pos;
									game->pos.y++;
								} while(!check_collision(game));
								al_play_sample(game->sounds[FALL_SOUND], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

								break;
							case 3: //roda anti-horario
								rotate_figure(game, 1); //tambem ja ve se nao pode rodar, e caso positivo, desfaz
								break;
							case 4: //roda horario
								rotate_figure(game, -1); //tambem ja ve se nao pode rodar, e caso positivo, desfaz
								break;
							case 5:
								game->pos.y++;
								break;
							default:
								break;
						}
					}

					if(check_collision(game)) {
						game->pos = prev_pos;
						memcpy(positions, game->curr_figure.positions, 8 * sizeof(int));

						if(game->curr_figure.type == 'I' || game->curr_figure.type == 'O')
							correct_positions(positions, game->orientation);

						for(i = 0; i < 4; ++i)
							game->block_matrix[game->pos.y + positions[2*i+1]][game->pos.x + positions[2*i]] = game->curr_figure.color;
						
						rows_counter = 0;
						for(i = 2; i >= -2; --i) {
							if(check_row_complete(game, prev_pos.y + i)) {
								rows_counter++;
								game->score += 100 * rows_counter;
								if(game->score > game->highscore) {
									game->should_update_highscore = 1;
									game->highscore = game->score;
								}
								memset(game->block_matrix[prev_pos.y+i], 0, sizeof(unsigned char) * BLOCKS_IN_ROW);
							}
						}

						if(rows_counter) al_set_timer_speed(game->fall_timer, (game->speed -= ACCELERATION)); //aumenta a velocidade
						if(game->speed < MAX_SPEED) al_set_timer_speed(game->fall_timer, (game->speed = MAX_SPEED)); 

						for(i = prev_pos.y + 2; i >= prev_pos.y - 2 && rows_counter; --i) {
							if(check_row_empty(game, i)) {
								for(int j = i-1; j >= 0; --j)
									memcpy(game->block_matrix[j+1], game->block_matrix[j], sizeof(unsigned char) * BLOCKS_IN_ROW);
								++i;
								--rows_counter;
							}
						}

						copy_figures(&game->curr_figure, game->next);
						random_figure(&game->next);

						game->orientation = 0;
						set_vec2(&game->pos, (BLOCKS_IN_ROW - 1)/2, 1);

						if(check_collision(game)) // You lose
							game->state = 5;
					}
					if(game->state != 5) draw_game(game); // O jogo nao acabou

				}
				break;
			case 5: //termina o jogo e inicia a musiquinha de game over
				al_stop_timer(game->fall_timer);
				al_stop_timer(game->loop_timer);
				game->running = false;

				al_stop_sample_instance(game->music.sample_instance);
				al_play_sample(game->sounds[END_SOUND], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				al_start_timer(game->another_timer);
				game->state = 6;
			case 6:	//espera a musiquinha de game over acabar (vai acabar quando o timer for disparado)
				if(al_get_next_event(game->events_queue, &evt)) {
					if(evt.type == ALLEGRO_EVENT_TIMER && evt.timer.source == game->another_timer) {
						al_stop_timer(game->another_timer);
						game->state = 7;
						draw_game_over(game);
					}
				}
				break;
			case 7: //espera apertar espaco para recomecar
				done = 0;
				break;
		}

	}
}

void register_score(GAME_MANAGER * game) {
	FILE * fp;
	char path[PATH_MAX];
	sprintf(path, "%s/.highscore", exec_path);

	struct stat st = {0};

	fp = fopen(path, "wa");
	if(!fp) {
		fprintf(stderr, "Could not register highscore: %d", game->highscore);
		return;
	}

	fprintf(fp, "%d", game->highscore);
	fclose(fp);
}

void terminate_game(GAME_MANAGER * game) {
	int i;
	if(game->should_update_highscore) register_score(game);

	for(i = 0; i < BLOCKS_IN_COLUMN; ++i)
		free(game->block_matrix[i]);
	free(game->block_matrix);

	for(i = 0; i < NFONTS; ++i)
		al_destroy_font(game->fonts[i]);

	/* Timers and Events */
	al_destroy_timer(game->loop_timer);
	al_destroy_timer(game->fall_timer);
	al_destroy_timer(game->regressive_timer);
	al_destroy_timer(game->another_timer);
	al_destroy_event_queue(game->timer_queue);
	al_destroy_event_queue(game->input_queue);

	/* Displays */
	al_destroy_bitmap(game->display_bitmap);
	al_destroy_bitmap(game->game_screen_bitmap);
	al_destroy_bitmap(game->next_figure_bitmap);
	al_destroy_display(game->display);

	/* Music and Audio */
	al_destroy_sample_instance(game->music.sample_instance);
	al_destroy_sample(game->music.sample);

	for(i = 0; i < NSOUNDS; ++i)
		al_destroy_sample(game->sounds[i]);

	/* Addons */
	al_shutdown_ttf_addon();	
	al_shutdown_primitives_addon();
	al_uninstall_audio();
	free(game);	
}


void get_exec_path(char * s) {
	int len = strlen(s);
	for(char * p = s + len; p != s && *p != '/'; p--) *p = 0;

}

int main(int argc, char * argv[]) {
	GAME_MANAGER * game; 

	strcpy(exec_path, argv[0]);
	get_exec_path(exec_path);
	al_init(); //configuration of the game

	game = create_game();
	game_loop(game);
	terminate_game(game);


	return 0;
}