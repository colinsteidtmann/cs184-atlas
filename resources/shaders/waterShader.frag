#version 330 core

in vec4 clip_space;
in vec2 textureCoords;
in vec3 to_camera_vector;

out vec4 out_Color;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normal_map;

const float wave_strength = 0.02;
uniform float move_factor;

void main(void) {
	vec2 ndc = (clip_space.xy/clip_space.w)/2.0+ 0.5;
	vec2 refractTexCoords = ndc;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

	vec2 distortion1 = (texture(dudvMap, vec2(textureCoords.x +move_factor, textureCoords.y)).rg * 2.0 - 1.0) * wave_strength;
	vec2 distortion2 = (texture(dudvMap, vec2(-textureCoords.x, textureCoords.y + move_factor)).rg * 2.0 - 1.0) * wave_strength;
	vec2 total_distortion = distortion1 + distortion2;
	
	// vec2 distorted_tex_coords = texture(dudvMap, vec2(textureCoords.x + move_factor, textureCoords.y)).rg*0.1;
	// distorted_tex_coords = textureCoords + vec2(distorted_tex_coords.x, distorted_tex_coords.y + move_factor);
	// vec2 total_distortion = (texture(dudvMap, distorted_tex_coords).rg * 2.0 - 1.0) * wave_strength;
	

	refractTexCoords += total_distortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);
	reflectTexCoords += total_distortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, 0.001);

	vec4 reflectionColor = texture(reflectionTexture, reflectTexCoords);
	vec4 refractionColor = texture(refractionTexture, refractTexCoords);

	vec3 view_vector = normalize(to_camera_vector);
	float refractive_factor = dot(view_vector, vec3(0.0, 1.0, 0.0));
	// refractive_factor = pow(refractive_factor, 5);

	// vec4 normal_map_color = texture(normal_map, distorted_tex_coords);
	// vec3 normal = vec3(normal_map_color.r * 2.0 - 1.0, normal_map_color.b , normal_map_color.g * 2.0 - 1.0);
	// normal = normalize(normal);

	out_Color = mix(reflectionColor, refractionColor, refractive_factor);
	out_Color = mix(out_Color, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
	
}