#version 330 core

in vec4 clip_space;

out vec4 out_Color;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;

void main(void) {
	float plane_width = 2; // 127/2 * 3 / 10;
	vec2 ndc = (clip_space.xy/clip_space.w)/plane_width + 0.5;
	vec2 refractTexCoords = ndc;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

	vec4 reflectionColor = texture(reflectionTexture, reflectTexCoords);
	vec4 refractionColor = texture(refractionTexture, refractTexCoords);

	out_Color = mix(reflectionColor, refractionColor, 0.7);

	
}