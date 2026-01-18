#include "game_ext.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_struct.h"
#include "queue.h"

game game_new_ext(uint nb_rows, uint nb_cols, square* squares, uint* nb_tents_row, uint* nb_tents_col, bool wrapping)
{
  // Vérifie que les données fournies sont valides
  if (!squares || !nb_tents_row || !nb_tents_col) return NULL;

  // Alloue la mémoire pour la structure du jeu
  game g = malloc(sizeof(struct game_s));
  if (!g) return NULL;

  // Initialise les pointeurs à NULL pour éviter les bugs
  g->grid = NULL;
  g->expected_nb_tents_row = NULL;
  g->expected_nb_tents_col = NULL;
  g->undo_stack = NULL;
  g->redo_stack = NULL;

  // Configure la taille et l'option de wrapping
  g->height = nb_rows;
  g->width = nb_cols;
  g->wrapping = wrapping;

  // Alloue les lignes de la grille (calloc met tout à 0 pour la sécurité)
  g->grid = calloc(g->height, sizeof(square*));
  if (!g->grid) {
    free(g);
    return NULL;
  }

  // Alloue chaque colonne de la grille
  for (uint i = 0; i < g->height; i++) {
    g->grid[i] = malloc(g->width * sizeof(square));
    if (!g->grid[i]) {
      game_delete(g);  // Nettoie tout proprement si ça échoue
      return NULL;
    }
  }

  // Remplit la grille avec les cases fournies
  for (uint i = 0; i < g->height; i++) {
    for (uint j = 0; j < g->width; j++) {
      uint index = i * g->width + j;
      g->grid[i][j] = squares[index];
    }
  }

  // Alloue et copie les objectifs de tentes par ligne
  g->expected_nb_tents_row = malloc(g->height * sizeof(uint));
  if (!g->expected_nb_tents_row) {
    game_delete(g);
    return NULL;
  }
  for (uint i = 0; i < g->height; i++) {
    g->expected_nb_tents_row[i] = nb_tents_row[i];
  }

  // Alloue et copie les objectifs de tentes par colonne
  g->expected_nb_tents_col = malloc(g->width * sizeof(uint));
  if (!g->expected_nb_tents_col) {
    game_delete(g);
    return NULL;
  }
  for (uint j = 0; j < g->width; j++) {
    g->expected_nb_tents_col[j] = nb_tents_col[j];
  }

  // Crée les files pour l'historique (annuler/refaire)
  g->undo_stack = queue_new();
  g->redo_stack = queue_new();
  if (!g->undo_stack || !g->redo_stack) {
    game_delete(g);
    return NULL;
  }

  return g;
}

/* *********************************************************** */

game game_new_empty_ext(uint nb_rows, uint nb_cols, bool wrapping)
{
  // Calcule la taille totale
  uint size = nb_cols * nb_rows;

  // Prépare un tableau vide pour les cases
  square squares[size];
  for (uint i = 0; i < size; i++) {
    squares[i] = EMPTY;
  }

  // Prépare les tableaux pour les tentes (initialisés à 0)
  uint nb_tents_row[nb_rows];
  uint nb_tents_col[nb_cols];

  for (uint i = 0; i < nb_rows; i++) {
    nb_tents_row[i] = 0;
  }

  for (uint i = 0; i < nb_cols; i++) {
    nb_tents_col[i] = 0;
  }

  // Crée le jeu en appelant la fonction principale (sans malloc inutile avant)
  return game_new_ext(nb_rows, nb_cols, squares, nb_tents_row, nb_tents_col, wrapping);
}

/* *********************************************************** */

uint game_nb_rows(cgame g)
{
  // Vérifie que le jeu existe
  if (g == NULL) {
    fprintf(stderr, "JEU VIDE");
    exit(EXIT_FAILURE);
  }

  // Retourne la hauteur
  return g->height;
}

/* *********************************************************** */

uint game_nb_cols(cgame g)
{
  // Vérifie que le jeu existe
  if (g == NULL) {
    fprintf(stderr, "JEU VIDE");
    exit(EXIT_FAILURE);
  }

  // Retourne la largeur
  return g->width;
}

/* *********************************************************** */

bool game_is_wrapping(cgame g)
{
  // Vérifie que le jeu existe
  if (g == NULL) {
    fprintf(stderr, "JEU VIDE");
    exit(EXIT_FAILURE);
  }

  // Retourne si le mode "tore" est activé
  return g->wrapping;
}

/* *********************************************************** */

void game_undo(game g)
{
  // Vérifie erreurs mémoire
  if (g == NULL) {
    fprintf(stderr, "Erreur mémoire");
    exit(EXIT_FAILURE);
  }

  // Si rien à annuler, on ne fait rien
  if (queue_is_empty(g->undo_stack)) return;

  // Récupère le dernier coup joué
  move* m = queue_pop_tail(g->undo_stack);
  // Remet la case comme elle était avant
  g->grid[m->i][m->j] = m->before;
  // Ajoute ce coup dans la pile "refaire"
  queue_push_tail(g->redo_stack, m);
}

/* *********************************************************** */

void game_redo(game g)
{
  // Vérifie erreurs mémoire
  if (g == NULL) {
    fprintf(stderr, "Erreur mémoire");
    exit(EXIT_FAILURE);
  }

  // Si rien à refaire, on ne fait rien
  if (queue_is_empty(g->redo_stack)) return;

  // Récupère le coup annulé
  move* m = queue_pop_tail(g->redo_stack);
  // Remet la case avec la nouvelle valeur
  g->grid[m->i][m->j] = m->after;
  // Remet ce coup dans la pile "annuler"
  queue_push_tail(g->undo_stack, m);
}