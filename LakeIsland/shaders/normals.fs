#version 400
in vec3 normal;
out vec4 FragColor;
void main()
{
   float r = (normal[0]+1)/2.0;
   float g = (normal[1]+1)/2.0;
   float b = (normal[2]+1)/2.0;
   FragColor = vec4(r, g, b, 1.0);
}
