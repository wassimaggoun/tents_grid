// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include "model.h"
#include "game.h"
#include "game_aux.h"
#include <SDL.h>
#include <SDL_image.h> // required to load transparent texture from PNG
#include <SDL_ttf.h>   // required to use TTF fonts
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* **************************************************************** */

struct Env_t {
  game g;
};

/* **************************************************************** */

Env* init(SDL_Window* win, SDL_Renderer* ren, int argc, char* argv[]) {
  Env* env = malloc(sizeof(struct Env_t));

  env->g = game_default();

  return env;
}

/* **************************************************************** */

void render(SDL_Window* win, SDL_Renderer* ren, Env* env) {
  /* PUT YOUR CODE HERE TO RENDER TEXTURES, ... */
  SDL_Rect rect;

  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  uint rows = game_nb_rows(env->g);
  uint cols = game_nb_cols(env->g);

  int width = w / cols;
  int height = h / rows;

  SDL_SetRenderDrawColor(ren, 255, 0, 0, SDL_ALPHA_OPAQUE);

  for (uint i = 0; i < cols; i++) {
    int pos_x = i * height;
    SDL_RenderDrawLine(ren, pos_x, 0, pos_x, height);
  }

  for (uint j = 0; j < rows; j++) {
    int pos_y = j * width;
    SDL_RenderDrawLine(ren, 0, pos_y, width, pos_y);
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