#version 330 core

uniform vec3 posCam;
uniform float radiusSky;
uniform float opacityStars;
uniform float varStars;

layout (location=0) in vec3 vertex;
layout (location=1) in vec2 coordtex;
layout (location=2) in float right_asc_stars;
layout (location=3) in float declination_stars;
layout (location=4) in float shining_stars;
layout (location=5) in float amplitude_stars;
layout (location=6) in float frequency_stars;

out vec2 coordTexFrag;
out float opacite;

uniform mat4 projection;
uniform mat4 modelview;

vec4 drawStars(vec4 vertex);

mat4 rotationMatrix(vec3 axis,float angle);
 
void main(void)
{
	vec4 vertex2=vec4(vertex,1.0);
	
	vertex2=drawStars(vertex2);
	
	gl_Position=projection*modelview*vertex2;
	
	coordTexFrag=coordtex;
	
	const float PI=3.1415926535897932384626433832795;
	float pulsation=2*PI*frequency_stars;
	float brillance=amplitude_stars*sin(pulsation*varStars)+shining_stars;
	opacite=brillance*opacityStars;
}

vec4 drawStars(vec4 vertex)
{	
	vec4 vertex2=vertex;
	
	vertex2.x+=radiusSky;
	
	mat4 rotationH=rotationMatrix(vec3(0.0,0.0,1.0),right_asc_stars);		//Calcul position selon orientation
	mat4 rotationV=rotationMatrix(vec3(0.0,1.0,0.0),declination_stars);
	
	vertex2=rotationV*vertex2;
	vertex2=rotationH*vertex2;
	
	vertex2.x+=posCam.x;
	vertex2.y+=posCam.y;
	vertex2.z+=posCam.z;
	
	return vertex2;
}

mat4 rotationMatrix(vec3 axis,float angle)
{
    axis=normalize(axis);
    float s=sin(angle);
    float c=cos(angle);
    float oc=1.0-c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}