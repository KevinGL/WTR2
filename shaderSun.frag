#version 330 core

uniform sampler2D tex;

in vec2 coordTexFrag;
in vec4 couleurFrag;

out vec4 FragColor;

void main(void)
{
	FragColor=texture(tex,coordTexFrag.xy)*couleurFrag;
}