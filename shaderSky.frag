#version 330 core

void colorSky(void);
vec3 colorNight(vec3 color,vec3 colorHorizon,vec3 colorZenith);

uniform vec3 colorSkyHorizon;
uniform vec3 colorSkyZenit;
uniform vec3 colorSun;
uniform float redSunNormal;
uniform float redSunSunset;
uniform float radiusSky;
uniform vec3 posSun2;
in vec3 coordPoint;

in vec4 couleurFrag;

out vec4 FragColor;

void main(void)
{
	colorSky();
}

void colorSky(void)
{
	vec3 pente,color1,color2;
	
	pente[0]=(colorSkyZenit[0]-colorSkyHorizon[0])/radiusSky;		//Couleur selon hauteur point dans le ciel
	pente[1]=(colorSkyZenit[1]-colorSkyHorizon[1])/radiusSky;
	pente[2]=(colorSkyZenit[2]-colorSkyHorizon[2])/radiusSky;
	
	color1[0]=pente[0]*abs(coordPoint.z)+colorSkyHorizon[0];
	color1[1]=pente[1]*abs(coordPoint.z)+colorSkyHorizon[1];
	color1[2]=pente[2]*abs(coordPoint.z)+colorSkyHorizon[2];
	
	float diametreCiel=2*radiusSky;
	float distanceFromSun=sqrt(pow(coordPoint.x-posSun2.x,2)+pow(coordPoint.y-posSun2.y,2)+pow(coordPoint.z-posSun2.z,2));
	float shiningMin=0.05;
	float shiningMax=0.3;
	
	pente[0]=(shiningMax-shiningMin)/(redSunNormal-redSunSunset);
	float ord=-pente[0]*redSunNormal+shiningMax;
	
	float shining=pente[0]*colorSun[0]+ord;
	
	color1=colorNight(color1,colorSkyHorizon,colorSkyZenit);		//Couleur selon heure de la journée
	
	vec3 colorInterm=(color1+colorSun)/2.0;
	
	pente[0]=(color1[0]-colorInterm[0])/(shining*diametreCiel);
	pente[1]=(color1[1]-colorInterm[1])/(shining*diametreCiel);
	pente[2]=(color1[2]-colorInterm[2])/(shining*diametreCiel);
	
	if(distanceFromSun>=0&&distanceFromSun<=shining*diametreCiel)		//Couleur selon proximité Soleil
	{
		color2[0]=pente[0]*distanceFromSun+colorInterm[0];
		color2[1]=pente[1]*distanceFromSun+colorInterm[1];
		color2[2]=pente[2]*distanceFromSun+colorInterm[2];
	}
	
	else
	{
		color2[0]=color1[0];
		color2[1]=color1[1];
		color2[2]=color1[2];
	}
	
	FragColor[0]=color2[0];
	FragColor[1]=color2[1];
	FragColor[2]=color2[2];
	FragColor[3]=1.0;
}

vec3 colorNight(vec3 color,vec3 colorHorizon,vec3 colorZenith)
{
	float min=1.0;
	float valeurs[6];
	int i=0,j=0;
	
	while(true)
	{
		valeurs[i]=colorHorizon[j];
		
		i++;
		j++;
		
		if(j==3)
			break;
	}
	
	j=0;
	while(true)
	{
		valeurs[i]=colorZenith[j];
		
		i++;
		j++;
		
		if(j==3)
			break;
	}
	
	i=0;
	while(true)
	{
		if(valeurs[i]<min&&valeurs[i]!=0.0)
			min=valeurs[i];
		
		i++;
		if(i==6)
			break;
	}
	
	float decremente=min;
	
	float pente=decremente/(redSunSunset-redSunNormal);
	float ord=-pente*redSunNormal;
	
	if(color[0]!=0)
		color[0]-=pente*colorSun[0]+ord;
	if(color[1]!=0)
		color[1]-=pente*colorSun[0]+ord;
	if(color[2]!=0)
		color[2]-=pente*colorSun[0]+ord;
	
	return color;
}