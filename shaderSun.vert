#version 330 core

layout (location=0) in vec3 vertex;
layout (location=1) in vec2 coordTex;

uniform mat4 projection;
uniform mat4 modelview;

uniform mat4 transformation;

uniform vec3 colorSunset;		//Couleur au coucher
uniform vec3 colorNormal;		//Couleur normale
uniform float hauteur;			//Hauteur actuelle
uniform float hauteurMax;		//Hauteur max
uniform float rapportSunset;	//Valeur en-dessous de laquelle la couleur change

out vec2 coordTexFrag;
out vec4 couleurFrag;
 
void main(void)
{
	vec4 vertex2=vec4(vertex,1.0);
	
	vertex2=vertex2*transformation;
	
	gl_Position=projection*modelview*vertex2;
	
	coordTexFrag=coordTex;
	
	vec3 color;
	
	if(hauteur>=rapportSunset*hauteurMax)		//Calcule couleur selon hauteur
	{
		vec3 pente=(colorNormal-colorSunset)/(hauteurMax-rapportSunset*hauteurMax);
		vec3 ord=-pente*hauteurMax+colorNormal;
		color=pente*hauteur+ord;
	}
	else
		color=colorSunset;
	
	couleurFrag=vec4(color,1.0);
}
