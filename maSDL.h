// Partial SDL wrapper for display functions

#include "towerdefend.h"
#ifndef MASDL_H_INCLUDED
#define MASDL_H_INCLUDED

void message(char *myTitle, char *myMessage);

//void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination );
//void clear_surface(SDL_Surface *psurf);

void prepare_sprite( int x, int y, SDL_Surface* source, SDL_Surface* destination );
void efface_fenetre(SDL_Surface *psurf);
void maj_fenetre(SDL_Window *pWindow);

void prepareAllSpriteDuJeu(TplateauJeu jeu, int** chemin, int largeur, int hauteur, SDL_Surface **TabSprite, SDL_Surface* destination );
void dessineAttaque(SDL_Surface *surface, Tunite *attaquant, Tunite *cible );

// SDL specific functions

void pxl(SDL_Surface *surface, int x, int y, Uint32 color);
// Sets the pixel at (x, y) in the specified surface.

Uint32 getpxl(SDL_Surface *surface, int x, int y);
// Returns the value of a pixel at (x,y) from the specified surface.

void frame(SDL_Surface* surface, int x, int y, int w, int h, Uint32 color);
// Draws a colored filled box at (x, y) with specified width and height.

void cls(SDL_Surface* surface, Uint32 color);
// Fill the entire surface with specified color

void Hline(SDL_Surface* surface, int x, int y, int w, Uint32 color);
// Draws a colored horizontal line at (x, y) with specified width.

void Vline(SDL_Surface* surface, int x, int y, int h, Uint32 color);
// Draws a colored vertical line at (x, y) with specified height.

Uint8 Cpxl(SDL_Surface *surface, int x, int y, Uint32 color);
// Checks whether the pixel is in the surface before calling pxl function.

void box(SDL_Surface* surface, int x, int y, int w, int h, Uint32 color);
// Draws a colored square or a rectangle at (x, y) with specified width and
// height.

void line(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 color);
// Draws a colored line from A(x, y) to B(x, y).

void circle(SDL_Surface* surface, int cx, int cy, int rayon, int color);
// Draws a colored circle with center at (x, y) and with the specified radius.

void disc(SDL_Surface *surface, int cx, int cy, int rayon, int color);
// Draws a colored filled circle with center at (x, y) and with the specified
// radius.

#endif // MASDL_H_INCLUDED



/*
Usage examples:

pxl(ecran, 10, 10, color); // Pixel of specified color at (10, 10) on screen.

getpxl(ecran, 10, 10); // Returns the Uint32 corresponding to the pixel color at (10, 10).

frame(ecran, 10, 10, 20, 30, color); // Filled rectangle (width 20, height 30) at (10, 10) with specified color.

cls(ecran, 0); // Fills the screen with black (clears the screen).

Hline(ecran, 10, 10, 50, color); // Horizontal line of 50px starting at (10, 10).

Vline(ecran, 10, 10, 50, color); // Vertical line of 50px starting at (10, 10).

Cpxl(ecran, 10, 16, color); // Verifies that coordinates are within the screen before calling pxl (clipping).

box(ecran, 10, 10, 50, 20, color); // Empty rectangle of specified color at (10, 10) with dimensions 50x20.

line(ecran, 0, 0, 10, 10, 0xFF0000); // Red line between coordinates (0, 0) and (10, 10) inclusive.

circle(ecran, 10, 10, 5, color); // Empty circle at (10, 10) with radius 5.

disc(ecran, 10, 10, 5, color); // Filled circle at (10, 10) with radius 5.
*/