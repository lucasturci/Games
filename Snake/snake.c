#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <queue.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define DISPLAY_WIDTH 540
#define DISPLAY_HEIGHT 540

#define TILE_WIDTH 20
#define TILE_HEIGHT 20

#define SPEED 0.1
#define LINEAR_POS(i, j) (i * TILE_WIDTH + j)

#define BLOCK_WIDTH DISPLAY_WIDTH/TILE_WIDTH
#define BLOCK_HEIGHT DISPLAY_HEIGHT/TILE_HEIGHT

enum FONTS {
	ARIAL_15,
	ARIAL_30,
	DIGITAL,
	NFONTS
};

typedef struct {
	int x, y;
} TILE;

typedef struct {
	unsigned char running;
	unsigned char should_abort;
	unsigned char paused;
	
	int points;
	int highscore;
	int should_update_highscore;

	int snake_length;
	int dir;
	TILE pos;
	TILE pos_tail;
	TILE pos_food;
	QUEUE next_moves;
	int free_counter;
	unsigned char ** occupied;
	TILE * free_tiles; 

	ALLEGRO_DISPLAY * display;
	ALLEGRO_FONT * fonts[NFONTS];//[2];
	ALLEGRO_TIMER * timer;
	ALLEGRO_EVENT_QUEUE * timer_queue;

	ALLEGRO_EVENT_QUEUE * user_input_queue;
} GAME_MANAGER;

char exec_path[PATH_MAX];
ALLEGRO_COLOR white_color;
ALLEGRO_COLOR orange_color;
ALLEGRO_COLOR grey_color;
ALLEGRO_COLOR black_color;
ALLEGRO_COLOR blue_color;
ALLEGRO_COLOR dark_blue_color;
ALLEGRO_COLOR red_color;
ALLEGRO_COLOR green_color;
ALLEGRO_COLOR dark_green_color;


void remove_tile_of_array(TILE * array, int * counter, TILE t) {
	int i;
	for(i = 0; i < (*counter); ++i) {
		if(t.x == array[i].x && t.y == array[i].y) {
			array[i].x = array[(*counter)-1].x;
			array[i].y = array[(*counter)-1].y;
		}
	}
	(*counter)--;
}
void insert_tile_in_array(TILE * array, int *counter, TILE t) {
	(*counter)++;
	array[*counter-1].x = t.x;
	array[*counter-1].y = t.y;
}

GAME_MANAGER * create_game() {
	int i, j;
	FILE * fp;
	char font_paths[NFONTS][PATH_MAX];
	char path[PATH_MAX];
	TILE t;
	int highscore;
	sprintf(path, "%s/highscores/highscore.dat", exec_path);

	white_color = al_map_rgb(255, 255, 255);
	orange_color = al_map_rgb(125, 74, 20);
	grey_color = al_map_rgb(65, 65, 65);
	black_color = al_map_rgb(20, 20, 20);
	blue_color = al_map_rgb(0, 0, 255);
	dark_blue_color = al_map_rgb(0, 0, 60);
	red_color = al_map_rgb(255, 0, 0);
	green_color = al_map_rgb(0, 120, 0);
	dark_green_color = al_map_rgb(0, 100, 0);

	srand(time(NULL));
	GAME_MANAGER * game = (GAME_MANAGER *) calloc(1, sizeof(GAME_MANAGER));

	if((fp = fopen(path, "ra"))) {
		fscanf(fp, "%d", &highscore);
		game->highscore = highscore;
		fclose(fp);
	}
	else {
		game->highscore = -1;
		if (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
			fprintf(stderr, "Failed registering highscore");
	}

	game->pos.x = TILE_WIDTH/2 + 1;
	game->pos.y = game->pos_tail.y = TILE_HEIGHT/2 + 1;
	game->dir = 0;
	game->snake_length = 3;
	game->pos_tail.x = game->pos.x - game->snake_length + 1;
	game->next_moves = queue_create();
	game->occupied = (unsigned char **) malloc(sizeof(unsigned char *) * TILE_HEIGHT);
	game->free_tiles = (TILE *) malloc(sizeof(TILE) * TILE_WIDTH * TILE_HEIGHT);

	for(i = 0; i < TILE_HEIGHT; ++i) {
		game->occupied[i] = (unsigned char *) calloc(TILE_WIDTH, sizeof(unsigned char));
		for(j = 0; j < TILE_WIDTH; ++j) {
			game->free_tiles[LINEAR_POS(i, j)].x = j;
			game->free_tiles[LINEAR_POS(i, j)].y = i; 
		}
	}
	game->free_counter = TILE_WIDTH * TILE_HEIGHT;

	t = game->pos;
	for(i = 0; i < game->snake_length; ++i) {
		remove_tile_of_array(game->free_tiles, &game->free_counter, t);
		game->occupied[t.y][t.x] = 1;
		t.x--;
	}

	al_init_primitives_addon();
	al_init_ttf_addon();

	strcpy(font_paths[ARIAL_15], exec_path);
	strcat(font_paths[ARIAL_15], "arial.ttf");

	game->fonts[ARIAL_15] = al_load_ttf_font(font_paths[ARIAL_15], 15, 0);
	game->fonts[ARIAL_30] = al_load_ttf_font(font_paths[ARIAL_15], 30, 0);
	game->timer = al_create_timer(SPEED);
	game->timer_queue = al_create_event_queue();
	game->user_input_queue = al_create_event_queue();

	int new_pos_food = rand() % game->free_counter;
	game->pos_food = game->free_tiles[new_pos_food];

	al_register_event_source(game->timer_queue, al_get_timer_event_source(game->timer));

	game->display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	
	al_install_keyboard();
	al_register_event_source(game->user_input_queue, al_get_keyboard_event_source());
	al_register_event_source(game->user_input_queue, al_get_display_event_source(game->display));

	al_clear_to_color(al_map_rgba(255, 255, 255, 255));
	al_draw_text(game->fonts[ARIAL_30], al_map_rgba(0, 0, 0, 255), DISPLAY_WIDTH/2 - al_get_text_width(game->fonts[ARIAL_30], "SNAKE")/2, DISPLAY_HEIGHT/2 - 15, 0, "SNAKE");
	al_flip_display();
	al_rest(2.0);

	al_start_timer(game->timer);
	game->running = 1;
	return game;
}

void reset_game(GAME_MANAGER * game) {
	TILE t;
	int i, j;
	al_stop_timer(game->timer);

	game->points = 0;
	game->pos.x = TILE_WIDTH/2 + 1;
	game->pos.y = game->pos_tail.y = TILE_HEIGHT/2 + 1;
	game->dir = 0;
	game->snake_length = 3;
	game->pos_tail.x = game->pos.x - game->snake_length + 1;
	queue_free(&game->next_moves);
	game->next_moves = queue_create();

	for(i = 0; i < TILE_HEIGHT; ++i) {
		for(j = 0; j < TILE_WIDTH; ++j) {
			game->occupied[i][j] = 0;
			game->free_tiles[LINEAR_POS(i, j)].x = j;
			game->free_tiles[LINEAR_POS(i, j)].y = i; 
		}
	}
	game->free_counter = TILE_WIDTH * TILE_HEIGHT;


	t = game->pos;
	for(i = 0; i < game->snake_length; ++i) {
		remove_tile_of_array(game->free_tiles, &game->free_counter, t);
		game->occupied[t.y][t.x] = 1;
		t.x--;
	}

	int new_pos_food = rand() % game->free_counter;
	game->pos_food = game->free_tiles[new_pos_food];

	al_start_timer(game->timer);
	game->running = 1;

}
void pause_game(GAME_MANAGER * game) {
	game->paused = !game->paused;
	if(game->paused) al_stop_timer(game->timer);
	else al_resume_timer(game->timer);
}
void process_user_input(GAME_MANAGER * game) {
	ALLEGRO_EVENT evt;

	while(al_get_next_event(game->user_input_queue, &evt)) {
		if(evt.type == ALLEGRO_EVENT_KEY_DOWN) {
			if(!game->running && evt.keyboard.keycode != ALLEGRO_KEY_SPACE) continue;
			switch(evt.keyboard.keycode) {
				case ALLEGRO_KEY_RIGHT:
				case ALLEGRO_KEY_D:
					queue_push(&game->next_moves, 0);
					break;
				case ALLEGRO_KEY_UP:
				case ALLEGRO_KEY_W:
					queue_push(&game->next_moves, 1);
					break;
				case ALLEGRO_KEY_LEFT:
				case ALLEGRO_KEY_A:
					queue_push(&game->next_moves, 2);
					break;
				case ALLEGRO_KEY_DOWN:
				case ALLEGRO_KEY_S:
					queue_push(&game->next_moves, 3);
					break;
				case ALLEGRO_KEY_SPACE:
					if(!game->running) reset_game(game);
					else pause_game(game);
					break;
				default:
					break;
			}
		}
		else if(evt.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			game->should_abort = 1;
	}
}

void draw_game(GAME_MANAGER * game) {
	int i, j;
	char points[25];
	char highscore[25];
	float food_pos[] = {
		(game->pos_food.x + 0.8)*BLOCK_WIDTH, (game->pos_food.y + 0.5)*BLOCK_HEIGHT,
		(game->pos_food.x + 0.5)*BLOCK_WIDTH, (game->pos_food.y + 0.2)*BLOCK_HEIGHT,
		(game->pos_food.x + 0.2)*BLOCK_WIDTH, (game->pos_food.y + 0.5)*BLOCK_HEIGHT,
		(game->pos_food.x + 0.5)*BLOCK_WIDTH, (game->pos_food.y + 0.8)*BLOCK_HEIGHT
	};

	sprintf(points, "%d", game->points);
	if(game->points > game->highscore) {
		game->highscore = game->points;
		game->should_update_highscore = 1;
	}
	sprintf(highscore, "BEST:      %d", game->highscore);

	ALLEGRO_COLOR white_color = al_map_rgb(255, 255, 255);
	ALLEGRO_COLOR orange_color = al_map_rgb(75, 43, 10);
	ALLEGRO_COLOR grey_color = al_map_rgb(65, 65, 65);
	ALLEGRO_COLOR black_color = al_map_rgb(20, 20, 20);
	ALLEGRO_COLOR blue_color = al_map_rgb(0, 0, 255);
	ALLEGRO_COLOR dark_blue_color = al_map_rgb(0, 0, 60);
	ALLEGRO_COLOR red_color = al_map_rgb(255, 0, 0);
	ALLEGRO_COLOR green_color = al_map_rgb(0, 120, 0);
	ALLEGRO_COLOR dark_green_color = al_map_rgb(0, 100, 0);

	al_clear_to_color(black_color);
	for(i = 0; i < TILE_HEIGHT; ++i) {
		for(j = 0; j < TILE_WIDTH; ++j)
			if(game->occupied[i][j]) {
				al_draw_filled_rectangle(j * DISPLAY_WIDTH*1.0/TILE_WIDTH, i * DISPLAY_HEIGHT*1.0/TILE_HEIGHT,
					(j + 1)* 1.0*DISPLAY_WIDTH/TILE_WIDTH, (i + 1)* DISPLAY_HEIGHT*1.0/TILE_HEIGHT, dark_green_color);
				al_draw_filled_rectangle((j + 0.05)* DISPLAY_WIDTH*1.0/TILE_WIDTH, (i + 0.05)* DISPLAY_HEIGHT*1.0/TILE_HEIGHT,
					(j + 0.95)* 1.0*DISPLAY_WIDTH/TILE_WIDTH, (i + 0.95)* DISPLAY_HEIGHT*1.0/TILE_HEIGHT, green_color);
				if(game->pos.x == j && game->pos.y == i) {
					switch(game->dir) { //pinta os olhinhos
						case 0:
							al_draw_filled_rectangle((j + 0.75)* DISPLAY_WIDTH/TILE_WIDTH - 2, (i + 0.3) * DISPLAY_HEIGHT/TILE_HEIGHT - 2,
								(j + 0.75)* DISPLAY_WIDTH/TILE_WIDTH + 4, (i + 0.3) * DISPLAY_HEIGHT/TILE_HEIGHT + 4, black_color);
							al_draw_filled_rectangle((j + 0.75)* DISPLAY_WIDTH/TILE_WIDTH - 2, (i + 0.7) * DISPLAY_HEIGHT/TILE_HEIGHT - 2,
								(j + 0.75)* DISPLAY_WIDTH/TILE_WIDTH + 4, (i + 0.7) * DISPLAY_HEIGHT/TILE_HEIGHT + 4, black_color);
							break;
						case 1:
							al_draw_filled_rectangle((j + 0.3)* DISPLAY_WIDTH/TILE_WIDTH - 2, (i + 0.25) * DISPLAY_HEIGHT/TILE_HEIGHT - 2,
								(j + 0.3)* DISPLAY_WIDTH/TILE_WIDTH + 4, (i + 0.25) * DISPLAY_HEIGHT/TILE_HEIGHT + 4, black_color);
							al_draw_filled_rectangle((j + 0.7)* DISPLAY_WIDTH/TILE_WIDTH - 2, (i + 0.25) * DISPLAY_HEIGHT/TILE_HEIGHT - 2,
								(j + 0.7)* DISPLAY_WIDTH/TILE_WIDTH + 4, (i + 0.25) * DISPLAY_HEIGHT/TILE_HEIGHT + 4, black_color);
							break;
						case 2:
							al_draw_filled_rectangle((j + 0.25)* DISPLAY_WIDTH/TILE_WIDTH - 2, (i + 0.3) * DISPLAY_HEIGHT/TILE_HEIGHT - 2,
								(j + 0.25)* DISPLAY_WIDTH/TILE_WIDTH + 4, (i + 0.3) * DISPLAY_HEIGHT/TILE_HEIGHT + 4, black_color);
							al_draw_filled_rectangle((j + 0.25)* DISPLAY_WIDTH/TILE_WIDTH - 2, (i + 0.7) * DISPLAY_HEIGHT/TILE_HEIGHT - 2,
								(j + 0.25)* DISPLAY_WIDTH/TILE_WIDTH + 4, (i + 0.7) * DISPLAY_HEIGHT/TILE_HEIGHT + 4, black_color);
							break;
						default:
							al_draw_filled_rectangle((j + 0.3)* DISPLAY_WIDTH/TILE_WIDTH -2 , (i + 0.75) * DISPLAY_HEIGHT/TILE_HEIGHT - 2,
								(j + 0.3)* DISPLAY_WIDTH/TILE_WIDTH + 4, (i + 0.75) * DISPLAY_HEIGHT/TILE_HEIGHT + 4, black_color);
							al_draw_filled_rectangle((j + 0.7)* DISPLAY_WIDTH/TILE_WIDTH - 2, (i + 0.75) * DISPLAY_HEIGHT/TILE_HEIGHT - 2,
								(j + 0.7)* DISPLAY_WIDTH/TILE_WIDTH + 4, (i + 0.75) * DISPLAY_HEIGHT/TILE_HEIGHT + 4, black_color);

					}
				}
			}
	}

	al_draw_filled_polygon(food_pos, 4, red_color);
	// al_draw_filled_circle((game->pos_food.x + 0.5)* DISPLAY_WIDTH/TILE_WIDTH, (game->pos_food.y + 0.5) * DISPLAY_HEIGHT/TILE_HEIGHT,
			// 0.4 *DISPLAY_HEIGHT/TILE_HEIGHT, red_color);

	if(game->running){
		al_draw_text(game->fonts[ARIAL_15], white_color, (DISPLAY_WIDTH/2), 20, ALLEGRO_ALIGN_CENTER, points);
		if(game->paused) al_draw_text(game->fonts[ARIAL_30], white_color, (DISPLAY_WIDTH/2) - al_get_text_width(game->fonts[ARIAL_30], "PAUSED")/2, DISPLAY_HEIGHT/2 - 15, 0,  "PAUSED");
	}
	else {
			sprintf(points, "SCORE:  %d", game->points);

			al_draw_text(game->fonts[ARIAL_30], white_color, (DISPLAY_WIDTH/2), DISPLAY_HEIGHT/2 - 32, ALLEGRO_ALIGN_CENTER, points);
			al_draw_text(game->fonts[ARIAL_30], blue_color, (DISPLAY_WIDTH/2), DISPLAY_HEIGHT/2 + 2, ALLEGRO_ALIGN_CENTER, highscore);
	}

	al_flip_display();
}

void game_loop(GAME_MANAGER * game) {
	ALLEGRO_EVENT evt;
	int i;
	int prev_dir, dir;
	int printed_gameover_screen;
	TILE t, prev_pos;

	while(1) {
		process_user_input(game);
		if(game->should_abort) break;

		if(game->running) {
			if(game->paused) {
				draw_game(game);
				continue;
			}
			al_wait_for_event(game->timer_queue, &evt);
     		if (!al_is_event_queue_empty(game->timer_queue)) continue;

			
			if(evt.type == ALLEGRO_EVENT_TIMER) { 
				process_user_input(game);
				
				//roda as coisas
				prev_dir = game->dir;
				prev_pos = game->pos;

				if(!queue_pop(&game->next_moves, &dir)) {
					if(game->dir != (dir + 2)%4) game->dir = dir;
				}

				switch(game->dir) {
					case 0:
						game->pos.x = (game->pos.x + 1) % TILE_WIDTH;
						break;
					case 1:
						game->pos.y = (game->pos.y - 1);
						if(game->pos.y < 0) game->pos.y = TILE_HEIGHT - 1;
						break;
					case 2:
						game->pos.x = (game->pos.x - 1);
						if(game->pos.x < 0) game->pos.x = TILE_WIDTH - 1;
						break;
					case 3:
						game->pos.y = (game->pos.y + 1) % TILE_HEIGHT;				
					default:
						break;
				}

				game->occupied[prev_pos.y][prev_pos.x] = 1 + game->dir;

				//colisoes
				if(game->occupied[game->pos.y][game->pos.x]) {
					game->pos = prev_pos;
					game->running = 0;
					printed_gameover_screen = 0;
					continue;
				}

				game->occupied[game->pos.y][game->pos.x] = 1;
				remove_tile_of_array(game->free_tiles, &game->free_counter, game->pos);


				if(game->pos.y == game->pos_food.y && game->pos.x == game->pos_food.x) {
					int new_pos_food = rand() % game->free_counter;
					game->pos_food = game->free_tiles[new_pos_food];
					++game->points;
				}
				else {
					t = game->pos_tail;
					switch(game->occupied[game->pos_tail.y][game->pos_tail.x]) {
						case 1:
							game->pos_tail.x = (game->pos_tail.x + 1) % TILE_WIDTH;
							break;
						case 2:
							game->pos_tail.y = (game->pos_tail.y - 1);
							if(game->pos_tail.y < 0) game->pos_tail.y = TILE_HEIGHT - 1;
							break;
						case 3:
							game->pos_tail.x = (game->pos_tail.x - 1);
							if(game->pos_tail.x < 0) game->pos_tail.x = TILE_WIDTH - 1;
							break;
						case 4:
							game->pos_tail.y = (game->pos_tail.y + 1) % TILE_HEIGHT;
						default:
							break;
					}
					
					game->occupied[t.y][t.x] = 0;
					insert_tile_in_array(game->free_tiles, &game->free_counter, t);
				}				

				draw_game(game);
			}
		}
		else {
			if(!printed_gameover_screen) draw_game(game);
			printed_gameover_screen = 1;
		}
	}
}

void register_score(GAME_MANAGER * game) {
	char path[PATH_MAX];
	FILE * fp;
	sprintf(path, "%s/highscores/highscore.dat", exec_path);

	fp = fopen(path, "wa");
	fprintf(fp, "%d", game->highscore);
	fclose(fp);

}

void terminate_game(GAME_MANAGER * game) {
	int i;

	if(game->should_update_highscore) register_score(game);

	for(i = 0; i < TILE_HEIGHT; ++i)
		free(game->occupied[i]);
	free(game->occupied);

	for(i = 0; i < NFONTS; ++i)
		al_destroy_font(game->fonts[i]);

	free(game->free_tiles);
	queue_free(&game->next_moves);

	al_destroy_timer(game->timer);
	al_destroy_event_queue(game->timer_queue);
	al_destroy_event_queue(game->user_input_queue);
	al_destroy_display(game->display);

	al_shutdown_ttf_addon();	
	al_shutdown_primitives_addon();
}

void get_exec_path(char * exec) {
	int i;
	for(i = strlen(exec) - 1; exec[i] != '/'; i--)
	exec[i] = '\0';
}

int main(int argc, char * argv[]) {
	GAME_MANAGER * game; 
	strcpy(exec_path, argv[0]);
	get_exec_path(exec_path);
	printf("%s\n", exec_path);

	al_init();

	game = create_game();
	game_loop(game);
	terminate_game(game);

	return 0;
}