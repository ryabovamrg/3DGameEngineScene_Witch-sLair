#version 140

const vec4 FOG_COLOR = vec4(0.03125f, 0.046875f, 0.04296875f, 1.0f); 

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 specularExponent;
    vec3 textureScale;
    float shininess;
}; 


struct Light
{
	vec3 direction;
    vec3 position; 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float cutOff;
	float spotExponent;

    float constant;
    float linear;
    float quadratic;
};

const int maxLightsCount = 10;
uniform Light[maxLightsCount] lights;
uniform int lightsCount;

struct Camera 
{
    vec3 position;
    vec3 direction;
};

// interpolated ----------------------v
in vec3 fragPosition;
in vec2 fragTextCoord;
in vec3 fragNormal;
in vec3 cameraSpace_fragPosition;
// -----------------------------------^

uniform float deltaTime;
uniform float elapsedTime;
uniform sampler2D texture0;

uniform Material material;
uniform Camera   camera;

out vec4 color;

// lightining =============================================================================v
// DIRECTIONAL
vec4 calculateDirectionalLight(Light light){

	vec3 L = normalize(-light.direction);
    
    vec3 N = normalize(fragNormal );
    vec3 R = normalize(reflect(L, N));
    vec3 V = normalize(camera.position - fragPosition);
	vec3 S = normalize( (V+L) );
	vec3 ambient = material.ambient * light.ambient;

    float cosAlpha = dot (N, L);
    vec3  diffuse = max(cosAlpha, 0.0f) * material.diffuse * light.diffuse;
	
//  float cosBeta =  dot (V,R);
    float cosBeta =  dot (N,S);
    float shininess = 1.0 / material.shininess;
    vec3  specular = pow(max(cosBeta, 0.0f), 100) * light.specular * material.specular;
	//   vec3 specular = vec3(0.0f);
	return vec4(ambient + diffuse + specular, 0.0f);
}
//--------------------------------------------------------
// POINT LIGHT

vec4 calculatePointLight(Light light)
{

	vec3 L = normalize(light.position - fragPosition);
    vec3 N = normalize(fragNormal);
    vec3 R = normalize(reflect(L, fragNormal));
    vec3 V = normalize(camera.position - fragPosition);
	vec3 S = normalize( (V+L) );
	vec3 ambient = material.ambient * light.ambient;

    float cosAlpha = dot (N, L);
    vec3  diffuse = max(cosAlpha, 0.0f) * material.diffuse * light.diffuse ;
	//	diffuse += vec3(abs(sin(elapsedTime))/2.0, 0.0, 0.0);
	float cosBeta =  dot (N,S);
    float shininess = 1.0 / material.shininess;
    vec3  specular = pow(max(cosBeta, 0.0f), shininess) * light.specular * material.specular;

	float distance    = length(light.position - fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation; 

	return vec4(ambient + diffuse + specular, 0.0f);
}

//--------------------------------------------------------
// SPOTLIGHT

vec4 calculateSpotlight(Light light){

	vec3 ret = vec3(0.0);

	vec3 L = normalize(light.position - fragPosition);
	vec3 N = normalize(fragNormal);
	vec3 R = normalize(reflect(L, fragNormal));
	vec3 V = normalize(camera.position - fragPosition);
	vec3 S = normalize( (V+L) );
	vec3 ambient = material.ambient * light.ambient;

	float cosAlpha = dot (N, L);
	vec3  diffuse  = max(cosAlpha, 0.0f) * material.diffuse * light.diffuse ;

	float cosBeta =  dot (N,S);
	
	float shininess = 1.0 / material.shininess;
	vec3  specular  = pow(max(cosBeta, 0.0f), shininess) * light.specular * material.specular ;

	ret = ambient + diffuse;

    
	if(dot(normalize(light.direction), -L) >= light.cutOff) 
	{
		ret *= pow(max(dot(normalize(light.direction), -L), 0.0), light.spotExponent);
	}
	else
	{
		ret *= 0.0;
	}

	ret *= (1.0+sin(elapsedTime))/2.0;

	return vec4(ret, 1.0);
}




// ========================================================================================^

vec4 texture_component (in sampler2D _texture){
    vec2 textCoord = vec2(fragTextCoord.x / material.textureScale.x, fragTextCoord.y / material.textureScale.y);
    return texture(_texture, textCoord);
}

// fog ====================================================================================v
float fogDistanceFactor(){
    
    float start = 20;
    float end   = 80;
    float dist  = abs(cameraSpace_fragPosition.z);
    
    float f = (end - dist)/(end - start);
    f = clamp( f, 0.0, 1.0 );
    
    return f;
}
// ========================================================================================^



void main() 
{

    vec4 _texture = texture_component(texture0);

	vec4 dirLightColor   = calculateDirectionalLight(lights[0]);
	vec4 spotLightColor  = calculateSpotlight(lights[1]);
	vec4 pointLightColor = vec4(0.0);
    
	for(int i = 2 ; i < lightsCount ; i++){
		pointLightColor += calculatePointLight(lights[i]);
	}

	color =  _texture * (dirLightColor + spotLightColor + pointLightColor);

	
	
	color = mix(FOG_COLOR, color, fogDistanceFactor());

	
}
