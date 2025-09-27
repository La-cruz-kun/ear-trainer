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
    sampler2D metallicRoughness; // Combined metallic-roughness texture
    sampler2D normal;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

const float PI = 3.14159265359;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(material.normal, fragTexCoord).xyz * 2.0 - 1.0;
    
    vec3 Q1  = dFdx(fragPosition);
    vec3 Q2  = dFdy(fragPosition);
    vec2 st1 = dFdx(fragTexCoord);
    vec2 st2 = dFdy(fragTexCoord);
    
    vec3 N   = normalize(fragNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    
    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / max(denom, 0.001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / max(denom, 0.001);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{		
    // Sample textures
    vec4 albedoAlpha = texture(material.diffuse, fragTexCoord);
    vec3 albedo = pow(albedoAlpha.rgb, vec3(2.2)); // Convert from sRGB to linear
    
    // For glTF, metallic is in B channel, roughness is in G channel
    vec3 metallicRoughness = texture(material.metallicRoughness, fragTexCoord).rgb;
    float metallic = metallicRoughness.r;
    float roughness = metallicRoughness.g;
    float ao = 1.0; // If no AO texture, use 1.0
    
    vec3 N = getNormalFromMap();
    vec3 V = normalize(viewPos - fragPosition);
    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    
    // Calculate light contribution
    vec3 Lo = vec3(0.0);
    
    // Light direction (FROM fragment TO light)
    vec3 L = normalize(light.position - fragPosition);
    vec3 H = normalize(V + L);
    
    float distance = length(light.position - fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    vec3 radiance = light.diffuse * attenuation;
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = numerator / denominator;
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    
    float NdotL = max(dot(N, L), 0.0);        
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    
    // Ambient lighting
    vec3 ambient = light.ambient * albedo * ao;
    
    float diffuse = max(dot(L, fragPosition), 0);
    Lo = Lo * diffuse;
    vec3 color = ambient + Lo;
    
    // Tone mapping and gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 
    
    FragColor = vec4(color, albedoAlpha.a);
}
