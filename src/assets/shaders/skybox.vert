
#version 140
in vec3 position;
out vec3 pos;
uniform mat4 view;
uniform mat4 projection;

void main() 
{

	gl_Position =  projection * view * vec4(position, 1.0f);
	pos = position;

	//skybox always looses depth_test to everyone and is always drawn at background
	gl_Position = gl_Position.xyww;


}