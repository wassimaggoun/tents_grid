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

#define TREE "graphics/tree.png"
#define BIGTREE "graphics/bigtree.png"
#define TENT "graphics/tent.png"
#define GRASS "graphics/grass.png"
#define BACKGROUND "graphics/background.png"

/* **************************************************************** */

struct Env_t {
  game g;
  SDL_Texture* tree;
  SDL_Texture* tent;
  SDL_Texture* bigtree;
  SDL_Texture* grass;
  SDL_Texture* background;
};

/* **************************************************************** */

Env* init(SDL_Window* win, SDL_Renderer* ren, int argc, char* argv[]) {
  Env* env = malloc(sizeof(struct Env_t));

  env->g = game_new_empty_ext(10, 7, false);
  env->background = IMG_LoadTexture(ren, BACKGROUND);
  if (!env->background)
    ERROR("IMG_LoadTexture: %s\n", BACKGROUND);
  return env;
}

/* **************************************************************** */

void render(SDL_Window* win, SDL_Renderer* ren, Env* env) {
  /* PUT YOUR CODE HERE TO RENDER TEXTURES, ... */
  SDL_Rect rect;

  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_RenderCopy(ren, env->background, NULL, NULL);

  uint rows = game_nb_rows(env->g);
  uint cols = game_nb_cols(env->g);

  int width = w / cols;
  int height = h / rows;

  SDL_SetRenderDrawColor(ren, 255, 0, 0, SDL_ALPHA_OPAQUE);

  for (uint i = 0; i <= cols; i++) {
    int pos_x = i * width;
    SDL_RenderDrawLine(ren, pos_x, 0, pos_x, h);
  }

  for (uint j = 0; j <= rows; j++) {
    int pos_y = j * height;
    SDL_RenderDrawLine(ren, 0, pos_y, w, pos_y);
  }
}

/* **************************************************************** */

bool process(SDL_Window* win, SDL_Renderer* ren, Env* env, SDL_Event* e) {
  if (e->type == SDL_QUIT) {
    return true;
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