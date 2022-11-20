#version 140

in vec3 position;
in vec3 nextPosition;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float t;

//out vec3 fragPosition;


void main()
{
	vec3 pos = mix(position, nextPosition, t);
 
	gl_Position  = projection * view * model * vec4(pos, 1.0 );

}
