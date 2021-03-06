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

void WTR2_Sky::InitStars(string pathStars)
{
    FILE *fichier=fopen(pathStars.c_str(),"r");     //Chargement fichier contenant param�tres �toiles

    float ascension,declinaison;
    float eclat,amplitude,frequence;
    int retour_fscanf;
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
    }       //Lecture fichier

    fclose(fichier);

    stars.Init(ascensions_droites,declinaisons,brillances,amplitudes,frequences);       //Initalisation instances et VBO

    chrono=SDL_GetTicks();
}

int WTR2_Stars::getNbStars(void)
{
    return right_asc.size();
}

void WTR2_Stars::Init(vector<float> &r,vector<float> &d,vector<float> &s,vector<float> &a,vector<float> &f)
{
    right_asc=r;
    declinations=d;
    shinings=s;
    amplitudes=a;
    frequencies=f;

    const int tailleMemoire=2*3*3*sizeof(float)+2*3*2*sizeof(float)+right_asc.size()*sizeof(float)+declinations.size()*sizeof(float)+shinings.size()*sizeof(float)+amplitudes.size()*sizeof(float)+frequencies.size()*sizeof(float);

    float coordVertices[2*3*3];     //Un carr� de deux triangles de trois vertices de trois coordonn�es chacun
    float coordTextures[2*3*2];

    coordVertices[0]=0.0;               //Calcul coordonn�es vertices d'une instance
    coordVertices[1]=WTR2_SIZE_STARS/2;
    coordVertices[2]=-WTR2_SIZE_STARS/2;

    coordVertices[3]=0.0;
    coordVertices[4]=WTR2_SIZE_STARS/2;
    coordVertices[5]=WTR2_SIZE_STARS/2;

    coordVertices[6]=0.0;
    coordVertices[7]=-WTR2_SIZE_STARS/2;
    coordVertices[8]=-WTR2_SIZE_STARS/2;

    coordVertices[9]=0.0;
    coordVertices[10]=-WTR2_SIZE_STARS/2;
    coordVertices[11]=-WTR2_SIZE_STARS/2;

    coordVertices[12]=0.0;
    coordVertices[13]=WTR2_SIZE_STARS/2;
    coordVertices[14]=WTR2_SIZE_STARS/2;

    coordVertices[15]=0.0;
    coordVertices[16]=-WTR2_SIZE_STARS/2;
    coordVertices[17]=WTR2_SIZE_STARS/2;

    ///////////////////////////////////////////

    coordTextures[0]=0.0;       //Coordonn�es texture
    coordTextures[1]=0.0;

    coordTextures[2]=0.0;
    coordTextures[3]=1.0;

    coordTextures[4]=1.0;
    coordTextures[5]=0.0;

    coordTextures[6]=1.0;
    coordTextures[7]=0.0;

    coordTextures[8]=0.0;
    coordTextures[9]=1.0;

    coordTextures[10]=1.0;
    coordTextures[11]=1.0;

    ///////////////////////////////////////////

    glGenVertexArrays(1,&VAO);      //Initialisation VBO
    glGenBuffers(1,&bufferVRAM);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,bufferVRAM);

    glBufferData(GL_ARRAY_BUFFER,tailleMemoire,NULL,GL_STREAM_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER,0,2*3*3*sizeof(float),coordVertices);
    glBufferSubData(GL_ARRAY_BUFFER,2*3*3*sizeof(float),2*3*2*sizeof(float),coordTextures);
    glBufferSubData(GL_ARRAY_BUFFER,2*3*3*sizeof(float)+2*3*2*sizeof(float),right_asc.size()*sizeof(float),&right_asc[0]);
    glBufferSubData(GL_ARRAY_BUFFER,2*3*3*sizeof(float)+2*3*2*sizeof(float)+right_asc.size()*sizeof(float),declinations.size()*sizeof(float),&declinations[0]);
    glBufferSubData(GL_ARRAY_BUFFER,2*3*3*sizeof(float)+2*3*2*sizeof(float)+right_asc.size()*sizeof(float)+declinations.size()*sizeof(float),shinings.size()*sizeof(float),&shinings[0]);
    glBufferSubData(GL_ARRAY_BUFFER,2*3*3*sizeof(float)+2*3*2*sizeof(float)+right_asc.size()*sizeof(float)+declinations.size()*sizeof(float)+shinings.size()*sizeof(float),amplitudes.size()*sizeof(float),&amplitudes[0]);
    glBufferSubData(GL_ARRAY_BUFFER,2*3*3*sizeof(float)+2*3*2*sizeof(float)+right_asc.size()*sizeof(float)+declinations.size()*sizeof(float)+shinings.size()*sizeof(float)+amplitudes.size()*sizeof(float),frequencies.size()*sizeof(float),&frequencies[0]);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)(2*3*3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,sizeof(float),(void*)(2*3*3*sizeof(float)+2*3*2*sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3,1,GL_FLOAT,GL_FALSE,sizeof(float),(void*)(2*3*3*sizeof(float)+2*3*2*sizeof(float)+right_asc.size()*sizeof(float)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4,1,GL_FLOAT,GL_FALSE,sizeof(float),(void*)(2*3*3*sizeof(float)+2*3*2*sizeof(float)+right_asc.size()*sizeof(float)+declinations.size()*sizeof(float)));
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(5,1,GL_FLOAT,GL_FALSE,sizeof(float),(void*)(2*3*3*sizeof(float)+2*3*2*sizeof(float)+right_asc.size()*sizeof(float)+declinations.size()*sizeof(float)+shinings.size()*sizeof(float)));
    glEnableVertexAttribArray(5);

    glVertexAttribPointer(6,1,GL_FLOAT,GL_FALSE,sizeof(float),(void*)(2*3*3*sizeof(float)+2*3*2*sizeof(float)+right_asc.size()*sizeof(float)+declinations.size()*sizeof(float)+shinings.size()*sizeof(float)+amplitudes.size()*sizeof(float)));
    glEnableVertexAttribArray(6);

    glVertexAttribDivisor(2,1);
    glVertexAttribDivisor(3,1);
    glVertexAttribDivisor(4,1);
    glVertexAttribDivisor(5,1);
    glVertexAttribDivisor(6,1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    texture=loadTexture("Mini-libs/wtr2utils/Stars.tga",0);     //Chargement texture
}

void WTR2_Sky::DrawStars(const float xCam,const float yCam,const float zCam,const float radiusSky,glm::mat4 projection,glm::mat4 modelview)
{
    glDisable(GL_DEPTH_TEST);       //D�sactivation Z-buffer

    const float hauteurSoleil=sun.getPos()[2];
    const float ratioStars=-0.1;

    const float pente=-1.0/(0.0-ratioStars*sun.getMax());
    float opacite;

    if(hauteurSoleil>=ratioStars*sun.getMax()&&hauteurSoleil<=0.0)      //Calcul opacit� selon hauteur Soleil
        opacite=pente*hauteurSoleil;
    else
    if(hauteurSoleil<ratioStars*sun.getMax())
        opacite=1.0;
    else
        opacite=0.0;

    const float variable=(SDL_GetTicks()-chrono)/1000.0;

    stars.Draw(xCam,yCam,zCam,opacite,variable,radiusSky,shaderStars,projection,modelview);     //Rendu

    glEnable(GL_DEPTH_TEST);
}

void WTR2_Stars::Draw(const float xCam,const float yCam,const float zCam,const float opacity,const float var,const float radiusSky,const GLuint shader,glm::mat4 projection,glm::mat4 modelview)
{
    glUniform3f(glGetUniformLocation(shader,"posCam"),xCam,yCam,zCam);      //Envoi param�tres au shader
    glUniform1f(glGetUniformLocation(shader,"radiusSky"),radiusSky);
    glUniform1f(glGetUniformLocation(shader,"opacityStars"),opacity);
    glUniform1f(glGetUniformLocation(shader,"varStars"),var);

    glUniformMatrix4fv(glGetUniformLocation(shader,"projection"),1,false,glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader,"modelview"),1,false,glm::value_ptr(modelview));

    glBindTexture(GL_TEXTURE_2D,texture);

    glBindVertexArray(VAO);

    glDrawArraysInstanced(GL_TRIANGLES,0,6,right_asc.size());       //Rendu

    glBindVertexArray(0);
}
