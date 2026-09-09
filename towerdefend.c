#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include "towerdefend.h"
#include <stdbool.h>

#include "maSDL.h"

TplateauJeu AlloueTab2D(int largeur, int hauteur){
    TplateauJeu jeu;
    jeu = (Tunite***)malloc(sizeof(Tunite**)*largeur);
    for (int i=0;i<largeur;i++){
        jeu[i] = (Tunite**)malloc(sizeof(Tunite*)*hauteur);
    }
    return jeu;  // 2D array containing pointers
}

void initPlateauAvecNULL(TplateauJeu jeu,int largeur, int hauteur){
    for (int i=0;i<largeur;i++){
        for (int j=0;j<hauteur;j++){
            jeu[i][j] = NULL;
        }
    }
}

/*
Time complexity: O(n) due to list insertion
Space complexity: O(1) as we only create one unit
This function creates the King's tower and positions it just above the last cell of the path, then adds it to the defense list.
*/
void initTourRoi(TListePlayer *playerDefense, TplateauJeu jeu, int** tabParcours)
{
    int posX = tabParcours[NBCOORDPARCOURS - 1][X];
    int posY = (tabParcours[NBCOORDPARCOURS - 1][Y] - 1); // The King will always be one cell above the end of the path
    
    Tunite *tourRoi = creeTourRoi(posX,posY);
    AjouterUnite(playerDefense, tourRoi);
    jeu[posX][posY]= tourRoi;
}

/*
Time complexity: O(n) as we potentially traverse the entire defense list to find the King
Space complexity: O(1) no memory allocation
This function traverses the defenders list to find the King's tower. If its HP drops to 0, it returns true to trigger Game Over.
*/
bool tourRoiDetruite(TListePlayer playerDefense)
{
    TListePlayer current = playerDefense;
    
    while(current != NULL)
    {
        if(current->pdata->nom == tourRoi)
        {
            if(current->pdata->pointsDeVie <= 0)
            {
                return true; 
            }
            else
            {
                return false;
            }
        }
        current = current->suiv;
    }
    
    return true; 
}

/*
Time complexity: O(n) where n is the total number of units in the lists.
Space complexity: O(1) we only free memory and reset pointers to null, no allocation.
This function cleans up everything before a load. 
It empties the game board with NULLs then frees both player lists to prevent mixing old units with new ones.
*/
void nettoyerPartie(TListePlayer *playerAttaque, TListePlayer *playerDefense, TplateauJeu jeu) 
{
    initPlateauAvecNULL(jeu, LARGEURJEU, HAUTEURJEU);
    free_ListeTemporaire(*playerAttaque);    // Originally free_ListeTemporaire was made for main_attaque
    free_ListeTemporaire(*playerDefense);    // but it perfectly matches our needs here
    *playerAttaque = NULL;
    *playerDefense = NULL;
}

/*
Time complexity: O(n) where n is the number of saved cells in the list
Space complexity: O(1) we only use a temporary pointer to clean up
This function traverses the coordinates list to empty it. It is used on position lists for optimal tower placement.
*/
void viderListeCoord(TListeCoord *liste) 
{
    while (*liste != NULL) 
    {
        TListeCoord temp = *liste;
        *liste = (*liste)->suiv;
        free(temp->pdata); 
        free(temp);
    }
}

/*
Time complexity: O(n) where n is the total number of units in the game.
Space complexity: O(1)
This function creates or overwrites a text file to save the game state. 
It starts by saving the 34 path cells, then counts and writes all vital tower data, 
then does the same for monsters, separating variables with spaces for reading.
*/
void sauvegardeSequentielle(TListePlayer playerAttaque, TListePlayer playerDefense, int** tabParcours) 
{
    FILE *f_out; 

    if ((f_out = fopen("partieseq.tds", "w")) == NULL) 
    { 
        fprintf(stderr, "\nError: Impossible to write in partieseq.tds\n"); 
        return;
    }

    // Path
    for (int i = 0; i < NBCOORDPARCOURS; i++) 
    {
        fprintf(f_out, "%d %d\n", tabParcours[i][X], tabParcours[i][Y]); 
    }

    // Defense player
    int nbDefense = 0;
    TListePlayer current = playerDefense;
    
    // Count the number of towers
    while(current != NULL) 
    { 
        nbDefense++; 
        current = current->suiv; 
    }

    fprintf(f_out, "%d\n", nbDefense); 

    current = playerDefense;
    for (int i = 0; i < nbDefense; i++) 
    { 
        Tunite *u = current->pdata;
        // We save: name, posX, posY, pointsDeVie, caseChemin, deplacementCumule
        fprintf(f_out, "%d %d %d %d %d %f\n", u->nom, u->posX, u->posY, u->pointsDeVie, u->caseChemin, u->deplacementCumule);
        current = current->suiv;
    }

    // Attack player
    int nbAttaque = 0;
    current = playerAttaque;
    
    while(current != NULL) 
    { 
        nbAttaque++; 
        current = current->suiv; 
    }

    fprintf(f_out, "%d\n", nbAttaque); 

    current = playerAttaque;
    for (int i = 0; i < nbAttaque; i++) 
    {
        Tunite *u = current->pdata;
        fprintf(f_out, "%d %d %d %d %d %f\n", u->nom, u->posX, u->posY, u->pointsDeVie, u->caseChemin, u->deplacementCumule);
        current = current->suiv;
    }

    fclose(f_out); 
    printf("Sequential save successful!\n"); 
}

/*
Time complexity: O(n^2) 
Space complexity: O(n) 
This function reads a text file to load a previously saved game state.
*/
void chargementSequentiel(TListePlayer *playerAttaque, TListePlayer *playerDefense, TplateauJeu jeu, int** tabParcours) 
{
    FILE *f_in; 

    if ((f_in = fopen("partieseq.tds", "r")) == NULL) 
    { 
        fprintf(stderr, "\nError: Impossible to read partieseq.tds\n"); 
        return;
    }

    // Path
    for (int i = 0; i < NBCOORDPARCOURS; i++) 
    {
        fscanf(f_in, "%d", &tabParcours[i][X]);
        fscanf(f_in, "%d", &tabParcours[i][Y]);
    }

    int nbUnites, nom, posX, posY, pv, caseC;
    float depCumule;

    // Defense team
    fscanf(f_in, "%d", &nbUnites); 

    for (int i = 0; i < nbUnites; i++) 
    { 
        fscanf(f_in, "%d", &nom);
        fscanf(f_in, "%d", &posX);
        fscanf(f_in, "%d", &posY);
        fscanf(f_in, "%d", &pv);
        fscanf(f_in, "%d", &caseC);
        fscanf(f_in, "%f", &depCumule);

        Tunite *nouv = NULL;

        if (nom == tourSol) 
            nouv = creeTourSol(posX, posY);
        else if (nom == tourAir) 
            nouv = creeTourAir(posX, posY);
        else if (nom == tourRoi) 
            nouv = creeTourRoi(posX, posY);

        if (nouv != NULL) {
            nouv->pointsDeVie = pv;
            nouv->caseChemin = caseC;
            nouv->deplacementCumule = depCumule;
            
            AjouterUnite(playerDefense, nouv);
            jeu[posX][posY] = nouv;
        }
    }

    // Attack team
    fscanf(f_in, "%d", &nbUnites); 

    for (int i = 0; i < nbUnites; i++) 
    {
        fscanf(f_in, "%d", &nom);
        fscanf(f_in, "%d", &posX);
        fscanf(f_in, "%d", &posY);
        fscanf(f_in, "%d", &pv);
        fscanf(f_in, "%d", &caseC);
        fscanf(f_in, "%f", &depCumule);

        Tunite *nouv = NULL;

        if (nom == archer) 
            nouv = creeArcher(posX, posY);

        else if (nom == chevalier) 
            nouv = creeChevalier(posX, posY);

        else if (nom == dragon) 
            nouv = creeDragon(posX, posY);

        else if (nom == gargouille) 
            nouv = creeGargouille(posX, posY);

        if (nouv != NULL) 
        {
            nouv->pointsDeVie = pv;
            nouv->caseChemin = caseC;
            nouv->deplacementCumule = depCumule;
            
            AjouterUnite(playerAttaque, nouv);
            jeu[posX][posY] = nouv;
        }
    }

    fclose(f_in); 
    printf("Sequential load successful!\n");
}

/*
Time complexity: O(n)
Space complexity: O(1)
Same role as sequential save but in binary format.
*/
void sauvegardeBinaire(TListePlayer playerAttaque, TListePlayer playerDefense, int** tabParcours) {
    FILE *f_out; 
    
    if ((f_out = fopen("partiebin.tdb", "wb")) == NULL) 
    { 
        fprintf(stderr, "\nError: Impossible to write in partiebin.tdb\n"); 
        return;
    }

    
    for (int i = 0; i < NBCOORDPARCOURS; i++) 
    {
        fwrite(tabParcours[i], sizeof(int), 2, f_out);
    }

    
    int nbDefense = 0;
    TListePlayer current = playerDefense;
    while(current != NULL) 
    { 
        nbDefense++; 
        current = current->suiv; 
    }

    
    fwrite(&nbDefense, sizeof(int), 1, f_out); 

    current = playerDefense;
    for (int i = 0; i < nbDefense; i++) 
    {
        fwrite(current->pdata, sizeof(Tunite), 1, f_out); 
        current = current->suiv;
    }

    int nbAttaque = 0;
    current = playerAttaque;
    while(current != NULL) 
    { 
        nbAttaque++; 
        current = current->suiv; 
    }

    fwrite(&nbAttaque, sizeof(int), 1, f_out);

    current = playerAttaque;
    for (int i = 0; i < nbAttaque; i++) 
    {
        fwrite(current->pdata, sizeof(Tunite), 1, f_out);
        current = current->suiv;
    }

    fclose(f_out); 
    printf("Binary save successful!\n");
}

/*
Time complexity: O(n^2) due to AjouterUnite
Space complexity: O(n) as we allocate (malloc) a new memory block (Tunite) for each element read from the file
Same role as sequential load but in binary format.
*/
void chargementBinaire(TListePlayer *playerAttaque, TListePlayer *playerDefense, TplateauJeu jeu, int** tabParcours) {
    FILE *f_in; 

    
    if ((f_in = fopen("partiebin.tdb", "rb")) == NULL) { 
        fprintf(stderr, "\nError: Impossible to read partiebin.tdb\n"); 
        return;
    }

    for (int i = 0; i < NBCOORDPARCOURS; i++) {
        fread(tabParcours[i], sizeof(int), 2, f_in);
    }

    int nbUnites;


    fread(&nbUnites, sizeof(int), 1, f_in); 

    for (int i = 0; i < nbUnites; i++) 
    {
        Tunite *nouv = (Tunite*)malloc(sizeof(Tunite));
        
        fread(nouv, sizeof(Tunite), 1, f_in); 
        
        AjouterUnite(playerDefense, nouv);
        jeu[nouv->posX][nouv->posY] = nouv;
    }

    fread(&nbUnites, sizeof(int), 1, f_in);

    for (int i = 0; i < nbUnites; i++) {
        Tunite *nouv = (Tunite*)malloc(sizeof(Tunite));
        
        fread(nouv, sizeof(Tunite), 1, f_in);
        
        AjouterUnite(playerAttaque, nouv);
        jeu[nouv->posX][nouv->posY] = nouv;
    }

    fclose(f_in); 
    printf("Binary load successful!\n");
}


/*
Time complexity: O(n) where n is the number of path cells (NBCOORDPARCOURS)
Space complexity: O(n) as we allocate (with malloc) a 2D array to store the 34 coordinate pairs
This function randomly generates the complete layout of the path. 
It starts at the bottom middle, then alternates between upward phases (vertical) and lateral movement phases. 
It returns the 2D array containing all the path coordinates.
*/
int **initRandomChemin()
{
    int **chemin = (int**)malloc(sizeof(int*)*NBCOORDPARCOURS);

    for (int j=0;j<NBCOORDPARCOURS;j++){
        chemin[j] = (int*)malloc(sizeof(int)*2);  // 2 cells: index 0 for X coord, index 1 for Y coord
    }

    // Initialization of the starting point, it will always be the same.
    int x = LARGEURJEU / 2;
    int y = HAUTEURJEU - 1;
    int index = 0;

    // Adding the starting cell
    chemin[index][X] = x;
    chemin[index][Y] = y;
    index = index + 1;

    // For random generation, we alternate upward paths and lateral path creation phases
    // For lateral phases, we randomly choose left or right
    int direction = 0;
    int distance;
    int directionHorizontal;

    while(index < (NBCOORDPARCOURS - 1))
    {
        // Vertical phase
        if(direction == 0)
        {
            distance = (rand() % 3) + 2;   // Arbitrarily choose a distance between 2 and 5 cells

            int z = 0;
            while( z < distance && index < (NBCOORDPARCOURS - 1) && y > 3)
            {
                y--;

                chemin[index][X] = x;
                chemin[index][Y] = y;
                index++;

                z++;
            }

            direction = 1;
        }

        // Horizontal phase
        else if(direction == 1)
        {
            directionHorizontal = rand() % 2; // 0 = left, 1 = right
            distance = (rand() % 5) + 5;   // Arbitrarily choose a distance between 5 and 10 cells
            
            if(directionHorizontal == 0 && x == 1)
                directionHorizontal = 1;

            if(directionHorizontal == 1 && x == LARGEURJEU -  2)
                directionHorizontal = 0;

            // Left
            if(directionHorizontal == 0)
            {
                int i = 0;
                while(i < distance && index < (NBCOORDPARCOURS - 1) && x > 1)
                    {
                        x--;

                        chemin[index][X] = x;
                        chemin[index][Y] = y;
                        index++;

                        i++;
                    }
            }

            // Right
            else if(directionHorizontal == 1)
            {
                int j = 0;
                while(j < distance && index < (NBCOORDPARCOURS - 1) && x < LARGEURJEU -  2)
                    {
                        x++;

                        chemin[index][X] = x;
                        chemin[index][Y] = y;
                        index++;

                        j++;
                    }
            }

            direction = 0;
        }
    }

    y--;
    chemin[index][X] = x;
    chemin[index][Y] = y;

    return chemin;
}


/*
Time complexity: O(n^2) due to the final sort (trisListePositionTour) and duplicate verification.
Space complexity: O(n) as we allocate a new memory node (with malloc) for each valid cell found around the path.
This function traverses the 34 path cells. For each path cell, it searches all surrounding cells within range to place an air tower. 
It adds them to a list, then sorts this list to put the cells with the highest score at the very beginning.
*/
TListeCoord initTabPositionToursAir(TplateauJeu jeu, int** tabParcours)
{
    TListeCoord listeCoordTourAir = NULL;
    int index = 0;
    int x = 0;
    int y = 0;

    while(index < NBCOORDPARCOURS)
    {
        x = tabParcours[index][X];
        y = tabParcours[index][Y];

        AjouterCoordCase(x, y, &listeCoordTourAir, 0, tabParcours, jeu);

        index++;
    }

    trisListePositionTour(listeCoordTourAir);
    return listeCoordTourAir;
}

/*
Time complexity: O(n^2) 
Space complexity: O(n)
Same functionality as for air towers, but adapted for ground towers.
*/
TListeCoord initTabPositionToursSol(TplateauJeu jeu, int** tabParcours)
{
    TListeCoord listeCoordTourSol = NULL;
    int index = 0;
    int x = 0;
    int y = 0;

    while(index < NBCOORDPARCOURS)
    {
        x = tabParcours[index][X];
        y = tabParcours[index][Y];

        AjouterCoordCase(x, y, &listeCoordTourSol, 1, tabParcours, jeu);

        index++;
    }
    trisListePositionTour(listeCoordTourSol);
    return listeCoordTourSol;
}

/*
Time complexity: O(n) 
Space complexity: O(1) 
Helper function used to retrieve all surrounding cells within the tower's placement range.
*/
void AjouterCoordCase(int caseX, int caseY, TListeCoord *listeCoord, int typeTour, int** tabParcours, TplateauJeu jeu)  // typeTour defines the type (Air = 0 or Sol = 1) of the tower
{
    int portee;
    if(typeTour == 0)
        portee = 3;

    if(typeTour == 1)
        portee = 5;

    for(int i = (caseX - portee); i <= (caseX + portee); i++)  
    {
        for(int j = (caseY - portee); j <= (caseY + portee); j++)
        {
            if(i >= 0 && i < LARGEURJEU && j >= 0 && j < HAUTEURJEU)
            {
                if(jeu[i][j] == NULL)
                {
                    int distance = abs(caseX - i) + abs(caseY - j);
                    
                    if(distance <= portee)
                    {
                        if( !(CoordInList(i, j, *listeCoord)) && !(CoordInParcour(i, j, tabParcours)))
                        {
                            AjouterCoord(listeCoord, i, j, tabParcours, portee); 
                        }
                    }
                }
            }
        }
    }
}

/*
Time complexity: O(n)
Space complexity: O(1) 
Used in the AjouterCoordCase function, simply verifies that the cell is empty.
*/
bool CoordInParcour(int x, int y, int** tabParcours)
{
    int index = 0;
    int tabX;
    int tabY;

    while(index < NBCOORDPARCOURS)
    {
        tabX = tabParcours[index][X];
        tabY = tabParcours[index][Y];

        if(tabX == x && tabY == y)
            return true;

        index++;
    }

    return false;
}

/*
Time complexity: O(n) 
Space complexity: O(1) 
Only verifies that the cell is not already present in the list.
*/
bool CoordInList(int x, int y, TListeCoord listeCoord)
{
    TListeCoord liste = listeCoord;

    while(liste != NULL)
    {
        if(liste->pdata->x == x && liste->pdata->y == y)
        {
            return true; 
        }
        liste = liste->suiv;
    }

    return false;
}

/*
Time complexity: O(n)
Space complexity: O(1)
Function that adds a coordinate into the tower placement linked list, calling a score calculation function.
*/
void AjouterCoord(TListeCoord *liste, int x, int y, int** tabParcours, int portee)
{
    Tcoord *nouvData = (Tcoord*)malloc(sizeof(Tcoord));
    nouvData->x = x;
    nouvData->y = y;
    nouvData->score = calculScoreCase(x, y, tabParcours, portee);
    
    TListeCoord nouvNode = (TListeCoord)malloc(sizeof(struct T_cell_coord));
    nouvNode->pdata = nouvData; 

    nouvNode->suiv = *liste;
    *liste = nouvNode;
}

/*
Time complexity: O(n)
Space complexity: O(1)
Simple score calculation by counting the number of path cells within range of the tested cell.
*/
int calculScoreCase(int x, int y, int** tabParcours, int portee)
{
    int index = 0;
    int score = 0;
    int tabX;
    int tabY;

    while(index < NBCOORDPARCOURS)
    {
        tabX = tabParcours[index][X];
        tabY = tabParcours[index][Y];

        int distance = abs(x - tabX) + abs(y - tabY);

        if(distance <= portee)
        {
            score++;
        }

        index++;
    }

    return score;
}

/*
Time complexity: O(n^2)
Space complexity: O(1)
Simple function that sorts the tower placement list from highest to lowest score, using selection sort.
*/
void trisListePositionTour(TListeCoord liste)
{
    Tcoord *temp;
    TListeCoord en_cours = liste;
    TListeCoord j;
    TListeCoord plus_grand;
    while(en_cours != NULL)
    {
        plus_grand = en_cours;
        j = en_cours->suiv;
        while(j != NULL)
        {
            if ((j->pdata->score) > (plus_grand->pdata->score))
                plus_grand = j;
            j = j->suiv;
        }

        temp = en_cours->pdata;
        en_cours->pdata = plus_grand->pdata;
        plus_grand->pdata = temp;
        en_cours = en_cours->suiv;
    }
}


/*
Function that manages the entire combat phase, starting with the attacking team, then defense.
*/
void main_attaque(TplateauJeu jeu, TListePlayer *playerAttaque, TListePlayer *playerDefense, SDL_Surface* surface, SDL_Window *window)
{
    Tunite *res;
    TListePlayer liste;
    TListePlayer attaque = *playerAttaque;
    while(attaque != NULL)
    {
        liste = quiEstAPortee(jeu, attaque->pdata);
        res = get_first_unit(liste);
        free_ListeTemporaire(liste);
        if(res != NULL)
        {
            combat(surface, window,  (attaque->pdata), res);
        }
        attaque = attaque->suiv;
    }

    TListePlayer defense = *playerDefense;

    while(defense != NULL)
    {
        liste = quiEstAPortee(jeu, defense->pdata);
        res = get_first_unit(liste);
        free_ListeTemporaire(liste);
        if(res != NULL)
        {
            combat(surface, window, (defense->pdata), res); 
            if(res->pointsDeVie <= 0)
            {
                supprimerUnite(playerAttaque, res, jeu);

            }
        }
        defense = defense->suiv;
    }
}

/*
Main function for the Movement phase, mainly acts as a caller to other functions.
*/
void main_deplacement(TListePlayer playerAttaque, TListePlayer playerDefense, TplateauJeu jeu, int** tabParcours)
{
    TListePlayer attaque = playerAttaque;
    TListePlayer defense = playerDefense;

    while(attaque != NULL)
    {
        deplacement(attaque->pdata, jeu, tabParcours);
        attaque = attaque->suiv;
    }

}

/*
Movement function. Globally, the new cell points towards the unit and the old one is set to NULL.
An interesting point: I added a cumulative movement field in Tunite to manage the number of cells 
advanced each turn based on the movement speed.
*/
void deplacement(Tunite *unit, TplateauJeu jeu, int** tabParcours)
{
    unit->deplacementCumule += unit->vitessedeplacement;
    int casesAjouuter = (int)(unit->deplacementCumule);
    
    if(casesAjouuter > 0)
    {
        int nouveauIndiceChemin = unit->caseChemin + casesAjouuter;

        if(nouveauIndiceChemin >= NBCOORDPARCOURS)
        {
            nouveauIndiceChemin = NBCOORDPARCOURS - 1; // Stop at the last cell
        }

        int newX = tabParcours[nouveauIndiceChemin][X];
        int newY = tabParcours[nouveauIndiceChemin][Y];

        while(nouveauIndiceChemin > unit->caseChemin && jeu[newX][newY] != NULL)
        {
            nouveauIndiceChemin = nouveauIndiceChemin - 1;
            newX = tabParcours[nouveauIndiceChemin][X];
            newY = tabParcours[nouveauIndiceChemin][Y];
        }

        if(jeu[newX][newY] == NULL)
        {
            jeu[newX][newY] = unit;
            jeu[unit->posX][unit->posY] = NULL;

            unit->deplacementCumule = unit->deplacementCumule - (nouveauIndiceChemin - unit->caseChemin);
            unit->caseChemin = unit->caseChemin + (nouveauIndiceChemin - unit->caseChemin);
            unit->posX = newX;
            unit->posY = newY;
        }
    }
}

/*
Main function for the creation phase. It proceeds with a random draw out of 100 to simulate percentages, 
determining whether a new unit is created. Done once for the defense camp and once for the attack.
*/
void main_Creation(TListePlayer *playerAttaque, TListePlayer *playerDefense, TplateauJeu jeu, int** tabParcours, TListeCoord *tabTourAir, TListeCoord *tabTourSol)
{
    int tirageDefense = (rand() % 100) + 1;
    int tirageAttaque = (rand() % 100) + 1;
    int departX = tabParcours[0][X];
    int departY = tabParcours[0][Y];

    if(tirageAttaque <= PROBAATTAQUE)
    {
        int tirageUnite = (rand() % 4); // Randomly choose the unit, spawn rates for different units can be added later
        
        if(tirageUnite == 0)
        {
            Tunite *archer = creeArcher(departX, departY);
            AjouterUnite(playerAttaque, archer);
            jeu[departX][departY] = archer;
        }

        if(tirageUnite == 1)
        {
            Tunite *gargouille = creeGargouille(departX, departY);
            AjouterUnite(playerAttaque, gargouille);
            jeu[departX][departY] = gargouille;
        }

        if(tirageUnite == 2)
        {
            Tunite *dragon = creeDragon(departX, departY);
            AjouterUnite(playerAttaque, dragon);
            jeu[departX][departY] = dragon;
        }

        if(tirageUnite == 3)
        {
            Tunite *chevalier = creeChevalier(departX, departY);
            AjouterUnite(playerAttaque, chevalier);
            jeu[departX][departY] = chevalier;
        }
    }

    if(tirageDefense <= PROBADEFENSE && jeu[(departX - 1)][departY] == NULL)
    {
        int tirageUnite = (rand() % 2);


        if(tirageUnite == 0)
        {
            while(ListeParcourueOUCaseNonVide(tabTourSol, jeu))
            {
                TListeCoord temp = *tabTourSol;
                (*tabTourSol) = (*tabTourSol)->suiv;

                free(temp->pdata);
                free(temp);
            }
            if(*tabTourSol != NULL)
            {
                Tunite *tour = creeTourSol((*tabTourSol)->pdata->x, (*tabTourSol)->pdata->y);
                AjouterUnite(playerDefense, tour);
                jeu[(*tabTourSol)->pdata->x][(*tabTourSol)->pdata->y] = tour;
            }
        }

        if(tirageUnite == 1)
        {
            while(ListeParcourueOUCaseNonVide(tabTourAir, jeu))
            {
                TListeCoord temp = *tabTourAir;
                (*tabTourAir) = (*tabTourAir)->suiv;

                free(temp->pdata);
                free(temp);
            }
            if(*tabTourAir != NULL)
            {
                Tunite *tour = creeTourAir((*tabTourAir)->pdata->x, (*tabTourAir)->pdata->y);
                AjouterUnite(playerDefense, tour);
                jeu[(*tabTourAir)->pdata->x][(*tabTourAir)->pdata->y] = tour;
            }
        }
    }
}

/*
This function checks if the list has been fully traversed or not, and if the cell already contains something.
*/
bool ListeParcourueOUCaseNonVide(TListeCoord *tabTour, TplateauJeu jeu)
{
    if(*tabTour == NULL)
        return false;

    else if(jeu[(*tabTour)->pdata->x][(*tabTour)->pdata->y] != NULL)
        return true;
    
    return false;
}

/*
Function returning all targetable cells within the parameter unit's range.
*/
TListePlayer quiEstAPortee(TplateauJeu jeu, Tunite *UniteAttaquante)
{
    int range = UniteAttaquante->portee;
    int unitX = UniteAttaquante->posX;
    int unitY = UniteAttaquante->posY;
    int tmp = 0;

    TListePlayer res = NULL;

    for (int i = unitX-range; i <= unitX+range; i++)
    {
        for (int j = unitY-range; j <= unitY+range; j++)
        {
            if(i >= 0 && i < LARGEURJEU && j >= 0 && j < HAUTEURJEU)
            {
                tmp = (abs(unitX - i) + abs(unitY - j));
                if (tmp > 0 && tmp <= range)
                {
                    if(jeu[i][j] != NULL && jeu[i][j]->nom != tourAir && jeu[i][j]->nom != tourSol)
                    {
                        if(((jeu[i][j])->equipe != UniteAttaquante->equipe) )
                        {
                            if(estAttaquable(UniteAttaquante, (jeu[i][j])))
                            {
                               AjouterUnite(&res, jeu[i][j]);
                            }
                        }
                    }
                }
            }
        }
    }

    tris_liste(res);

    return res;
}

/*
Function used at the beginning of each turn to reset the attack counter to 0.
*/
void reset_attaque(TListePlayer playerAttaque, TListePlayer playerDefense)
{
    TListePlayer attaque = playerAttaque;
    TListePlayer defense = playerDefense;

    while(attaque != NULL)
    {
        attaque->pdata->peutAttaquer = 1;

        attaque = attaque->suiv;
    }
    while(defense != NULL)
    {
        defense->pdata->peutAttaquer = 1;

        defense = defense->suiv;
    }
}

/*
Function that sorts a TListePlayer list to return the unit with the lowest health points.
*/
void tris_liste(TListePlayer liste)
{
    Tunite *temp;
    TListePlayer en_cours = liste;
    TListePlayer j;
    TListePlayer plus_petit;
    while(en_cours != NULL)
    {
        plus_petit = en_cours;
        j = en_cours->suiv;
        while(j != NULL)
        {
            if (j->pdata->pointsDeVie < plus_petit->pdata->pointsDeVie)
                plus_petit = j;
            j = j->suiv;
        }

        temp = en_cours->pdata;
        en_cours->pdata = plus_petit->pdata;
        plus_petit->pdata = temp;
        en_cours = en_cours->suiv;
    }
}

/*
Simple function returning a boolean based on whether the target is attackable by the unit.
*/
bool estAttaquable (Tunite *UniteAttaquante, Tunite *cible)
{
    if(UniteAttaquante->cibleAttaquable == solEtAir)
        return true;
    else
    {
        if(UniteAttaquante->cibleAttaquable == cible->maposition)
        {
            return true;
        }
    return false;
    }
}

/*
Simple function returning the first element of a TListePlayer list.
*/
Tunite *get_first_unit(TListePlayer liste)
{
    TListePlayer current = liste;

    while(current != NULL)
    {
        if(current->pdata != NULL && current->pdata->pointsDeVie > 0)
            return current->pdata;

        current = current->suiv;
    }
    

    return NULL;
}

/* Function managing the combat part between the attacker and the target */
void combat(SDL_Surface *surface , SDL_Window *window, Tunite *UniteAttaquante, Tunite *UniteCible)
{
    if(UniteAttaquante->peutAttaquer == 1)
    {
        dessineAttaque(surface, UniteAttaquante, UniteCible);
        maj_fenetre(window);
        UniteCible->pointsDeVie = (UniteCible->pointsDeVie - UniteAttaquante->degats);
        UniteAttaquante->peutAttaquer = 0;
    }
}

/*
Function simply useful for understanding the game and debugging, displays the path cells.
*/
void afficheCoordonneesParcours(int **chemin, int nbcoord){
    printf("Coordinates list: ");
    for (int i=0; i<nbcoord; i++){
        printf("(%d, %d)",chemin[i][X], chemin[i][Y]);
    }
    printf("\nEnd of coordinates list\n");
}

/*
Function that frees the entire path array.
*/
void freeChemin(int **tab){
    for (int j=0;j<NBCOORDPARCOURS;j++){
        free(tab[j]);  // Frees each cell, which is an array of 2 cells
    }
    free(tab);
}

/* Provided function, useful for debugging */
void affichePlateauConsole(TplateauJeu jeu, int largeur, int hauteur){
    // For console display, related to enum TuniteDuJeu
    const char* InitialeUnite[7]={"s", "a", "r", "A", "C", "D", "G"};

    printf("\n");
    for (int j=0;j<hauteur;j++){
        for (int i=0;i<largeur;i++){
            if (jeu[i][j] != NULL){
                    printf("%s",InitialeUnite[jeu[i][j]->nom]);
            }
            else printf(" ");  // i.e., no unit on this cell
        }
        printf("\n");
    }
}


/*
Adds a Tunite into a TListePlayer
*/
void AjouterUnite(TListePlayer *player, Tunite *nouvelleUnite)
{
    TListePlayer newU = (TListePlayer)malloc(sizeof(struct T_cell));
    newU->suiv = NULL;
    newU->pdata = nouvelleUnite;

    if(*player == NULL)
    {
        *player = newU;
    }
    else
    {
        TListePlayer current = *player;

        while(current->suiv != NULL)
        {
            current = current->suiv;
        }
        current->suiv = newU;
    }

}

/*
Empties a TListePlayer list
*/
void free_ListeTemporaire(TListePlayer liste)
{
    TListePlayer temp;
    while(liste != NULL)
    {
        temp = liste;
        liste = liste->suiv;
        free(temp);
    }
}

/*
Deletes a unit from its TListePlayer list and from the Board
*/
void supprimerUnite(TListePlayer *player, Tunite *UniteDetruite, TplateauJeu jeu)
{
    TListePlayer current = *player;
    TListePlayer prev = NULL;

    while (current != NULL && current->pdata != UniteDetruite)
    {
       prev = current;
       current = current->suiv;
    }
    if (current == NULL)
    {
        printf("Unit not found \n");
    }
    else
    {
        jeu[(*(current->pdata)).posX][(*(current->pdata)).posY] = NULL;
        if(current->suiv == NULL)
        {
            free(current->pdata);
            if (prev == NULL)
            {
                *player = NULL;
                free(current);
            }
            else
            {
                prev->suiv = NULL;
                free(current);
            }
        }
        else
        {
            if(prev == NULL)
                *player = current->suiv;
            else
                prev->suiv = current->suiv;
            free(current->pdata);
            free(current);
        }
    }
}

/*
Function for debugging, displays a TListePlayer
*/
void printListePlayer(TListePlayer player)
{
    TListePlayer current = player;
    if(current == NULL)
    {
        printf("Player list is empty\n");
    }
    else
    {
        int cmp = 0;
        while(current != NULL)
        {
            printf("Unit : %d pts = %d \n", cmp, current->pdata->pointsDeVie);
            current = current->suiv;
            cmp++;
        }
    }
}

/* Functions required for unit creation */

Tunite *creeTourSol(int posx, int posy){
    Tunite *nouv = (Tunite*)malloc(sizeof(Tunite));
    nouv->nom = tourSol;
    nouv->equipe = 0;
    nouv->cibleAttaquable = sol;
    nouv->maposition = sol;
    nouv->pointsDeVie = 500;
    nouv->vitesseAttaque = 1.5;
    nouv->degats = 120;
    nouv->portee = PORTEETOURSOL;
    nouv->vitessedeplacement = 0;
    nouv->posX = posx;
    nouv->posY = posy;
    nouv->peutAttaquer = 1;
    nouv->caseChemin = 0;
    nouv->deplacementCumule = 0;
    return nouv;
}
Tunite *creeTourAir(int posx, int posy){
    Tunite *nouv = (Tunite*)malloc(sizeof(Tunite));
    nouv->nom = tourAir;
    nouv->equipe = 0;
    nouv->cibleAttaquable = air;
    nouv->maposition = sol;
    nouv->pointsDeVie = 500;
    nouv->vitesseAttaque = 1.0;
    nouv->degats = 100;
    nouv->portee = PORTEETOURAIR;
    nouv->vitessedeplacement = 0;
    nouv->posX = posx;
    nouv->posY = posy;
    nouv->peutAttaquer = 1;
    nouv->caseChemin = 0;
    nouv->deplacementCumule = 0;
    return nouv;
}
Tunite *creeTourRoi(int posx, int posy){
    Tunite *nouv = (Tunite*)malloc(sizeof(Tunite));
    nouv->nom = tourRoi;
    nouv->equipe = 0;
    nouv->cibleAttaquable = solEtAir;
    nouv->maposition = sol;
    nouv->pointsDeVie = 800;
    nouv->vitesseAttaque = 1.2;
    nouv->degats = 180;
    nouv->portee = 4;
    nouv->vitessedeplacement = 0;
    nouv->posX = posx;
    nouv->posY = posy;
    nouv->peutAttaquer = 1;
    nouv->caseChemin = 0;
    nouv->deplacementCumule = 0;
    return nouv;
}
Tunite *creeDragon(int posx, int posy){
    Tunite *nouv = (Tunite*)malloc(sizeof(Tunite));
    nouv->nom = dragon;
    nouv->equipe = 1;
    nouv->cibleAttaquable = solEtAir;
    nouv->maposition = air;
    nouv->pointsDeVie = 200;
    nouv->vitesseAttaque = 1.1;
    nouv->degats = 180;
    nouv->portee = 2;
    nouv->vitessedeplacement = 2;
    nouv->posX = posx;
    nouv->posY = posy;
    nouv->peutAttaquer = 1;
    nouv->caseChemin = 0;
    nouv->deplacementCumule = 0;
    return nouv;
}

Tunite *creeArcher(int posx, int posy){
    Tunite *nouv = (Tunite*)malloc(sizeof(Tunite));
    nouv->nom = archer;
    nouv->equipe = 1;
    nouv->cibleAttaquable = solEtAir;
    nouv->maposition = sol;
    nouv->pointsDeVie = 80;
    nouv->vitesseAttaque = 0.7;
    nouv->degats = 120;
    nouv->portee = 3;
    nouv->vitessedeplacement = 1;
    nouv->posX = posx;
    nouv->posY = posy;
    nouv->peutAttaquer = 1;
    nouv->caseChemin = 0;
    nouv->deplacementCumule = 0;
    return nouv;
}

Tunite *creeChevalier(int posx, int posy){
    Tunite *nouv = (Tunite*)malloc(sizeof(Tunite));
    nouv->nom = chevalier;
    nouv->equipe = 1;
    nouv->cibleAttaquable = sol;
    nouv->maposition = sol;
    nouv->pointsDeVie = 400;
    nouv->vitesseAttaque = 1.2;
    nouv->degats = 250;
    nouv->portee = 1;
    nouv->vitessedeplacement = 2;
    nouv->posX = posx;
    nouv->posY = posy;
    nouv->peutAttaquer = 1;
    nouv->caseChemin = 0;
    nouv->deplacementCumule = 0;
    return nouv;
}

Tunite *creeGargouille(int posx, int posy){
    Tunite *nouv = (Tunite*)malloc(sizeof(Tunite));
    nouv->nom = gargouille;
    nouv->equipe = 1;
    nouv->cibleAttaquable = solEtAir;
    nouv->maposition = air;
    nouv->pointsDeVie = 80;
    nouv->vitesseAttaque = 0.6;
    nouv->degats = 90;
    nouv->portee = 1;
    nouv->vitessedeplacement = 3;
    nouv->posX = posx;
    nouv->posY = posy;
    nouv->peutAttaquer = 1;
    nouv->caseChemin = 0;
    nouv->deplacementCumule = 0;
    return nouv;
}