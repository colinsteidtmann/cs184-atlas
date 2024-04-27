#version 330 core

in vec3 Color;
in vec3 fragPos;
in float height;

uniform vec3 u_viewPos;

uniform vec3 fogColor = vec3(0.45);
uniform float distance;
uniform float fogExponential = 3.0;

uniform bool isFog;
uniform bool isFogLinear;
uniform bool isFogExponential;
uniform bool isFogExponentialSquared;

out vec4 FragColor;

float calculateFogLinear(float cameraToPixel) {
  float distRatio = cameraToPixel / distance;
  return clamp(distRatio, 0.0, 1.0);
}

float calculateFogExponential(float cameraToPixel) {
  float distRatio = 2.0 * cameraToPixel / distance;
  float fogFactor;
  if (isFogExponentialSquared) {
    fogFactor = exp(-distRatio * fogExponential * distRatio * fogExponential);
  } else {
    fogFactor = exp(-distRatio * fogExponential);
  }
  return clamp(1 - fogFactor, 0.0, 1.0);
}

void main() {
  if (isFogLinear || isFogExponential || isFogExponentialSquared) {
    float cameraToPixel = length(fragPos - u_viewPos); 
    float fogFactor = 0.0;

    if (isFogLinear) {
      fogFactor = calculateFogLinear(cameraToPixel);
    } else if (isFogExponential || isFogExponentialSquared) {
      fogFactor = calculateFogExponential(cameraToPixel);
    }

    vec3 finalColor = mix(Color, fogColor, fogFactor);
  
    FragColor = vec4(finalColor, 1.0);
  } else {
    FragColor = vec4(Color, 1.0);
  }
}