#version 330 core

in vec3 position;

out vec2 texCoord;

void main()
{
	texCoord = position.xy;
	texCoord += vec2(1.0, 1.0);
	texCoord /= vec2(2.0, 2.0);
	
	gl_Position = vec4(position, 1.0);
}
