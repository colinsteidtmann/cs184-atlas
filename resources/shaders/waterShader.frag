#version 330 core

in vec2 textureCoords;

out vec4 out_Color;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;

void main(void) {
	if (textureCoords[0] > 1) {
		out_Color = vec4(1, 0, 0, 1);
	} else if (textureCoords[1] > 1) {
		out_Color = vec4(1, 0, 0, 1);
	} else {
		vec4 reflectionColor = texture(reflectionTexture, textureCoords);
		vec4 refractionColor = texture(reflectionTexture, textureCoords);

		out_Color = mix(reflectionColor, refractionColor, 0.5);
		//out_Color = vec4(0,0,1,1);
	}

	
}