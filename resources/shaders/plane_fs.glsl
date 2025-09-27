#version 330 core
out vec4 FragColor;

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuse;
    sampler2D roughness;
    sampler2D normal;
    sampler2D metallic;
    sampler2D ao;
    float shininess;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;
uniform vec2 tile;

const float PI = 3.14159265359;


void main()
{		
    // Sample textures
    vec2 uv = fragTexCoord * tile;
    vec3 albedo = texture(material.diffuse, uv).rgb;
    float ao = texture(material.ao, uv).r;
    float specular = texture(material.metallic, uv).r;
    vec3 lightDir = normalize(light.position - fragPosition);
    float diffuse = max(dot(lightDir, fragPosition) + 0.25, 0) ;

    vec3 ambient = albedo * light.ambient;
    albedo = albedo * diffuse * ao * specular;
    
    FragColor =  vec4(albedo + ambient, 1);
}
//a.b = cos0
