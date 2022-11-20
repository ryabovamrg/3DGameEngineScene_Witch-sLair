#version 140


// interpolated ----------------------v
in vec3 fragPosition;
in vec2 fragTextCoord;
//in vec3 fragNormal;
in vec3 cameraSpace_fragPosition;
// -----------------------------------^
uniform float frameSpeed;
uniform int framesHorizontally;
uniform int framesVertically;
uniform float deltaTime;
uniform float elapsedTime;
uniform sampler2D texture0;


out vec4 color;


int getFrameNumber(in int framesCount)
{
	float res = mod(elapsedTime * frameSpeed, framesCount);
	return int(res);
}


void main() 
{

    // one frame size
	float dx = 1.0/framesHorizontally;
	float dy = 1.0/framesVertically;

	// start coord in texture 
	float x0 = 0.0;
	float y0 = 1.0 - dy;

	// res
	vec2 uv;

	//curent frame number
	int frameNumber = getFrameNumber(framesHorizontally*framesVertically);
	int x_offset = frameNumber % framesHorizontally; //  number of column of current frame
	int y_offset = frameNumber / framesVertically;   //  number of line of current frame

	float x = (x0 + dx * x_offset) + fragTextCoord.x/framesHorizontally;
	float y = (y0 - dy * y_offset) + fragTextCoord.y/framesVertically;

	uv.x = x;
	uv.y = y;
	color = texture(texture0, uv);

	if (color.r + color.g + color.b  < 0.3) {
		color.a = color.r + color.g + color.b;
	}

}
