#version 330 core

layout (location=0) in vec2 vertex;
layout (location=1) in vec4 color;

out vec4 couleurFrag;
 
void main(void)
{
	vec4 vertex2=vec4(vertex,0.0,1.0);
	
	gl_Position=vec4(vertex.x,vertex.y,0.0,1.0);		//Rendu 2D
	
	couleurFrag=color;
}
