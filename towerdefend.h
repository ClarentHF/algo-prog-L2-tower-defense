#ifndef TOWERDEFEND_H_INCLUDED
#define TOWERDEFEND_H_INCLUDED

#define LARGEURJEU 11
#define HAUTEURJEU 19
#define NBCOORDPARCOURS 34
#define X 0
#define Y 1
#define PROBADEFENSE 8
#define PROBAATTAQUE 50
#define PORTEETOURAIR 3
#define PORTEETOURSOL 5

#include <stdbool.h>

typedef enum {tourSol, tourAir, tourRoi, archer, chevalier, dragon, gargouille} TuniteDuJeu;
typedef enum {sol, solEtAir, air} Tcible;


typedef struct {
    TuniteDuJeu nom;
    int equipe;             // 0 corresponds to defense towers, 1 to attacking units
    Tcible cibleAttaquable; // Indicates the type of units this entity can attack
    Tcible maposition;      // Indicates 'air' or 'sol' (ground), useful to know who can attack us
    int pointsDeVie;
    float vitesseAttaque;   // In seconds, smaller value means faster attack speed
    int degats;
    int portee;             // Attack range in grid cells/meters

    int caseChemin;         // Path index reference for movement tracking
    float vitessedeplacement; // Movement speed in m/s
    float deplacementCumule; 
    int posX, posY;         // X, Y coordinates on the game board
    int peutAttaquer;       // 0 = has already attacked, 1 = can attack this turn (reset to 1 at each turn start)
} Tunite;

typedef struct T_cell {
    struct T_cell *suiv;
    Tunite *pdata;          // Pointer to a game unit
} *TListePlayer;


typedef struct {
    int x;
    int y;
    int score;
} Tcoord;

typedef struct T_cell_coord {
    struct T_cell_coord *suiv;
    Tcoord *pdata;
} *TListeCoord;


typedef Tunite* ** TplateauJeu;  // 2D Array of width LARGEURJEU and height HAUTEURJEU containing pointers (Tunite*)


TplateauJeu AlloueTab2D(int largeur, int hauteur);
void afficheCoordonneesParcours(int **t, int nbcoord);
int **initChemin();         
void freeChemin(int **tab);

void initPlateauAvecNULL(TplateauJeu jeu,int largeur, int hauteur);
void affichePlateauConsole(TplateauJeu jeu, int largeur, int hauteur);

Tunite *creeTourSol(int posx, int posy);
Tunite *creeTourAir(int posx, int posy);
Tunite *creeTourRoi(int posx, int posy);
Tunite *creeArcher(int posx, int posy);
Tunite *creeGargouille(int posx, int posy);
Tunite *creeDragon(int posx, int posy);
Tunite *creeChevalier(int posx, int posy);

bool estAttaquable (Tunite *UniteAttaquante, Tunite *cible);
TListePlayer quiEstAPortee(TplateauJeu jeu, Tunite *UniteAttaquante);

void supprimerUnite(TListePlayer *player, Tunite *UniteDetruite, TplateauJeu jeu);

void AjouterUnite(TListePlayer *player, Tunite *nouvelleUnite);
void printListePlayer(TListePlayer player);
void tris_liste(TListePlayer liste);
Tunite *get_first_unit(TListePlayer liste);

void combat(SDL_Surface *surface , SDL_Window *window, Tunite *UniteAttaquante, Tunite *UniteCible);
void main_attaque(TplateauJeu jeu, TListePlayer *playerAttaque, TListePlayer *playerDefense, SDL_Surface* surface, SDL_Window *window);
void reset_attaque(TListePlayer playerAttaque, TListePlayer playerDefense);
void free_ListeTemporaire(TListePlayer liste);

void deplacement(Tunite *unit, TplateauJeu jeu, int** tabParcours);
void main_deplacement(TListePlayer playerAttaque, TListePlayer playerDefense, TplateauJeu jeu, int** tabParcours);
void main_Creation(TListePlayer *playerAttaque, TListePlayer *playerDefense, TplateauJeu jeu, int** tabParcours, TListeCoord *tabTourAir, TListeCoord *tabTourSol);

void initTourRoi(TListePlayer *playerDefense, TplateauJeu jeu, int** tabParcours);
bool tourRoiDetruite(TListePlayer playerDefense);
int **initRandomChemin();
void AjouterCoord(TListeCoord *liste, int x, int y, int** tabParcours, int portee);
TListeCoord initTabPositionTours(TplateauJeu jeu, int** tabParcours);
bool CoordInList(int x, int y, TListeCoord listeCoord);
void AjouterCoordCase(int caseX, int caseY, TListeCoord *listeCoord, int typeTour, int** tabParcours, TplateauJeu jeu);
bool CoordInParcour(int x, int y, int** tabParcours);
TListeCoord initTabPositionToursAir(TplateauJeu jeu, int** tabParcours);
TListeCoord initTabPositionToursSol(TplateauJeu jeu, int** tabParcours);

int calculScoreCase(int x, int y, int** tabParcours, int portee);
void trisListePositionTour(TListeCoord liste);
bool ListeParcourueOUCaseNonVide(TListeCoord *tabTour, TplateauJeu jeu);

void chargementSequentiel(TListePlayer *playerAttaque, TListePlayer *playerDefense, TplateauJeu jeu, int** tabParcours);
void sauvegardeSequentielle(TListePlayer playerAttaque, TListePlayer playerDefense, int** tabParcours);
void nettoyerPartie(TListePlayer *playerAttaque, TListePlayer *playerDefense, TplateauJeu jeu);
void viderListeCoord(TListeCoord *liste);
void sauvegardeBinaire(TListePlayer playerAttaque, TListePlayer playerDefense, int** tabParcours);
void chargementBinaire(TListePlayer *playerAttaque, TListePlayer *playerDefense, TplateauJeu jeu, int** tabParcours);

#endif // TOWERDEFEND_H_INCLUDED