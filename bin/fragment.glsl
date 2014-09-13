#version 330 core

#define screenX 1.0/800
#define screenY 1.0/600

out vec4 color;

uniform sampler2D texUnit;
uniform vec2 mouse;

in vec2 texCoord;

void main()
{
	if(distance(texCoord, mouse) < 0.005){
		color = vec4(1);
		return;
	}

	color = texture(texUnit, texCoord);

	if(color == vec4(0, 0, 0, 1)){
		float top = texture(texUnit, texCoord + vec2(0, screenY)).x;

	}

	float top = texture(texUnit, texCoord + vec2(0, screenY)).x;
	if(top == 1 && color.r == 0 && color.g == 0){
		color = vec4(1, 0, 0.99, 1);
		return;
	}

	vec2 bot = texture(texUnit, texCoord + vec2(0, -screenY)).xz;
	if(bot.x == 1 && bot.y <= 0.99){
		color = vec4(0);
		return;
	}

	if(color.r == 1){
		color = vec4(1);
	}
}
