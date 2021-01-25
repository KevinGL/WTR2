#version 330 core

layout (location=0) in vec3 vertex;
layout (location=1) in vec2 coordTex;

uniform mat4 projection;
uniform mat4 modelview;

uniform mat4 transformation;

uniform vec3 color;

out vec2 coordTexFrag;
out vec4 couleurFrag;
 
void main(void)
{
	vec4 vertex2=vec4(vertex,1.0);
	
	vertex2=vertex2*transformation;				//Transormations géométriques
	
	gl_Position=projection*modelview*vertex2;
	
	coordTexFrag=coordTex;
	
	couleurFrag=vec4(color,1.0);
}
