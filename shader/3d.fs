#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;  

uniform vec3 objectColor;
uniform vec3 lightPos; 
uniform vec3 lightColor;



void main()
{
    vec3 normal = normalize(Normal);

    vec3 lightDir = normalize(lightPos - FragPos);

    float diffuseIntensity = max(dot(normal, lightDir), 0.0);

    vec3 diffuseColor = objectColor * lightColor * diffuseIntensity;

    FragColor = vec4(diffuseColor, 1.0);
} 

