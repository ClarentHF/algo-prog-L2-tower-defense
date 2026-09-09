//NOM pr�nom �tudiant 1 : HEMERY FAY CLARENT
//NOM pr�nom �tudiant 2 : HUCHET Florant
//Squellette du Code Fournie dans le cadre du projet par Yannick Degardin

#include "SDL.h"
#include "maSDL.h"    //biblioth�que avec des fonctions d'affichage utilisant la SDL
#include "towerdefend.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NB_SPRITES 11


/*--------- Main ---------------------*/
int main(int argc, char* argv[])
{
    SDL_Window *pWindow;
    SDL_Init(SDL_INIT_VIDEO);

    pWindow = SDL_CreateWindow(
        "Press ESC to quit, S/C and D/V to manage saves",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        LARGEURJEU*40,
        HAUTEURJEU*40,
        SDL_WINDOW_SHOWN
    );

    SDL_Surface* pWinSurf = SDL_GetWindowSurface(pWindow);  //le sprite qui couvre tout l'�cran
    SDL_Surface* pSpriteTourSol = SDL_LoadBMP("./data/TourSol.bmp");  //indice 0 dans tabSprite (via l'enum TuniteDuJeu)
    SDL_Surface* pSpriteTourAir = SDL_LoadBMP("./data/TourAir.bmp");  //indice 1 dans tabSprite (via l'enum TuniteDuJeu)
    SDL_Surface* pSpriteTourRoi = SDL_LoadBMP("./data/TourRoi.bmp"); //indice 2
    SDL_Surface* pSpriteArcher = SDL_LoadBMP("./data/Archer.bmp"); //indice 3
    SDL_Surface* pSpriteChevalier = SDL_LoadBMP("./data/Chevalier.bmp"); //indice 4
    SDL_Surface* pSpriteDragon = SDL_LoadBMP("./data/Dragon.bmp"); //indice 5
    SDL_Surface* pSpriteGargouille = SDL_LoadBMP("./data/Gargouille.bmp"); //indice 6
    SDL_Surface* pSpriteEau = SDL_LoadBMP("./data/Eau.bmp"); //indice 7  Ne figure pas dans l'enum TuniteDuJeu
    SDL_Surface* pSpriteHerbe = SDL_LoadBMP("./data/Herbe.bmp"); //indice 8 idem
    SDL_Surface* pSpritePont = SDL_LoadBMP("./data/Pont.bmp"); //indice 9 idem
    SDL_Surface* pSpriteTerre = SDL_LoadBMP("./data/Terre.bmp"); //indice 10 idem

    SDL_Surface* TabSprite[NB_SPRITES]={pSpriteTourSol,pSpriteTourAir,pSpriteTourRoi,pSpriteArcher,pSpriteChevalier,pSpriteDragon,pSpriteGargouille,pSpriteEau,pSpriteHerbe,pSpritePont,pSpriteTerre};
    srand(time(NULL));
    int** tabParcours= initRandomChemin();  //tabParcours est un tableau de NBCOORDPARCOURS cases, chacune contenant un tableau � 2 cases (indice 0 pour X, indice 1 pour Y)

    // Check if all sprites loaded successfully
    bool spritesLoaded = true;
    for(int i = 0; i < NB_SPRITES; i++) {
        if(TabSprite[i] == NULL) {
            spritesLoaded = false;
            break;
        }
    }

    if ( spritesLoaded )
    {
        TplateauJeu jeu = AlloueTab2D(LARGEURJEU,HAUTEURJEU);
        initPlateauAvecNULL(jeu,LARGEURJEU,HAUTEURJEU);
        affichePlateauConsole(jeu,LARGEURJEU,HAUTEURJEU);



        prepareAllSpriteDuJeu(jeu,tabParcours,LARGEURJEU,HAUTEURJEU,TabSprite,pWinSurf);
        maj_fenetre(pWindow);


        /**********************************************************************/
        /*                                                                    */
        /*              DEFINISSEZ/INITIALISER ICI VOS VARIABLES              */
        /*
                                                              */
        TListePlayer playerAttaque = NULL;
        TListePlayer playerDefense = NULL;
        initTourRoi(&playerDefense, jeu, tabParcours);
        TListeCoord tabTourAir = initTabPositionToursAir(jeu, tabParcours);
        TListeCoord tabTourSol = initTabPositionToursSol(jeu, tabParcours);
        
        /**********************************************************************/

        // boucle principale du jeu
        int cont = 1;
        while ( cont != 0 ){   //LA FIN DU JEU -> tourRoiDetruite
                SDL_PumpEvents(); //do events

                /***********************************************************************/
                /*                                                                     */
                /*                                                                     */
                //APPELEZ ICI VOS FONCTIONS QUI FONT EVOLUER LE JEU
    
                reset_attaque(playerAttaque, playerDefense);
                main_deplacement(playerAttaque, playerDefense, jeu, tabParcours);
                main_Creation(&playerAttaque, &playerDefense, jeu, tabParcours, &tabTourAir, &tabTourSol);
                
                if (tourRoiDetruite(playerDefense) == true)
                {
                        printf("\n====================================\n");
                        printf("   GAME OVER : The King's Tower has fallen !  \n");
                        printf("====================================\n");
                        
                        message("GAME OVER", "The King's Tower has been destroyed !");
                        
                        cont = 0;
                }
                                                                                   
                // FIN DE VOS APPELS
                /***********************************************************************/
                //affichage du jeu à chaque tour
                efface_fenetre(pWinSurf);
                prepareAllSpriteDuJeu(jeu,tabParcours,LARGEURJEU,HAUTEURJEU,TabSprite,pWinSurf);
                main_attaque(jeu, &playerAttaque, &playerDefense, pWinSurf, pWindow);
                


                maj_fenetre(pWindow);
                SDL_Delay(300);  //valeur du d�lai � modifier �ventuellement


                //LECTURE DE CERTAINES TOUCHES POUR LANCER LES RESTAURATIONS ET SAUVEGARDES
                const Uint8* pKeyStates = SDL_GetKeyboardState(NULL);
                if ( pKeyStates[SDL_SCANCODE_V] ){ //touche V appuyé
                        sauvegardeBinaire(playerAttaque, playerDefense, tabParcours);
                        message("Save", "Binary Save successful");

                        //Ne pas modifiez les 4 lignes ci-dessous
                        efface_fenetre(pWinSurf);
                        prepareAllSpriteDuJeu(jeu,tabParcours,LARGEURJEU,HAUTEURJEU,TabSprite,pWinSurf);
                        maj_fenetre(pWindow);
                        SDL_Delay(300);
                }
                if ( pKeyStates[SDL_SCANCODE_C] ){ //touche C appuyé
                        nettoyerPartie(&playerAttaque, &playerDefense, jeu);
                        viderListeCoord(&tabTourAir);
                        viderListeCoord(&tabTourSol);

                        chargementBinaire(&playerAttaque, &playerDefense, jeu, tabParcours);

                        tabTourAir = initTabPositionToursAir(jeu, tabParcours);
                        tabTourSol = initTabPositionToursSol(jeu, tabParcours);
                        message("Load", "Binary Load successful");

                        //Ne pas modifiez les 4 lignes ci-dessous
                        efface_fenetre(pWinSurf);
                        prepareAllSpriteDuJeu(jeu,tabParcours,LARGEURJEU,HAUTEURJEU,TabSprite,pWinSurf);
                        maj_fenetre(pWindow);
                        SDL_Delay(300);
                }
                if ( pKeyStates[SDL_SCANCODE_D] ){  //touche D appuy
                        sauvegardeSequentielle(playerAttaque, playerDefense, tabParcours);
                        message("Save", "Sequential Save successful");

                        //Ne pas modifiez les 4 lignes ci-dessous
                        efface_fenetre(pWinSurf);
                        prepareAllSpriteDuJeu(jeu,tabParcours,LARGEURJEU,HAUTEURJEU,TabSprite,pWinSurf);
                        maj_fenetre(pWindow);
                        SDL_Delay(300);
                }
                if ( pKeyStates[SDL_SCANCODE_S] ){  //touche S appuyé
                        // 1. Nettoyage complet
                        nettoyerPartie(&playerAttaque, &playerDefense, jeu);
                        viderListeCoord(&tabTourAir);
                        viderListeCoord(&tabTourSol);

                        chargementSequentiel(&playerAttaque, &playerDefense, jeu, tabParcours);

                        tabTourAir = initTabPositionToursAir(jeu, tabParcours);
                        tabTourSol = initTabPositionToursSol(jeu, tabParcours);
                        message("Load", "Sequential Load successful");

                        //Ne pas modifiez les 4 lignes ci-dessous
                        efface_fenetre(pWinSurf);
                        prepareAllSpriteDuJeu(jeu,tabParcours,LARGEURJEU,HAUTEURJEU,TabSprite,pWinSurf);
                        maj_fenetre(pWindow);
                        SDL_Delay(300);
                }
                if ( pKeyStates[SDL_SCANCODE_ESCAPE] ){
                        cont = 0;  //sortie de la boucle
                }

        }
        //fin boucle du jeu

        SDL_FreeSurface(pSpriteTourSol); // Lib�ration de la ressource occup�e par le sprite
        SDL_FreeSurface(pSpriteTourAir);
        SDL_FreeSurface(pSpriteTourRoi);
        SDL_FreeSurface(pSpriteArcher);
        SDL_FreeSurface(pSpriteChevalier);
        SDL_FreeSurface(pSpriteDragon);
        SDL_FreeSurface(pSpriteGargouille);
        SDL_FreeSurface(pSpriteEau);
        SDL_FreeSurface(pSpriteHerbe);
        SDL_FreeSurface(pSpritePont);
        SDL_FreeSurface(pWinSurf);
    }
    else
    {
        fprintf(stdout,"Failed to load sprite (%s)\n",SDL_GetError());
    }

    SDL_DestroyWindow(pWindow);
    SDL_Quit();
    freeChemin(tabParcours);
    return 0;
}
