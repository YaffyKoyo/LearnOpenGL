#version 420 core
in vec3 Normal;
in vec3 Position;
out vec4 color;

const float EtaR = 0.65;
const float EtaG = 0.67; // Ratio of indices of refraction
const float EtaB = 0.69;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{   
    float ratio = 1.00/1.10;
	vec3 RA,refractColor,RR,RG,RB;
	vec3 I = normalize(Position - cameraPos);
    
    ////////////////////////////////////
    //reflect         
    ////////////////////////////////////
    vec3 R = reflect(I, normalize(Normal));
    color = texture(skybox, R);
    
    //////////////////////////////////
    //refraction
    //////////////////////////////////
    
    RR = refract(I,normalize(Normal),EtaR);
	RG = refract(I,normalize(Normal),EtaG);
	RB = refract(I,normalize(Normal),EtaB);
		
	refractColor.r = vec3(texture(skybox,RR)).r;
	refractColor.g = vec3(texture(skybox,RG)).g;
	refractColor.b = vec3(texture(skybox,RB)).b;
		
	RA = reflect(I, normalize(Normal));
	color = mix(vec4(refractColor,1.0),texture(skybox,RA),0.40);
}