#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vUV;

out vec3 normal; 

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;

void main()
{
   normal = vNormal;
   gl_Position = MVP * vec4(vPos, 1.0);
}
