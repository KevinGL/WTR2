#version 330 core

layout (location=0) in vec3 vertex;
layout (location=1) in vec2 coordTex;
layout (location=2) in vec3 normale;
layout (location=3) in vec4 color;

out vec3 coordPoint;

uniform mat4 projection;
uniform mat4 modelview;

uniform mat4 transformation;

out vec4 couleurFrag;
 
void main(void)
{
	vec4 vertex2=vec4(vertex,1.0);
	
	vertex2=vertex2*transformation;
	
	gl_Position=projection*modelview*vertex2;
	
	couleurFrag=color;
	
	coordPoint=vertex;
}
