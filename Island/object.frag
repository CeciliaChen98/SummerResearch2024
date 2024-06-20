#version 330 core

in vec3 FragPos;       // Fragment position (from vertex shader)
in vec3 Normal;        // Normal vector (from vertex shader)
in vec2 TexCoord;      // Texture coordinate (from vertex shader)

out vec4 FragColor;    // Output color

struct Material {
    sampler2D diffuse; // Diffuse texture
    sampler2D specular;// Specular texture
    float shininess;   // Shininess coefficient
};

struct Light {
    vec3 position;     // Light position

    vec3 ambient;      // Ambient color
    vec3 diffuse;      // Diffuse color
    vec3 specular;     // Specular color
};

uniform Material material;
uniform Light light;
uniform vec3 camPos;  // Camera position

void main()
{
    // Ambient lighting
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;
    
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;
    
    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;
    
    // Combine results
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
