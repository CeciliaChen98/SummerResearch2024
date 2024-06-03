#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormals;
layout (location = 2) in vec2 vTextureCoords;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform bool HasUV;

out vec3 color;

void main()
{
   vec3 n_eye = normalize(NormalMatrix * vNormals);
   vec4 p_eye = ModelViewMatrix * vec4(vPos, 1.0);  
   vec3 L = normalize(vec3(0,0,1.0)-vec3(p_eye));

   //Ambient color
   vec3 Ia = vec3(0,0,0);

   //Diffuse color
   float diffuse = max(dot(L,n_eye),0.0);
   vec3 materialColor = vec3(0.6, 0.6, 0.9);
   vec3 Id = diffuse*materialColor;

   //Specular color
   vec3 lightColor = vec3(1.0, 1.0, 1.0);
   vec3 R = 2*(L*n_eye)*n_eye - L;
   vec3 v = normalize(vec3(-p_eye));
   float specular = max(dot(R,v),0.0);

   vec3 Is = lightColor*pow(specular,10);
   color = Ia+Id+Is;
   gl_Position = MVP * vec4(vPos, 1.0);
}
