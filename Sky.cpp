#include <glew.h>
#include <gl.h>
#include <glu.h>
#include <math.h>
#include <sdl.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include "wtr2utils.h"
#include "../shaderutils/shaderutils.h"
#include "../sdlglutils/sdlglutils.h"
#include "../mathutils/mathutils.h"

void WTR2_Sky::Init(const float SpeedSun,float hour,int day,int month,double latitude,string pathStars,WTR2_Color cHorizon,WTR2_Color cZenit,WTR2_Color cSun,WTR2_Color cSunSunset,const int wWindow,const int hWindow)
{
    colorHorizon.R=cHorizon.R;      //Couleur horizon
    colorHorizon.G=cHorizon.G;
    colorHorizon.B=cHorizon.B;

    colorZenit.R=cZenit.R;          //Couleur zenith
    colorZenit.G=cZenit.G;
    colorZenit.B=cZenit.B;

    shader=SHD_LoadShaders("Mini-libs/wtr2utils/shaderSky.");       //Chargement shader ciel
    shaderStars=SHD_LoadShaders("Mini-libs/wtr2utils/shaderStars.");    //Chargement shader étoiles

    sun.Init(SpeedSun,hour,day,month,latitude,cSun,cSunSunset,wWindow,hWindow);     //Initialisation Soleil

    InitStars(pathStars);           //Initialisation étoiles

    sphere.InitSphere(WTR2_RADIUS_SKY,20,20);       //Initialisation sphère ciel
}

void WTR2_Sky::InitStars(string pathStars)
{
    FILE *fichier=fopen(pathStars.c_str(),"r");     //Chargement fichier paramètres étoiles

    float ascension,declinaison;
    float eclat,amplitude,frequence;
    int retour_fscanf;
    //WTR2_Star s;
    vector<float> ascensions_droites;
    vector<float> declinaisons;
    vector<float> brillances;
    vector<float> amplitudes;
    vector<float> frequences;

    while(1)
    {
        retour_fscanf=fscanf(fichier,"Right ascension : %f | Declination :  %f | Shiness : %f | Amplitude : %f | Frequency : %f\n",&ascension,&declinaison,&eclat,&amplitude,&frequence);
        if(retour_fscanf<0)
            break;

        ascensions_droites.push_back(ascension*M_PI/180);
        declinaisons.push_back(declinaison*M_PI/180);
        brillances.push_back(eclat);
        amplitudes.push_back(amplitude);
        frequences.push_back(frequence);
    }

    fclose(fichier);

    stars.Init(ascensions_droites,declinaisons,brillances,amplitudes,frequences);       //Initialisation

    chrono=SDL_GetTicks();
}

void WTR2_Sky::Draw(const float xCam,const float yCam,const float zCam,glm::mat4 projection,glm::mat4 modelview)
{
    glDisable(GL_DEPTH_TEST);

    float posSun2[3];

    WTR2_calculPosSun(sun.getPos(),posSun2);        //Coordonnées du Soleil sur la sphère de ciel

    glUseProgram(shader);

    glUniform3f(glGetUniformLocation(shader,"colorSkyHorizon"),colorHorizon.R,colorHorizon.G,colorHorizon.B);
    glUniform3f(glGetUniformLocation(shader,"colorSkyZenit"),colorZenit.R,colorZenit.G,colorZenit.B);
    glUniform3f(glGetUniformLocation(shader,"colorSun"),sun.getColor().R,sun.getColor().G,sun.getColor().B);
    glUniform3f(glGetUniformLocation(shader,"posSun2"),posSun2[0],posSun2[1],posSun2[2]);
    glUniform1f(glGetUniformLocation(shader,"radiusSky"),WTR2_RADIUS_SKY);
    glUniform1f(glGetUniformLocation(shader,"redSunNormal"),sun.getColorNormal().R);
    glUniform1f(glGetUniformLocation(shader,"redSunSunset"),sun.getColorSunset().R);        //Envoi paramètres au shader

    Transform t;        //Transformations géométriques
    vector<Transform> transf;

    t.translate.x=xCam;
    t.translate.y=yCam;
    t.translate.z=zCam;
    t.type=TYPE_TRANSF_TRANSLATE;

    transf.push_back(t);

    glm::mat4 transformations=getMatrixtransformation(transf);      //Calcul matrice de transformation

    sphere.Draw(shader,projection,modelview,transformations);       //Rendu ciel

    glEnable(GL_DEPTH_TEST);

    if(stars.getNbStars()!=0)
    {
        glUseProgram(shaderStars);
        DrawStars(xCam,yCam,zCam,WTR2_RADIUS_SKY,projection,modelview);     //Rendu étoiles
        glUseProgram(shader);
    }

    //glUniform1i(glGetUniformLocation(shader,"sun"),true);
    sun.Draw(xCam,yCam,zCam,projection,modelview);              //Rendu Soleil
    //glUniform1i(glGetUniformLocation(shader,"sun"),false);
}

void WTR2_Sky::DrawStars(const float xCam,const float yCam,const float zCam,const float radiusSky,glm::mat4 projection,glm::mat4 modelview)
{
    glDisable(GL_DEPTH_TEST);

    const float pente=1.0/(sun.getColorSunset().R-sun.getColorNormal().R);
    const float ord=-pente*sun.getColorSunset().R+1.0;
    const float opacite=pente*sun.getColor().R+ord;     //Opacité selon heure journée

    const float variable=(SDL_GetTicks()-chrono)/1000.0;        //Chronomètre

    stars.Draw(xCam,yCam,zCam,opacite,variable,radiusSky,shaderStars,projection,modelview);     //Rendu étoiles

    glEnable(GL_DEPTH_TEST);
}


