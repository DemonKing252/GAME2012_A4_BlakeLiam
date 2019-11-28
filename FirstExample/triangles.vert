#version 430 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_texture;
layout(location = 2) in vec3 vertex_normal;

out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;

// Values that stay constant

out vec3 colour;
out vec2 texCoord;

// Values that stay constant for the whole mesh.

uniform highp mat4 mvp;
uniform highp mat4 V;
uniform highp mat4 M;


void main()
{
	texCoord = vertex_texture;
	gl_Position = mvp * vec4(vertex_position, 1.0);

	Position_worldspace = vec3(M * vec4(vertex_position, 1));

	vec3 vertexPosition_cameraspace = vec3(V * M * vec4(vertex_position, 1));
	EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;

	Normal_cameraspace = vec3(V * M * vec4(vertex_normal, 1));

}