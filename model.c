// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include "model.h"
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"
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
  TTF_Font* font;
};

/* **************************************************************** */

Env* init(SDL_Window* win, SDL_Renderer* ren, int argc, char* argv[]) {
  Env* env = malloc(sizeof(struct Env_t));

  if (argc > 1) {
    env->g = game_load(argv[1]);
  } else {
    env->g = game_default();
  }
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
  env->bigtree = IMG_LoadTexture(ren, BIGTREE_IMG);
  if (!env->bigtree)
    ERROR("IMG_LoadTexture: %s\n", BIGTREE_IMG);

  env->font = TTF_OpenFont(FONT, FONTSIZE);
  if (!env->font)
    ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(env->font, TTF_STYLE_BOLD);
  SDL_Color color = {0, 0, 255, 255};                               
  SDL_Surface* surf = TTF_RenderText_Blended(env->font, "CONGRATS!!!", color); // blended rendering for ultra nice text
  env->text = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);




  return env;
}

/* **************************************************************** */

void render(SDL_Window* win, SDL_Renderer* ren, Env* env) {

  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  SDL_RenderCopy(ren, env->background, NULL, NULL);

  uint rows = game_nb_rows(env->g);
  uint cols = game_nb_cols(env->g);

  // Réserver 1 cellule à droite et en bas pour les compteurs
  int cell_w = w / (cols + 1);
  int cell_h = h / (rows + 1);

  int grid_w = cols * cell_w;
  int grid_h = rows * cell_h;

  int offset_x = 0;
  int offset_y = 0;

  // Cases
  for (uint i = 0; i < rows; i++) {
    for (uint j = 0; j < cols; j++) {
      square s = game_get_square(env->g, i, j);
      int x = offset_x + j * cell_w;
      int y = offset_y + i * cell_h;
      SDL_Rect rect = {x, y, cell_w, cell_h};
      if      (s == TREE)    SDL_RenderCopy(ren, env->tree,    NULL, &rect);
      else if (s == TENT)    SDL_RenderCopy(ren, env->tent,    NULL, &rect);
      else if (s == GRASS)   SDL_RenderCopy(ren, env->grass,   NULL, &rect);
      else if (s == BIGTREE) SDL_RenderCopy(ren, env->bigtree, NULL, &rect);
    }
  }

  // Grille
  SDL_SetRenderDrawColor(ren, 255, 0, 0, SDL_ALPHA_OPAQUE);

  for (uint i = 0; i <= cols; i++) {
    int pos_x = offset_x + i * cell_w;
    SDL_RenderDrawLine(ren, pos_x, offset_y, pos_x, offset_y + grid_h);
  }

  for (uint j = 0; j <= rows; j++) {
    int pos_y = offset_y + j * cell_h;
    SDL_RenderDrawLine(ren, offset_x, pos_y, offset_x + grid_w, pos_y);
  }

  // Compteurs lignes (droite)
  SDL_Color white = {255, 255, 255, 255};
  SDL_Color red   = {255, 50,  50,  255};
  for (uint i = 0; i < rows; i++) {
    uint exp = game_get_expected_nb_tents_row(env->g, i);
    uint cur = game_get_current_nb_tents_row(env->g, i);
    SDL_Color col = (cur == exp) ? white : red;
    char buf[8]; snprintf(buf, sizeof(buf), "%u", exp);
    SDL_Surface* surf = TTF_RenderText_Blended(env->font, buf, col);
    SDL_Texture* tex  = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    int tw, th; SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
    SDL_Rect dst = {offset_x + grid_w + (cell_w-tw)/2,
                    offset_y + i*cell_h + (cell_h-th)/2, tw, th};
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
  }

  // Compteurs colonnes (bas)
  for (uint j = 0; j < cols; j++) {
    uint exp = game_get_expected_nb_tents_col(env->g, j);
    uint cur = game_get_current_nb_tents_col(env->g, j);
    SDL_Color col = (cur == exp) ? white : red;
    char buf[8]; snprintf(buf, sizeof(buf), "%u", exp);
    SDL_Surface* surf = TTF_RenderText_Blended(env->font, buf, col);
    SDL_Texture* tex  = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    int tw, th; SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
    SDL_Rect dst = {offset_x + j*cell_w + (cell_w-tw)/2,
                    offset_y + grid_h + (cell_h-th)/2, tw, th};
    SDL_RenderCopy(ren, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
  }

  // Overlay rouge sur tentes invalides
  SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(ren, 255, 0, 0, 120);
  for (uint i = 0; i < rows; i++) {
    for (uint j = 0; j < cols; j++) {
      if (game_get_square(env->g, i, j) == TENT &&
          !game_check_move(env->g, i, j, TENT)) {
        SDL_Rect rect = {offset_x + (int)j*cell_w,
                         offset_y + (int)i*cell_h, cell_w, cell_h};
        SDL_RenderFillRect(ren, &rect);
      }
    }
  }
  SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);

  // Victoire
  if (env->won) {
    SDL_Rect rect = {w/2-100, h/2-25, 200, 50};
    SDL_RenderCopy(ren, env->text, NULL, &rect);
  }
}

/* **************************************************************** */

bool process(SDL_Window* win, SDL_Renderer* ren, Env* env, SDL_Event* e) {
  if (e->type == SDL_QUIT) {
    return true;
  }

  else if (e->type == SDL_MOUSEBUTTONDOWN) {
    if (env->won)
      return false;
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    int w, h;
    SDL_GetWindowSize(win, &w, &h);

    uint rows = game_nb_rows(env->g);
    uint cols = game_nb_cols(env->g);

    int cell_w = (w) / (cols+1);
    int cell_h = (h) / (rows+1);

    int grid_w = cols * cell_w;
    int grid_h = rows * cell_h;

    int offset_x = 0;
    int offset_y = 0;

    int col = (mouse_x - offset_x) / cell_w;
    int row = (mouse_y - offset_y) / cell_h;

    if (row < 0 || row >= rows || col < 0 || col >= cols)
      return false;

    if (e->button.button == SDL_BUTTON_LEFT) {

      square s = game_get_square(env->g, row, col);

      if (s == TENT) {
        game_play_move(env->g, row, col, EMPTY);
      } else {
        
        game_play_move(env->g, row, col, TENT);
        
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

    case SDLK_s:
      if (game_solve(env->g)) {
        env->won = true;
      }
      break;

    case SDLK_ESCAPE: return true;
    }
  }

  /* PUT YOUR CODE HERE TO PROCESS EVENTS */

  return false;
}

/* **************************************************************** */

void clean(SDL_Window* win, SDL_Renderer* ren, Env* env) {
  game_delete(env->g);

  SDL_DestroyTexture(env->tree);
  SDL_DestroyTexture(env->tent);
  SDL_DestroyTexture(env->bigtree);
  SDL_DestroyTexture(env->grass);
  SDL_DestroyTexture(env->background);
  SDL_DestroyTexture(env->text);
  TTF_CloseFont(env->font);

  free(env);
}

/* **************************************************************** */