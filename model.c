// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include "model.h"
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include <SDL.h>
#include <SDL_image.h> // required to load transparent texture from PNG
#include <SDL_ttf.h>   // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define FONT "res/arial.ttf"
#define FONTSIZE 36
#define TREE_IMG "res/tree.png"
#define BIGTREE_IMG "res/bigtree.png"
#define TENT_IMG "res/tent.png"
#define GRASS_IMG "res/grass.png"
#define BACKGROUND "res/background.png"

/* **************************************************************** */

struct Env_t {
  game g;
  bool won;
  SDL_Texture* tree;
  SDL_Texture* tent;
  SDL_Texture* bigtree;
  SDL_Texture* grass;
  SDL_Texture* background;
  SDL_Texture* text;
};

/* **************************************************************** */

Env* init(SDL_Window* win, SDL_Renderer* ren, int argc, char* argv[]) {
  Env* env = malloc(sizeof(struct Env_t));

  env->g = game_default();
  env->won = false;

  env->background = IMG_LoadTexture(ren, BACKGROUND);
  if (!env->background)
    ERROR("IMG_LoadTexture: %s\n", BACKGROUND);
  env->tree = IMG_LoadTexture(ren, TREE_IMG);
  if (!env->tree)
    ERROR("IMG_LoadTexture: %s\n", TREE_IMG);
  env->tent = IMG_LoadTexture(ren, TENT_IMG);
  if (!env->tent)
    ERROR("IMG_LoadTexture: %s\n", TENT_IMG);
  env->grass = IMG_LoadTexture(ren, GRASS_IMG);
  if (!env->grass)
    ERROR("IMG_LoadTexture: %s\n", GRASS_IMG);

  SDL_Color color = {0, 0, 255, 255};
  TTF_Font* font = TTF_OpenFont(FONT, FONTSIZE);
  if (!font)
    ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font, TTF_STYLE_BOLD);                                 // TTF_STYLE_ITALIC | TTF_STYLE_NORMAL
  SDL_Surface* surf = TTF_RenderText_Blended(font, "CONGRATS!!!", color); // blended rendering for ultra nice text
  env->text = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
  TTF_CloseFont(font);

  return env;
}

/* **************************************************************** */

void render(SDL_Window* win, SDL_Renderer* ren, Env* env) {
  /* PUT YOUR CODE HERE TO RENDER TEXTURES, ... */

  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_RenderCopy(ren, env->background, NULL, NULL);

  uint rows = game_nb_rows(env->g);
  uint cols = game_nb_cols(env->g);

  int cell_w = (w * 0.8) / cols;
  int cell_h = (h * 0.8) / rows;

  int grid_w = w * 0.8;
  int grid_h = h * 0.8;

  int offset_x = (w - grid_w) / 2;
  int offset_y = (h - grid_h) / 2;

  for (uint i = 0; i < rows; i++) {
    for (uint j = 0; j < cols; j++) {
      square s = game_get_square(env->g, i, j);
      int x = offset_x + j * cell_w;
      int y = offset_y + i * cell_h;
      SDL_Rect rect = {x, y, cell_w, cell_h};
      if (s == TREE) {
        SDL_RenderCopy(ren, env->tree, NULL, &rect);
      } else if (s == TENT) {
        SDL_RenderCopy(ren, env->tent, NULL, &rect);
      } else if (s == GRASS) {
        SDL_RenderCopy(ren, env->grass, NULL, &rect);
      }
    }
  }

  SDL_SetRenderDrawColor(ren, 255, 0, 0, SDL_ALPHA_OPAQUE);

  for (uint i = 0; i <= cols; i++) {
    int pos_x = offset_x + i * cell_w;
    SDL_RenderDrawLine(ren, pos_x, offset_y, pos_x, offset_y + grid_h);
  }

  for (uint j = 0; j <= rows; j++) {
    int pos_y = offset_y + j * cell_h;
    SDL_RenderDrawLine(ren, offset_x, pos_y, offset_x + grid_w, pos_y);
  }

  if (env->won) {
    SDL_Rect rect = {w / 2 - 100, h / 2 - 25, 200, 50};
    SDL_RenderCopy(ren, env->text, NULL, &rect);
  }
}

/* **************************************************************** */

bool process(SDL_Window* win, SDL_Renderer* ren, Env* env, SDL_Event* e) {
  if (e->type == SDL_QUIT) {
    return true;
  }

  else if (e->type == SDL_MOUSEBUTTONDOWN) {
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    int w, h;
    SDL_GetWindowSize(win, &w, &h);

    uint rows = game_nb_rows(env->g);
    uint cols = game_nb_cols(env->g);

    int cell_w = (w * 0.8) / cols;
    int cell_h = (h * 0.8) / rows;

    int grid_w = w * 0.8;
    int grid_h = h * 0.8;

    int offset_x = (w - grid_w) / 2;
    int offset_y = (h - grid_h) / 2;

    int col = (mouse_x - offset_x) / cell_w;
    int row = (mouse_y - offset_y) / cell_h;
    if (e->button.button == SDL_BUTTON_LEFT) {

      square s = game_get_square(env->g, row, col);

      if (s == TENT) {
        game_play_move(env->g, row, col, EMPTY);
      } else {
        if (game_check_move(env->g, row, col, TENT)) {
          game_play_move(env->g, row, col, TENT);
        }
      }
    }
    if (e->button.button == SDL_BUTTON_RIGHT) {

      square s = game_get_square(env->g, row, col);

      if (s == GRASS) {
        game_play_move(env->g, row, col, EMPTY);
      } else {
        game_play_move(env->g, row, col, GRASS);
      }
    }
    if (!env->won && game_won(env->g)) {
      env->won = true;
    }
  }

  else if (e->type == SDL_KEYDOWN) {
    switch (e->key.keysym.sym) {
    case SDLK_r:
      game_restart(env->g);
      env->won = false;
      break;

    case SDLK_u: game_undo(env->g); break;

    case SDLK_y: game_redo(env->g); break;

    case SDLK_ESCAPE: return true;
    }
  }

  /* PUT YOUR CODE HERE TO PROCESS EVENTS */

  return false;
}

/* **************************************************************** */

void clean(SDL_Window* win, SDL_Renderer* ren, Env* env) {
  /* PUT YOUR CODE HERE TO CLEAN MEMORY */

  free(env);
}

/* **************************************************************** */