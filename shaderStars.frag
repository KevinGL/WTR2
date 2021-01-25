#version 330 core
 
uniform sampler2D tex;

in vec2 coordTexFrag;
in float opacite;

out vec4 FragColor;
 
void main(void)
{
	FragColor=texture(tex,coordTexFrag.xy);
	
	FragColor[3]=opacite;
}