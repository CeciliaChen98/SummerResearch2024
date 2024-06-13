#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D frameBuffer;

void main()
{
	FragColor = texture(frameBuffer, texCoord);
}