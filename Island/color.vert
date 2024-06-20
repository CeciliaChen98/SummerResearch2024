#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
//uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

uniform vec4 plane;

void main()
{
	vec4 worldPosition = model * rotation * scale * vec4(aPos, 1.0f);
	gl_ClipDistance[0] = dot(worldPosition,plane);
	gl_Position = projection * view * worldPosition;
}