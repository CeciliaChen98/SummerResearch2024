#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// Outputs the fragment position of the light
out vec4 fragPosLight;
out vec3 Normal;
out vec3 crntPos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
//uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 lightProjection;
uniform vec4 plane;

void main()
{
	vec4 worldPosition = model * rotation * scale * vec4(aPos, 1.0f);
	gl_ClipDistance[0] = dot(worldPosition,plane);
	crntPos = vec3(worldPosition);
	Normal = aNormal;
	fragPosLight = lightProjection *  vec4(crntPos, 1.0f);
	gl_Position = projection * view * worldPosition;
}