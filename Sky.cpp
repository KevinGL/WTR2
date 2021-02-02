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

float WTR2_Sky::radiusSky=2.0;
float WTR2_Sky::ratioSunset=-0.1;

void WTR2_Sky::Init(const float SpeedSun,float hour,int day,int month,double latitude,string pathStars,WTR2_Color cHorizon,WTR2_Color cZenit,WTR2_Color cSun,WTR2_Color cSunSunset,const int wWindow,const int hWindow)
{
    colorHorizon.R=cHorizon.R;
    colorHorizon.G=cHorizon.G;
    colorHorizon.B=cHorizon.B;

    colorZenit.R=cZenit.R;
    colorZenit.G=cZenit.G;
    colorZenit.B=cZenit.B;

    shader=SHD_LoadShaders("Mini-libs/wtr2utils/shaderSky.");       //Chargement shaders
    shaderStars=SHD_LoadShaders("Mini-libs/wtr2utils/shaderStars.");

    sun.Init(SpeedSun,hour,day,month,latitude,cSun,cSunSunset,wWindow,hWindow);     //Initialisation Soleil

    InitStars(pathStars);       //Initialisation étoiles

    sphere.InitSphere(radiusSky,20,20);     //Initialisation sphère de ciel
}

void WTR2_Sky::Draw(const float xCam,const float yCam,const float zCam,glm::mat4 projection,glm::mat4 modelview)
{
    glDisable(GL_DEPTH_TEST);       //Désactivation Z-buffer

    float posSun2[3];

    glUseProgram(shader);

    glUniform3f(glGetUniformLocation(shader,"colorSkyHorizon"),colorHorizon.R,colorHorizon.G,colorHorizon.B);
    glUniform3f(glGetUniformLocation(shader,"colorSkyZenith"),colorZenit.R,colorZenit.G,colorZenit.B);
    glUniform1f(glGetUniformLocation(shader,"hauteurSun"),sun.getPos()[2]);
    glUniform1f(glGetUniformLocation(shader,"hauteurSunMax"),sun.getMax());
    glUniform1f(glGetUniformLocation(shader,"hauteurSunMin"),sun.getMin());
    glUniform1f(glGetUniformLocation(shader,"radiusSky"),radiusSky);
    glUniform1f(glGetUniformLocation(shader,"rapportSunset"),ratioSunset);
    glUniform3f(glGetUniformLocation(shader,"colorSunSunset"),sun.getColorSunset().R,sun.getColorSunset().G,sun.getColorSunset().B);
    //Envoi couleurs et hauteurs de Soleil au fragment shader

    Transform t;
    vector<Transform> transf;

    t.translate.x=xCam;
    t.translate.y=yCam;
    t.translate.z=zCam;
    t.type=TYPE_TRANSF_TRANSLATE;

    transf.push_back(t);

    glm::mat4 transformations=getMatrixtransformation(transf);      //Calcul matrice de transformation géomatrique

    sphere.Draw(shader,projection,modelview,transformations);       //Rendu sphère

    glEnable(GL_DEPTH_TEST);

    if(stars.getNbStars()!=0)
    {
        glUseProgram(shaderStars);
        DrawStars(xCam,yCam,zCam,radiusSky,projection,modelview);   //Rendu étoiles
        glUseProgram(shader);
    }

    sun.Draw(xCam,yCam,zCam,ratioSunset,projection,modelview);      //Rendu Soleil
}
