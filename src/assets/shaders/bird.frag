
#version 140

uniform vec3 color;
out vec4 fragmentColor;
uniform sampler2D texture0;

struct Camera{
	vec3 position;
	vec3 direction;
};

uniform Camera camera;

void main() 
{
	vec2 texCoord = gl_FragCoord.xy;
	//fragmentColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	fragmentColor = texture(texture0, vec2(texCoord.x/200, texCoord.y /100) ) /* vec4(color, 1.0f)*/;
}