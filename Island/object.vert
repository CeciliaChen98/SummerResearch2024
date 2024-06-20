#version 330 core

layout(location = 0) in vec3 aPosition;  // Vertex position
layout(location = 1) in vec3 aNormal;    // Vertex normal
layout(location = 2) in vec2 aTexCoord;  // Vertex texture coordinate

out vec3 FragPos;      // Fragment position (for lighting calculations)
out vec3 Normal;       // Normal vector (for lighting calculations)
out vec2 TexCoord;     // Texture coordinate (for texturing)

uniform mat4 model;    // Model matrix
uniform mat4 view;     // View matrix
uniform mat4 projection;// Projection matrix

void main()
{
    // Calculate the vertex position in world space
    FragPos = vec3(model * vec4(aPosition, 1.0));
    
    // Pass the normal vector to the fragment shader
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    // Pass the texture coordinate to the fragment shader
    TexCoord = aTexCoord;
    
    // Calculate the final vertex position in clip space
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
