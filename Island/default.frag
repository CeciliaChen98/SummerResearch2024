#version 330 core

out vec4 FragColor;

uniform vec4 objectColor;

in vec4 fragPosLight;
in vec3 Normal;
in vec3 crntPos;

uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 camPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

vec4 direcLight()
{
	 vec3 color = vec3(objectColor);
	 vec3 lightColor = vec3(1.0f,1.0f,1.0f);

	// ambient lighting
	float ambient = 0.20f;
	vec3 ambientColor = ambient  * color;


	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(crntPos - lightPos );
	float diffuse = max(dot(normal, lightDirection), 0.0f);
	vec3 diffuseColor = diffuse * lightColor;

	// specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 64);
		specular = specAmount * specularLight;
	};
	vec3 specularColor = specular * lightColor ;

	// Shadow value
	float shadow = 0.0f;
	// Sets lightCoords to cull space
	vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
	if(lightCoords.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;
		// Prevents shadow acne
		float bias = max(0.025f * (1.0f - dot(normal, lightDirection)), 0.0005f);

		// Smoothens out the shadows
		int sampleRadius = 2;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;     
		    }    
		}
		// Get average shadow
		shadow /= pow((sampleRadius * 2 + 1), 2);

	}

	//shadow = ShadowCalculation(fragPosLight);

	vec3 lighting = (ambientColor + (1.0 - shadow) * (diffuseColor + specularColor)) * color;   
	return vec4(lighting, 1.0f);
}

void main()
{
	FragColor = direcLight();
}