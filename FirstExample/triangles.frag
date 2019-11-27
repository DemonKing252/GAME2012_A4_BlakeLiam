#version 430 core
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
struct Light
{
	vec3 Color;
	vec3 Position;
	float Strength;
	float falloffStart;
	float falloffEnd;

};
vec3 totalLight;
const int NUM_POINT_LIGHTS = 2;
uniform sampler2D diffuseTexture;
uniform Light pointLights[NUM_POINT_LIGHTS];

uniform highp mat4 V;

in vec3 colour;
in vec2 texCoord;
out vec4 frag_colour;

uniform sampler2D texture0;
vec3 calcPointLights()
{
	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		float distance = length(pointLights[i].Position - Position_worldspace);
		// Normal of the computed fragment
		if (distance <= pointLights[i].falloffEnd)
		{
			vec3 n = normalize(Normal_cameraspace);
			// Direction of the light (from the fragment to the light)
			vec3 LightPosition_cameraspace = ( V * vec4(pointLights[i].Position, 1)).xyz;
			//vec4(pointLights[i].Position,1).xyz;
			vec3 lightDirection = LightPosition_cameraspace + EyeDirection_cameraspace;
			vec3 l = normalize( lightDirection );

			float diff = max(dot(n, l), 0.0f);
			vec3 diffuse = pointLights[i].Color * pointLights[i].Strength * diff * ((pointLights[i].falloffEnd - pointLights[i].falloffStart));
			totalLight += diffuse;

			///!!!! ^^^

			// Calculate specular
			//float specStrength = 10;
			//vec3 E = normalize(EyeDirection_cameraspace);
			//vec3 R = reflect(-l, n);
			//float cosAlpha = max(dot(E, R), 0);
			//vec3 specular = pointLights[i].Color * specStrength * pow(cosAlpha, 32) / (distance*distance);
			//totalLight += specular;
		}
			
	}
	return totalLight;
}

void main()
{

	vec4 ambient = vec4(0.5f, 0.5f, 0.5f,1.0f);
	
	//frag_colour = vec4(colour, 1.0);
	
	//frag_colour = vec4(calcPointLights(),1) * texture(texture0, texCoord);

	frag_colour = vec4(calcPointLights(),1) * texture(texture0, texCoord);
	// use rgb for ambient light


	//frag_colour = ambient * texture(texture0, texCoord);
}