/**
 *	!vertex: p2
 */
#version 330 core

in vec2 position;

void main(void)
{
	gl_Position = vec4(position, 0.0, 1.0);
}
