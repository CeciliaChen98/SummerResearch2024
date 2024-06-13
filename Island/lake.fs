
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 texCoord;
uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

vec4 direcLight(vec4 Rcolor)
{
	vec4 lightColor = vec4(1.0f,1.0f,1.0f,1.0f);
	// ambient lighting
	float ambient = 0.2f;

	// diffuse lighting
	vec3 normal = -normalize(Normal);
	vec3 lightDirection = normalize(vec3(-1.0f, -1.0f, -1.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(cameraPos - Position);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	//return (texture(tex0, texCoord) * (diffuse + ambient) + texture(tex1, texCoord).r * specular) * lightColor;
	return texture(texture_specular1, texCoord).r*texture(texture_diffuse1, texCoord) * (ambient) + Rcolor*(diffuse) + lightColor*specular;
}


void main()
{
    // Calculate the view direction from the camera to the fragment position
    vec3 I = Position - cameraPos;

    // Reflect the view direction around the normal to get the reflection direction
    vec3 R = reflect(I, normalize(Normal));
    R.z = -R.z;
    // Sample the skybox texture using the reflection direction
    vec4 reflectedColor = texture(skybox, R);

    // Use the reflected color as the fragment color
    vec4 Rcolor = vec4(reflectedColor.rgb, 1.0);

	FragColor = direcLight(Rcolor);
}