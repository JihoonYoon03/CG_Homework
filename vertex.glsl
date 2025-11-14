#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;

uniform mat4 projection = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 world = mat4(1.0);
uniform mat4 model = mat4(1.0);

out vec3 FragPos;
out vec3 Normal;
out vec3 color;

void main()
{
	gl_Position = projection * view * world * model * vec4(position, 1.0);
	FragPos = vec3(world * model * vec4(position, 1.0));
	Normal = normalize(mat3(world * model) * vNormal);
	color = vColor;
} 