
#version 140
in vec3 pos;
out vec4 color;

uniform samplerCube ourTexture;

void main() 
{
	vec3 newPos = vec3 ( pos.x, -pos.y, pos.z);
	color = texture(ourTexture, newPos);

}