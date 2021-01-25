#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <gl.h>
#include <glu.h>
#include <time.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

void createBarre(SDL_Surface **barre);
void generateStars(const int nbStars);

int main(int argc, char* argv[])
{
    SDL_Window* fenetre;
    SDL_Event evenement;
    SDL_Surface *bouton;
    SDL_Rect posBouton;

    srand(time(NULL));

    if(SDL_Init(SDL_INIT_VIDEO)<0)      //Initialisation SDL
    {
        printf("Erreur initialisation SDL, presser une touche pour quitter");
        getch();
        SDL_Quit();
    }

    TTF_Init();     //Initialisation TTF

    fenetre=SDL_CreateWindow("WTR2 - Stars Maker",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,200,SDL_WINDOW_SHOWN);
    //Création fenêtre

    /////////////////////////////////////////////////////////////////////////////////////////

    SDL_Surface *fond=SDL_CreateRGBSurface(0,800,200,32,0,0,0,0);       //Fond
    SDL_FillRect(fond,NULL,SDL_MapRGB(fond->format,0,0,0));

    SDL_Rect posFond;

    posFond.x=0;
    posFond.y=0;

    /////////////////////////////////////////////////////////////////////////////////////////

    TTF_Font *police=TTF_OpenFont("data/ARCENA.ttf",20);        //Chargement police
    bouton=TTF_RenderText_Shaded(police,"Generate stars",{255,255,255},{0,33,87});      //Bouton générateur d'étoiles

    posBouton.x=(800-bouton->w)/2;      //Bouton centré
    posBouton.y=200-bouton->h;

    /////////////////////////////////////////////////////////////////////////////////////////

    SDL_Surface *barre;         //Barre
    createBarre(&barre);

    SDL_Rect posBarre;

    posBarre.x=0;
    posBarre.y=(200-barre->h)/2;

    /////////////////////////////////////////////////////////////////////////////////////////

    SDL_Surface *curseur=SDL_LoadBMP("Mini-libs/wtr2utils/curseur.bmp");        //Chargement curseur
    SDL_SetColorKey(curseur,SDL_TRUE,SDL_MapRGB(curseur->format,0,0,0));

    SDL_Rect posCurseur;

    posCurseur.x=(800-curseur->w)/2;                //Position initiale curseur
    posCurseur.y=posBarre.y+barre->h/2-curseur->h;

    /////////////////////////////////////////////////////////////////////////////////////////

    SDL_Surface *sourisMain=SDL_LoadBMP("Mini-libs/wtr2utils/sourisMain.bmp");      //Chargement curseur souris
    SDL_SetColorKey(sourisMain,SDL_TRUE,SDL_MapRGB(sourisMain->format,255,0,0));

    SDL_Rect posSouris;
    int xCurseur,yCurseur;

    /////////////////////////////////////////////////////////////////////////////////////////

    SDL_Surface *nbStarsSurface=NULL;       //Affichage nombre d'étoiles
    SDL_Rect posNbStars;

    posNbStars.x=0;
    posNbStars.y=0;

    /////////////////////////////////////////////////////////////////////////////////////////

    bool boutonEnfonce=false;
    int yClic=0;

    const int minStars=100;
    const int maxStars=5000;
    int nbStars;

    while(1)        //Boucle principale
    {
        SDL_PollEvent(&evenement);
        if(evenement.window.event==SDL_WINDOWEVENT_CLOSE)
            break;
        if(evenement.type==SDL_MOUSEBUTTONDOWN)
        {
            if(evenement.button.button==SDL_BUTTON_LEFT)        //Détecte clic gauche
            {
                boutonEnfonce=true;
                yClic=evenement.button.y;
            }

            if(evenement.button.x>=posBouton.x&&evenement.button.x<=posBouton.x+bouton->w&&
               evenement.button.y>=200-bouton->h)
                break;
        }
        else
        if(evenement.type==SDL_MOUSEBUTTONUP)
        {
            if(evenement.button.button==SDL_BUTTON_LEFT)
                boutonEnfonce=false;
        }
        else
        if(evenement.type==SDL_MOUSEMOTION)     //Mouvement souris
        {
            if(boutonEnfonce&&
               yClic>=posBarre.y&&yClic<=posBarre.y+barre->h)
            {
                posCurseur.x=evenement.motion.x;        //Déplacement curseur avec souris

                if(posCurseur.x<0)
                    posCurseur.x=0;
                else
                if(posCurseur.x>800-curseur->w)
                    posCurseur.x=800-curseur->w;
            }
        }

        const float pente=(maxStars-minStars)/(800-curseur->w);
        nbStars=pente*posCurseur.x+minStars;        //Calcul nombre étoiles

        SDL_BlitSurface(fond,NULL,SDL_GetWindowSurface(fenetre),&posFond);      //Rendu
        SDL_BlitSurface(barre,NULL,SDL_GetWindowSurface(fenetre),&posBarre);
        SDL_BlitSurface(bouton,NULL,SDL_GetWindowSurface(fenetre),&posBouton);
        SDL_BlitSurface(curseur,NULL,SDL_GetWindowSurface(fenetre),&posCurseur);

        if(nbStarsSurface!=NULL)
            SDL_FreeSurface(nbStarsSurface);
        string nbStarsStr;
        ostringstream os;
        os << nbStars;
        nbStarsStr=os.str()+" stars";
        nbStarsSurface=TTF_RenderText_Blended(police,nbStarsStr.c_str(),{255,255,255});
        os.str("");
        SDL_BlitSurface(nbStarsSurface,NULL,SDL_GetWindowSurface(fenetre),&posNbStars);     //Affichage nombre étoiles

        SDL_GetMouseState(&xCurseur,&yCurseur);     //Position curseur

        if(xCurseur>=posBouton.x&&xCurseur<=posBouton.x+bouton->w&&yCurseur>=200-bouton->h||
           xCurseur>=0&&xCurseur<=800&&yCurseur>=posBarre.y&&yCurseur<=posBarre.y+barre->h)
        {
            SDL_ShowCursor(0);

            posSouris.x=xCurseur;
            posSouris.y=yCurseur;

            SDL_BlitSurface(sourisMain,NULL,SDL_GetWindowSurface(fenetre),&posSouris);      //Changement aspect curseur
        }

        else
            SDL_ShowCursor(1);

        SDL_UpdateWindowSurface(fenetre);
    }

    SDL_FreeSurface(fond);
    SDL_FreeSurface(bouton);
    SDL_FreeSurface(barre);
    SDL_FreeSurface(curseur);
    SDL_FreeSurface(nbStarsSurface);

    generateStars(nbStars);     //Génération fichier étoiles

    SDL_Quit();
    return 0;
}

void createBarre(SDL_Surface **barre)
{
    *barre=SDL_CreateRGBSurface(0,800,10,32,0,0,0,0);       //Création barre
    const int nbParts=100;
    const int largPart=800/nbParts;
    SDL_Rect posPart;
    float pente[3];
    const int couleurDeb[3]={0,255,0};
    const int couleurFin[3]={255,0,0};
    int couleur[3];

    posPart.x=0;
    posPart.y=0;

    pente[0]=(float)(couleurFin[0]-couleurDeb[0])/(nbParts-1);
    pente[1]=(float)(couleurFin[1]-couleurDeb[1])/(nbParts-1);
    pente[2]=(float)(couleurFin[2]-couleurDeb[2])/(nbParts-1);

    SDL_Surface *part=SDL_CreateRGBSurface(0,largPart,(*barre)->h,32,0,0,0,0);          //Particule de dégradé

    int compteur=0;
    while(1)            //Rendu dégradé
    {
        couleur[0]=pente[0]*compteur+couleurDeb[0];
        couleur[1]=pente[1]*compteur+couleurDeb[1];
        couleur[2]=pente[2]*compteur+couleurDeb[2];

        SDL_FillRect(part,NULL,SDL_MapRGB(part->format,couleur[0],couleur[1],couleur[2]));

        SDL_BlitSurface(part,NULL,*barre,&posPart);

        posPart.x+=largPart;

        compteur++;
        if(compteur==nbParts)
            break;
    }

    SDL_FreeSurface(part);
}

void generateStars(const int nbStars)
{
    FILE *fichier=fopen("Mini-libs/wtr2utils/Stars.txt","w");       //Création fichier

    int compteur=0;

    while(1)
    {
        const double theta=rand()%360;      //Paramètres aléatoires
        double delta=rand()%91;
        const float eclat=(rand()%50+25)/100.0;
        float amplitude,amplitudeMax;
        const float frequence=rand()%5+1.0;
        const float amplitudeMin=0.01;

        if(rand()%2==0)
            delta*=-1;

        const float valMoy=eclat;

        if(valMoy<=0.5)
            amplitudeMax=valMoy;
        else
            amplitudeMax=1.0-valMoy;

        const int amplitudeMaxInt=(int)100*amplitudeMax;
        const int amplitudeMinInt=(int)100*amplitudeMin;

        //cout << valMoy << " " << amplitudeMax << " => ";

        amplitude=(rand()%(amplitudeMaxInt-amplitudeMinInt+1))/100.0+amplitudeMin;

        //cout << amplitude << endl;

        fprintf(fichier,"Right ascension : %lf | Declination :  %lf | Shiness : %f | Amplitude : %f | Frequency : %f\n",theta,delta,eclat,amplitude,frequence);
        //Ecriture dans fichier

        compteur++;
        if(compteur==nbStars)
            break;
    }

    fclose(fichier);
}
