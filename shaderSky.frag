#version 330 core

void colorSky(void);

uniform vec3 colorSkyHorizon;		//Couleur horizon
uniform vec3 colorSkyZenith;		//Couleur zenith
uniform vec3 colorSunSunset;		//Couleur Soleil au coucher
uniform float hauteurSun;			//Hauteur actuelle Soleil
uniform float hauteurSunMax;		//Hauteur max Soleil
uniform float hauteurSunMin;		//Hauteur min Soleil
uniform float rapportSunset;		//Valeur en-dessous de laquelle le ciel devint sombre
in vec3 coordPoint;					//Hauteur fragment ciel

in vec4 couleurFrag;

out vec4 FragColor;

void main(void)
{
	colorSky();
}

void colorSky(void)
{
	vec3 colorNightZenith=vec3(13.0/255,0.0,76.0/255);		//Couleur zenith de nuit
	vec3 colorNightHorizon=vec3(0.0,33.0/255,87.0/255);		//Couleur horizon de nuit
	vec3 colorSunsetHorizon=0.8*colorSunSunset;				//Couleur horizon au coucher du Soleil
	
	vec3 colorZenith;
	vec3 pente,ord;
	
	if(hauteurSun>=rapportSunset*hauteurSunMax)				//Calcul couleur au zenith
	{
		pente=(colorSkyZenith-colorNightZenith)/(hauteurSunMax-rapportSunset*hauteurSunMax);
		ord=-pente*hauteurSunMax+colorSkyZenith;
		colorZenith=pente*hauteurSun+ord;
	}
	else
		colorZenith=colorNightZenith;
	
	vec3 colorHorizon;
	
	if(hauteurSun>=rapportSunset*hauteurSunMax)				//Calcul couleur à l'horizon
	{
		pente=(colorSkyHorizon-colorSunsetHorizon)/(hauteurSunMax-rapportSunset*hauteurSunMax);
		ord=-pente*hauteurSunMax+colorSkyHorizon;
		colorHorizon=pente*hauteurSun+ord;
	}
	else
	{
		pente=(colorSunsetHorizon-colorNightHorizon)/(rapportSunset*hauteurSunMax-hauteurSunMin);
		ord=-pente*(rapportSunset*hauteurSunMax)+colorSunsetHorizon;
		colorHorizon=pente*hauteurSun+ord;
	}
	
	pente=(colorZenith-colorHorizon)/radiusSky;
	FragColor=vec4(pente*abs(coordPoint.z)+colorHorizon,1.0);		//Couleur finale selon plus ou moins proche horizon/zenith
}