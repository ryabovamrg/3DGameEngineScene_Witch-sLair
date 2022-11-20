#version 140

in vec3 position;
//in vec3 normal;
in vec2 texcoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 normalModel;

out vec3 fragPosition;
out vec2 fragTextCoord;
//out vec3 fragNormal;
out vec3 cameraSpace_fragPosition;

void main()
{
  gl_Position  = projection * view * model * vec4(position, 1.0 );
  fragPosition = (model * vec4(position, 1.0)).xyz;
  fragTextCoord  = texcoord;
// fragNormal   = (normalModel * vec4(normal, 0.0)).xyz;

  // to calculate distance from camera
  cameraSpace_fragPosition = (view * model * vec4(position, 1.0 )).xyz;
}
