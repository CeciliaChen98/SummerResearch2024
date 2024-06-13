#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aText;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform vec3 camPos;

out vec4 clipSpace;
out vec2 texCoord;
out vec3 toCam;

void main()
{
	vec4 worldPos =  model *  vec4(aPos, 1.0f);
	gl_Position = projection * view * worldPos;
	clipSpace = gl_Position;
	texCoord = aText;
	toCam = camPos - worldPos.xyz;
}