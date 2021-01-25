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

void WTR2_Sun::Init(const float Speed,float hour,int day,int month,double latitude,WTR2_Color colNorm,WTR2_Color colSunset,const int wWindow,const int hWindow)
{
    speed=Speed;            //Vitesse défilement journée
    colorNormal=colNorm;    //Couleur normale
    colorSunset=colSunset;  //Couleur au coucher

    angleH_Init=-(hour/24.0)*360.0;

    const float largHorizon=10.0;
    const double obliquite=23.0;

    //heightOrbit=largHorizon*sin(obliquite*M_PI/180);
    const float hautMax=largHorizon*sin(obliquite*M_PI/180);        //Calcul orbite

    int nbJours[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    int compteur=0,date=0;

    date=day;
    if(month!=1)
    {
        while(1)
        {
            date+=nbJours[compteur];

            compteur++;
            if(compteur==month-1)
                break;
        }
    }

    const float pulsation=(2*M_PI)/365.0;
    const int variable=date+10;             //Calcul date en jours

    heightOrbit=-hautMax*cos(pulsation*variable);

    radiusOrbit=largHorizon*cos(obliquite*M_PI/180);        //Rayon orbite

    angleV=90-latitude;     //Inclinaison orbite

    ////////////////////////////////////////////////////////////////////////

    const float distance=1.0;
    const double diametreApparent=100.0;//20.0;//0.5;
    const float rayon=distance*tan((diametreApparent/2)*M_PI/180);      //Calcul rayon

    vbo.coordVertices.push_back(0.0);       //Calcul coordonnées sommets
    vbo.coordVertices.push_back(rayon);
    vbo.coordVertices.push_back(-rayon);

    vbo.coordVertices.push_back(0.0);
    vbo.coordVertices.push_back(rayon);
    vbo.coordVertices.push_back(rayon);

    vbo.coordVertices.push_back(0.0);
    vbo.coordVertices.push_back(-rayon);
    vbo.coordVertices.push_back(-rayon);

    //////////////////

    vbo.coordVertices.push_back(0.0);
    vbo.coordVertices.push_back(-rayon);
    vbo.coordVertices.push_back(-rayon);

    vbo.coordVertices.push_back(0.0);
    vbo.coordVertices.push_back(rayon);
    vbo.coordVertices.push_back(rayon);

    vbo.coordVertices.push_back(0.0);
    vbo.coordVertices.push_back(-rayon);
    vbo.coordVertices.push_back(rayon);

    //////////////////////////////////////

    vbo.coordTex.push_back(0.0);
    vbo.coordTex.push_back(0.0);

    vbo.coordTex.push_back(0.0);
    vbo.coordTex.push_back(1.0);

    vbo.coordTex.push_back(1.0);
    vbo.coordTex.push_back(0.0);

    //////////////////

    vbo.coordTex.push_back(1.0);
    vbo.coordTex.push_back(0.0);

    vbo.coordTex.push_back(0.0);
    vbo.coordTex.push_back(1.0);

    vbo.coordTex.push_back(1.0);
    vbo.coordTex.push_back(1.0);

    //////////////////////////////////////

    createTex(wWindow,hWindow);         //Création texture par FBO

    //////////////////////////////////////

    glGenBuffers(1,&vbo.bufferVRAM);       //Mise en mémoire VRAM
    glGenVertexArrays(1,&vbo.VAO);

    glBindBuffer(GL_ARRAY_BUFFER,vbo.bufferVRAM);
    glBindVertexArray(vbo.VAO);

    glBufferData(GL_ARRAY_BUFFER,vbo.coordVertices.size()*sizeof(float)+vbo.coordTex.size()*sizeof(float),NULL,GL_STREAM_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER,0,vbo.coordVertices.size()*sizeof(float),&vbo.coordVertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER,vbo.coordVertices.size()*sizeof(float),vbo.coordTex.size()*sizeof(float),&vbo.coordTex[0]);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)(vbo.coordVertices.size()*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    //////////////////////////////////////

    shader=SHD_LoadShaders("Mini-libs/wtr2utils/shaderSun.");       //Chargement shader

    chrono=SDL_GetTicks();      //Lancement chrono
}

void WTR2_Sun::createTex(const int wWindow,const int hWindow)
{
    GLuint FBO;                 //Initialisation FBO
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);

    GLuint tex;                 //Initialisation texture
    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D,tex);

    glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,tex,0);        //Attachement texture-rendu

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,wWindow,hWindow,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);      //Allocation mémoire texture

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);        //Paramètres texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER,0);

    GLenum status=glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status!=GL_FRAMEBUFFER_COMPLETE)
        printf("Error frame buffer : 0x%x\n",status);

    //Dessin texture

    const float baseTriangles=0.1;
    const int hauteurMin=5;
    const int hauteurMax=100;
    const int nbTriangles=200;
    const float alpha=360.0/nbTriangles;
    double angle=0.0;

    float vertices[nbTriangles*3*2];
    float couleurs[nbTriangles*3*4];

    int compteur=0,compteurVertices=0,compteurCouleurs=0;
    while(1)
    {
        const float hauteur=(rand()%(hauteurMax-hauteurMin+1)+hauteurMin)/100.0;

        glm::vec4 sommets[3];

        sommets[0]=glm::vec4(-baseTriangles/2,0.0,0.0,1.0);
        sommets[1]=glm::vec4(baseTriangles/2,0.0,0.0,1.0);
        sommets[2]=glm::vec4(0.0,hauteur,0.0,1.0);

        glm::mat4 matrice=MTH_GetMatrixRotate(angle,glm::vec3(0.0,0.0,1.0));

        sommets[0]=sommets[0]*matrice;
        sommets[1]=sommets[1]*matrice;
        sommets[2]=sommets[2]*matrice;

        vertices[compteurVertices]=sommets[0].x;
        vertices[compteurVertices+1]=sommets[0].y;

        vertices[compteurVertices+2]=sommets[1].x;
        vertices[compteurVertices+3]=sommets[1].y;

        vertices[compteurVertices+4]=sommets[2].x;
        vertices[compteurVertices+5]=sommets[2].y;

        ///////////////////////////////////////////////////////////

        couleurs[compteurCouleurs]=1.0;
        couleurs[compteurCouleurs+1]=1.0;
        couleurs[compteurCouleurs+2]=1.0;
        couleurs[compteurCouleurs+3]=1.0;

        couleurs[compteurCouleurs+4]=1.0;
        couleurs[compteurCouleurs+5]=1.0;
        couleurs[compteurCouleurs+6]=1.0;
        couleurs[compteurCouleurs+7]=1.0;

        couleurs[compteurCouleurs+8]=1.0;
        couleurs[compteurCouleurs+9]=1.0;
        couleurs[compteurCouleurs+10]=1.0;
        couleurs[compteurCouleurs+11]=0.0;

        compteurVertices+=6;
        compteurCouleurs+=12;

        angle+=alpha;

        compteur++;
        if(compteur==nbTriangles)
            break;
    }

    GLuint shaderCreate=SHD_LoadShaders("Mini-libs/wtr2utils/shaderCreateSun.");        //Chargement shader affichage FBO

    GLuint VBO_create,VAO_create;

    glGenBuffers(1,&VBO_create);            //Mise en mémoire vidéo
    glGenVertexArrays(1,&VAO_create);

    glBindBuffer(GL_ARRAY_BUFFER,VBO_create);
    glBindVertexArray(VAO_create);

    glBufferData(GL_ARRAY_BUFFER,nbTriangles*3*2*sizeof(float)+nbTriangles*3*4*sizeof(float),NULL,GL_STREAM_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER,0,nbTriangles*3*2*sizeof(float),vertices);
    glBufferSubData(GL_ARRAY_BUFFER,nbTriangles*3*2*sizeof(float),nbTriangles*3*4*sizeof(float),couleurs);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(nbTriangles*3*2*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER,0);

    /////////////////////////////////////////

    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glClearColor(1.0,1.0,1.0,0.0);
    //glClearColor(1.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO_create);
    glBindTexture(GL_TEXTURE_2D,tex);

    glUseProgram(shaderCreate);

    glDrawArrays(GL_TRIANGLES,0,nbTriangles*3);     //Rendu dans la texture

    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);

    glEnable(GL_DEPTH_TEST);

    vbo.textures.push_back(tex);
}

float* WTR2_Sun::getPos(void)
{
    return pos;
}

float WTR2_Sun::getMin(void)
{
    return zMin;
}

float WTR2_Sun::getMax(void)
{
    return zMax;
}

WTR2_Color WTR2_Sun::getColor(void)
{
    return color;
}

WTR2_Color WTR2_Sun::getColorNormal(void)
{
    return colorNormal;
}

WTR2_Color WTR2_Sun::getColorSunset(void)
{
    return colorSunset;
}

double WTR2_Sun::getAngleH(void)
{
    return angleH;
}

double WTR2_Sun::getAngleV(void)
{
    return angleV;
}

WTR2_Sun* WTR2_Sky::getSun(void)
{
    return &sun;
}

float WTR2_Sun::getHeightOrbit(void)
{
    return heightOrbit;
}

float WTR2_Sun::getRadiusOrbit(void)
{
    return radiusOrbit;
}

void WTR2_Sun::Draw(const float xCam,const float yCam,const float zCam,glm::mat4 projection,glm::mat4 modelview)
{
    glDisable(GL_DEPTH_TEST);       //Désactivation test de profondeur

    const double heuresEcoulees=(SDL_GetTicks()-chrono)/3600000.0;      //Conversion chrono
    angleH=angleH_Init-speed*(heuresEcoulees/24.0)*360.0;               //Calcul position selon heure

    float posCentreCercle[3];

    posCentreCercle[0]=heightOrbit*sin(angleV*M_PI/180);
    posCentreCercle[1]=0.0;
    posCentreCercle[2]=heightOrbit*cos(angleV*M_PI/180);

    pos[0]=posCentreCercle[0]+radiusOrbit*cos(angleH*M_PI/180)*cos(angleV*M_PI/180);
    pos[1]=posCentreCercle[1]+radiusOrbit*sin(angleH*M_PI/180);
    pos[2]=posCentreCercle[2]-(radiusOrbit*sin(angleV*M_PI/180))*cos(angleH*M_PI/180);

    zMax=posCentreCercle[2]-(radiusOrbit*sin(angleV*M_PI/180))*cos(180.0*M_PI/180);
    zMin=posCentreCercle[2]-(radiusOrbit*sin(angleV*M_PI/180))*cos(0.0*M_PI/180);       //Calcul paramètres orbite

    float pente[3];

    pente[0]=(colorNormal.R-colorSunset.R)/(0.1*zMax);      //Calcul couleur selon heure
    pente[1]=(colorNormal.G-colorSunset.G)/(0.1*zMax);
    pente[2]=(colorNormal.B-colorSunset.B)/(0.1*zMax);

    if(pos[2]>=0&&pos[2]<0.1*zMax)
    {
        color.R=pente[0]*fabs(pos[2])+colorSunset.R;
        color.G=pente[1]*fabs(pos[2])+colorSunset.G;
        color.B=pente[2]*fabs(pos[2])+colorSunset.B;
    }

    else
    if(pos[2]>=0.1*zMax)
    {
        color.R=colorNormal.R;
        color.G=colorNormal.G;
        color.B=colorNormal.B;
    }

    else
    {
        color.R=colorSunset.R;
        color.G=colorSunset.G;
        color.B=colorSunset.B;
    }

    glUseProgram(shader);

    vector<Transform> transf;       //Transformations géométriques
    Transform t;

    t.translate.x=xCam;
    t.translate.y=yCam;
    t.translate.z=zCam;
    t.type=TYPE_TRANSF_TRANSLATE;
    transf.push_back(t);

    t.angle=angleV;
    t.axe=glm::vec3(0.0,1.0,0.0);
    t.type=TYPE_TRANSF_ROTATE;
    transf.push_back(t);

    t.translate.x=0.0;
    t.translate.y=0.0;
    t.translate.z=heightOrbit;
    t.type=TYPE_TRANSF_TRANSLATE;
    transf.push_back(t);

    t.angle=angleH;
    t.axe=glm::vec3(0.0,0.0,1.0);
    t.type=TYPE_TRANSF_ROTATE;
    transf.push_back(t);

    t.translate.x=radiusOrbit;
    t.translate.y=0.0;
    t.translate.z=0.0;
    t.type=TYPE_TRANSF_TRANSLATE;
    transf.push_back(t);

    glm::mat4 matriceTransf=getMatrixtransformation(transf);        //Calcul matrice de transformation

    glUniformMatrix4fv(glGetUniformLocation(shader,"transformation"),1,false,glm::value_ptr(matriceTransf));      //Envoi matrices au shader
    glUniformMatrix4fv(glGetUniformLocation(shader,"projection"),1,false,glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader,"modelview"),1,false,glm::value_ptr(modelview));

    glUniform3f(glGetUniformLocation(shader,"color"),color.R,color.G,color.B);      //Envoi couleur au shader

    glBindTexture(GL_TEXTURE_2D,vbo.textures[0]);

    glBindVertexArray(vbo.VAO);

    glDrawArrays(GL_TRIANGLES,0,vbo.coordVertices.size()/3);        //Rendu Soleil

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

void WTR2_calculPosSun(float *posSun1,float *posSun2)
{
    MTH_Sphere sphere;

    sphere.center.X=0.0;            //Caméra
    sphere.center.Y=0.0;
    sphere.center.Z=0.0;

    sphere.radius=WTR2_RADIUS_SKY;

    MTH_Vector vecteur;

    vecteur.X=posSun1[0]-0.0;
    vecteur.Y=posSun1[1]-0.0;
    vecteur.Z=posSun1[2]-0.0;

    MTH_RightSpace droite;

    droite.A.X=0.0;            //Caméra
    droite.A.Y=0.0;
    droite.A.Z=0.0;

    droite.V.X=vecteur.X;
    droite.V.Y=vecteur.Y;
    droite.V.Z=vecteur.Z;

    vector<MTH_Vertex> solutions;

    MTH_CalculInterRight_Sphere(&sphere,&droite,solutions);

    MTH_Vector vecteur2;

    if(solutions.size()!=0)
    {
        vecteur2.X=solutions[0].X-0.0;
        vecteur2.Y=solutions[0].Y-0.0;
        vecteur2.Z=solutions[0].Z-0.0;

        if(vecteur2.X*vecteur.X+vecteur2.Y*vecteur.Y+vecteur2.Z*vecteur.Z>0)        //Produit scalaire
        {
            posSun2[0]=solutions[0].X;
            posSun2[1]=solutions[0].Y;
            posSun2[2]=solutions[0].Z;
        }
        else
        {
            posSun2[0]=solutions[1].X;
            posSun2[1]=solutions[1].Y;
            posSun2[2]=solutions[1].Z;
        }
    }
}
