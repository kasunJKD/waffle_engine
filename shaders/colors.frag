#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
  
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform mat4 invViewMatrix;
uniform vec3 cubeCenter;
uniform float cubeSize;

float sdBox(vec3 p, vec3 b)
{
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y, q.z)),0.0);
}

vec3 rayMarch(vec3 ro, vec3 rd, float start, float end, float step)
{
    float depth = start;
    for (float i = start; i < end; i += step)
    {
	//what is rd 
	vec3 pos = ro + rd * depth;
	float d = sdBox(pos - cubeCenter, vec3(cubeSize));
	if (d < 0.001) return pos;
	depth += d;
    }

    return vec3(0.0);
}

void main()
{   
    vec3 ro = viewPos;
    vec3 rd = normalize((invViewMatrix * vec4(FragPos, 0.0)).xyz);

    vec3 hitPos = rayMarch(ro, rd, 0.1, 100.0, 0.1);

    if (hitPos == vec3(0.0)) {
	FragColor = vec4(0, 0, 0, 1);
	return;
    }

    vec3 eps = vec3(0.001, 0.0, 0.0);

    vec3 n = normalize(vec3(
	sdBox(hitPos + eps.xyy, vec3(cubeSize)) - sdBox(hitPos - eps.xyy, vec3(cubeSize)),
	sdBox(hitPos + eps.yxy, vec3(cubeSize)) - sdBox(hitPos - eps.yxy, vec3(cubeSize)),
	sdBox(hitPos + eps.yyx, vec3(cubeSize)) - sdBox(hitPos - eps.yyx, vec3(cubeSize))
    ));

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
