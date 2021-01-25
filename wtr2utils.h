#include <gl.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../partutils/partutils.h"
#include "../primutils/primutils.h"

#define WTR2_MIN_DROPS 30
#define WTR2_MAX_DROPS 10000
#define WTR2_HEIGHT_DROP 0.1
#define WTR2_THICK_DROP 0.01
#define WTR2_Y_MAX_DROPS 6.0
#define WTR2_RADIUS_MAX_DROPS 5.0

#define WTR2_WIDTH_LIGHTNING 5.0
#define WTR2_WIDTH_LIGHTNING_H 3.2*WTR2_WIDTH_LIGHTNING
#define WTR2_WIDTH_LIGHTNING_BRANCH 2.5
#define WTR2_WIDTH_LIGHTNING_BRANCH_H 3.2*WTR2_WIDTH_LIGHTNING
#define WTR2_ANGLE_MAX_LIGHTNING 90.0
#define WTR2_HEIGHT_MAX_LIGHTNING 1000.0
#define WTR2_MIN_SEG_TRACER_LIGHTNING 30.0
#define WTR2_MAX_SEG_TRACER_LIGHTNING 50.0
#define WTR2_MIN_SEG_BRANCH_LIGHTNING 15.0
#define WTR2_MAX_SEG_BRANCH_LIGHTNING 25.0
/*#define WTR2_COLOR_R_LIGHTNING 1.0
#define WTR2_COLOR_G_LIGHTNING 222.0/255//1.0
#define WTR2_COLOR_B_LIGHTNING 222.0/255//1.0*/
#define WTR2_OPACITY_INT_LIGHTNING 0.5
#define WTR2_OPACITY_EXT_LIGHTNING 0.0
#define WTR2_ANGLE_BRANCH_LIGHTNING_UP 60.0
#define WTR2_ANGLE_BRANCH_LIGHTNING_DOWN (90.0+20.0+20.0)
#define WTR2_LENGTH_MAX_LIGHTNING_DOWN 0.5*WTR2_HEIGHT_MAX_LIGHTNING
#define WTR2_LENGTH_MIN_LIGHTNING_DOWN 0.125*WTR2_LENGTH_MAX_LIGHTNING_DOWN
#define WTR2_LENGTH_MAX_LIGHTNING_UP 1.5*WTR2_HEIGHT_MAX_LIGHTNING
#define WTR2_LENGTH_MIN_LIGHTNING_UP 0.125*WTR2_LENGTH_MAX_LIGHTNING_UP
#define WTR2_MAX_LEVEL_BRANCH 2
#define WTR2_MAX_BRANCH 5
#define WTR2_MIN_BRANCH 2
#define WTR2_RADIUS_TOP_LIGHTNING 150.0
#define WTR2_MIN_DELAY_LIGHTNING 2000
#define WTR2_MAX_DELAY_LIGHTNING 60000
#define WTR2_MIN_DURATION_LIGHTNING 20
#define WTR2_MAX_DURATION_LIGHTNING 500
#define WTR2_DURATION_LIGHTNING_WINK 1000
#define WTR2_PERIOD_LIGHTNING_WINK 10

#define WTR2_RADIUS_SKY 2.0

#define WTR2_SIZE_STARS 0.01

#define WTR2_MIN_DIST_RAINBOW 100
#define WTR2_MAX_DIST_RAINBOW 2000

#define BUFFER_OFFSET(a) ((char*)NULL + (a))

using namespace std;

enum WTR2_Rainfall
{
    WTR2_Rainfall_Low1,
    WTR2_Rainfall_Low2,
    WTR2_Rainfall_Mod1,
    WTR2_Rainfall_Mod2,
    WTR2_Rainfall_High1,
    WTR2_Rainfall_High2,
    WTR2_Rainfall_Varying,
};

enum WTR2_FrequencyRainbows
{
    WTR2_FrequencyRainbows_None,
    WTR2_FrequencyRainbows_Low1,
    WTR2_FrequencyRainbows_Low2,
    WTR2_FrequencyRainbows_Mod1,
    WTR2_FrequencyRainbows_Mod2,
    WTR2_FrequencyRainbows_High1,
    WTR2_FrequencyRainbows_High2,
    WTR2_FrequencyRainbows_Varying,
};

enum WTR2_FrequencyStorms
{
    WTR2_FrequencyStorms_None,
    WTR2_FrequencyStorms_Low1,
    WTR2_FrequencyStorms_Low2,
    WTR2_FrequencyStorms_Mod1,
    WTR2_FrequencyStorms_Mod2,
    WTR2_FrequencyStorms_High1,
    WTR2_FrequencyStorms_High2,
    WTR2_FrequencyStorms_Varying,
};

/*enum WTR2_axeRotation
{
    WTR2_AXE_X,
    WTR2_AXE_Y,
    WTR2_AXE_Z,
};*/

typedef struct WTR2_Vertex
{
    float X;
    float Y;
    float Z;
}WTR2_Vertex;

typedef struct WTR2_Color
{
    float R;
    float G;
    float B;
}WTR2_Color;

typedef struct WTR2_VBO
{
    vector<float> coordVertices;
    vector<float> coordTex;
    vector<float> normals;
    vector<float> colors;
    vector<float> attributs;
    vector<GLuint> textures;
    GLuint bufferVRAM;
    GLuint VAO;
    int nbVertices;
}WTR2_VBO;

typedef struct WTR2_Lightning
{
    WTR2_VBO structure;
    //int nbPointsTracer;
    unsigned int delayBefore;
    unsigned int duration;
    unsigned int chrono;
    bool down;
    //float mask;
    bool wink;
    float coord_pol[2];
    float coord_cart[3];
    float posCamInit[2];
    GLuint shader;
}WTR2_Lightning;

typedef struct WTR2_Rainbow
{
    WTR2_VBO structure;
    //WTR2_VBO cache;
    float opacite;
    GLuint shader;
    float hautCylindre;
    float offsetCache;
}WTR2_Rainbow;

class WTR2_Sun
{
    private :

    unsigned int chrono;
    float speed;
    double angleH_Init;
    double angleH;
    double angleV;
    float radiusOrbit;
    float heightOrbit;
    float pos[3];
    float zMin;
    float zMax;
    WTR2_Color colorSunset;
    WTR2_Color colorNormal;
    WTR2_Color color;
    WTR2_VBO vbo;
    GLuint shader;

    public :

    void Init(const float Speed,float hour,int day,int month,double latitude,WTR2_Color colNorm,WTR2_Color colSunset,const int wWindow,const int hWindow);
    void Draw(const float xCam,const float yCam,const float zCam,glm::mat4 projection,glm::mat4 modelview);
    float* getPos(void);
    float getMin(void);
    float getMax(void);
    double getAngleH(void);
    double getAngleV(void);
    float getHeightOrbit(void);
    float getRadiusOrbit(void);
    WTR2_Color getColor(void);
    WTR2_Color getColorNormal(void);
    WTR2_Color getColorSunset(void);
    void createTex(const int wWindow,const int hWindow);
};

class WTR2_Rain
{
    private :

    //WTR2_VBO drops;
    PART_system drops;
    WTR2_Rainfall rainfall;
    WTR2_FrequencyRainbows rainbows;
    WTR2_FrequencyStorms storms;
    //vector<float> speedsDrops;
    unsigned int chronoRain;
    int periodeVarying;
    int NbDropsVarying;
    GLuint texClouds;
    double angleClouds;
    float radiusLightning;
    WTR2_Lightning lightning;
    //unsigned int chronoLightning;
    vector<WTR2_Color> colorsLightning;
    WTR2_Rainbow rainbow;

    public :

    void Init(WTR2_Rainfall rainfall_Init,WTR2_FrequencyRainbows rainbows_Init,WTR2_FrequencyStorms storms_Init,const float radiusLght,const float radiusRB,const int periodeVar=1000);
    //void Draw(const int largFenetre,const int hautFenetre,glm::vec3 posCam,glm::vec3 targetCam,WTR2_Sun *sun,const GLuint shaderApp=0,const bool intern=false);
    void Draw(const int largFenetre,const int hautFenetre,glm::vec3 posCam,WTR2_Sun *sun,const GLuint shaderApp=0,const bool intern=false);
    void DrawClouds(const float radiusClouds,const float X=0,const float Y=0,const float Z=0);
    void InitLightning(void);
    void ReinitLightning(void);
    void InitDelayLightning(void);
    void InitDurationLightning(void);
    void loadColorsLightning(void);
    void InitTracerLightning(vector<WTR2_Vertex> &structure,WTR2_Color color);
    void InitTopLightning(const float xTop,const float yTop,const float zTop,WTR2_Color color);
    void InitBranchingLightning(const float xPoint,const float yPoint,const float zPoint,const float width_init,const float width_init_h,WTR2_Color color,const int level);
    void DrawLightning(const float xCam,const float yCam);
    void varyingRain(void);
    void InitRainbow(const float radius);
    //void DrawRainbow(glm::vec3 posCam,glm::vec3 targetCam,WTR2_Sun *sun);
    void DrawRainbow(glm::vec3 posCam,WTR2_Sun *sun);
};

/*class WTR2_Star
{
    private :

    double right_asc;
    double declination;
    float shining;
    float amplitude;
    float frequency;

    public :

    void setRightAsc(const double value);
    void setDeclin(const double value);
    void setShining(const float value);
    void setAmplitude(const float value);
    void setFrequency(const float value);
    double getRightAsc(void);
    double getDeclin(void);
    float getShining(void);
    float getAmplitude(void);
    float getFrequency(void);
};*/

class WTR2_Stars
{
    private :

    vector<float> right_asc;
    vector<float> declinations;
    vector<float> shinings;
    vector<float> amplitudes;
    vector<float> frequencies;
    GLuint bufferVRAM;
    GLuint VAO;
    GLuint texture;
    GLuint shader;

    public :

    void Init(vector<float> &r,vector<float> &d,vector<float> &s,vector<float> &a,vector<float> &f);
    void Draw(const float xCam,const float yCam,const float zCam,const float opacity,const float var,const float radiusSky,const GLuint shader,glm::mat4 projection,glm::mat4 modelview);
    int getNbStars(void);
};

class WTR2_Sky
{
    private :

    WTR2_Color colorHorizon;
    WTR2_Color colorZenit;
    //GLuint skybox;
    //GLuint texStar;
    //vector<WTR2_Star> stars;
    WTR2_Stars stars;
    unsigned int chrono;
    GLuint shader;
    GLuint shaderStars;
    PRIM_Solid sphere;
    WTR2_Sun sun;

    public :

    //void Init(const float SpeedSun,float hour,int day,int month,double latitude,string texSkybox,WTR2_Color cHorizon,WTR2_Color cZenit,WTR2_Color cSun,WTR2_Color cSunSunset,WTR2_Sun *sun);
    void Init(const float SpeedSun,float hour,int day,int month,double latitude,string pathStars,WTR2_Color cHorizon,WTR2_Color cZenit,WTR2_Color cSun,WTR2_Color cSunSunset,const int wWindow,const int hWindow);
    void InitStars(string pathStars);
    void DrawStars(const float xCam,const float yCam,const float zCam,const float radiusSky,glm::mat4 projection,glm::mat4 modelview);
    void Draw(const float xCam,const float yCam,const float zCam,glm::mat4 projection,glm::mat4 modelview);
    WTR2_Sun *getSun(void);
};

class WTR2_Wind
{
    private :

    float strenght;
    float speed;
    double direction;
    float varSin;
    vector<float> phases;
    GLuint bufferPhases;
    int attributPhase;

    public :

    void Init(const float decalMax,const float zMax,const float spd,const float dir,const int nbElements);
    void start(const GLuint shader);
    void end(const GLuint shader);
    void enablePhase(const GLuint shader);
    void disablePhase(void);
};

void WTR2_MatrixRotate(float *X,float *Y,float *Z,double angle,glm::vec3 axis);
void WTR2_Normalize(WTR2_Vertex *vect);
double WTR2_calculAngle(const double *pos1,const double *pos2);
void WTR2_calculPosSun(float *posSun1,float *posSun2);
