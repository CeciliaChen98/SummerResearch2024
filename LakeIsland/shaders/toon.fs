#version 400
in vec3 color;
out vec4 FragColor;

void main()
{
   vec4 c;
   float intensity = dot(color,color);
   if (intensity > 2.5)
		c = vec4(0.966,0.907,0.957,1.0);	
   else if (intensity > 1.5)
		c = vec4(0.906,0.777,0.867,1.0);
	else if (intensity > 1)
		c = vec4(0.847,0.738,0.769,1.0);
	else if (intensity > 0.5)
		c = vec4(0.738,0.6,0.5,1.0);
	else
		c = vec4(0.41,0.38,0.45,1.0);
   FragColor = c;
}




      
