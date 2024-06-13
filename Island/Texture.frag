#version 330 core

out vec4 FragColor;

in vec4 clipSpace;
in vec2 texCoord;
in vec3 toCam;

uniform sampler2D frameBuffer;
uniform sampler2D texture_specular1;


void main()
{
	vec2 ndc = (clipSpace.xy/clipSpace.w)/2 + 0.5;
	vec2 reflectCoord = vec2(ndc.x,-ndc.y);
	vec4 lightColor = vec4(0.3f,0.5f,0.8f,1.0f);

	vec3 viewV = normalize(toCam);
	float factor = dot(viewV, vec3(0.0f, 1.0f, 0.0f));

	FragColor = texture(frameBuffer,reflectCoord)*(1-factor) + factor * lightColor;
}