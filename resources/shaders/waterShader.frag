#version 330 core

in vec4 clip_space;
in vec2 textureCoords;

out vec4 out_Color;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;

const float wave_strength = 0.015;
uniform float move_factor;

void main(void) {
	vec2 ndc = (clip_space.xy/clip_space.w)/2.0+ 0.5;
	vec2 refractTexCoords = ndc;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

	vec2 distortion1 = (texture(dudvMap, vec2(textureCoords.x +move_factor, textureCoords.y)).rg * 2.0 - 1.0) * wave_strength;
	vec2 distortion2 = (texture(dudvMap, vec2(-textureCoords.x, textureCoords.y + move_factor)).rg * 2.0 - 1.0) * wave_strength;
	vec2 total_distortion = distortion1 + distortion2;
	refractTexCoords += total_distortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);
	reflectTexCoords += total_distortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, 0.001);

	vec4 reflectionColor = texture(reflectionTexture, reflectTexCoords);
	vec4 refractionColor = texture(refractionTexture, refractTexCoords);

	out_Color = mix(reflectionColor, refractionColor, 0.5);
	out_Color = mix(out_Color, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
	
}