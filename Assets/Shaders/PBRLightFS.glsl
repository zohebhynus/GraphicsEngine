#version 460 core
in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;
in vec4 fragPosLight;
#define NR_POINT_LIGHTS 10

struct Mat
{
	sampler2D albedo;
	sampler2D normal;
    sampler2D metallic;
	sampler2D roughness;
	sampler2D ao;
    int isMetal;
};

struct ambientLight
{
	vec4 lightColor;
	float colorIntensity;
};

struct directionLight
{
	vec4 direction;
	vec4 lightColor;
	float colorIntensity;
	float specularIntensity;
};

struct pointLight
{
	vec4 position;
	vec4 lightColor;
	vec4 radiusIntensity;
};


uniform ambientLight ambLight;
uniform directionLight dirLight;
uniform pointLight ptLights[NR_POINT_LIGHTS];

uniform sampler2D shadowMap;
uniform samplerCube shadowCubeMap[NR_POINT_LIGHTS];

uniform Mat material;

uniform vec4 viewPos;
uniform float farPlane;



out vec4 FragColor;

//constants
const float PI = 3.14159265359;
const float epsilon = 0.00001;
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);



vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(material.normal, v_TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(v_FragPos);
    vec3 Q2  = dFdy(v_FragPos);
    vec2 st1 = dFdx(v_TexCoord);
    vec2 st2 = dFdy(v_TexCoord);

    vec3 N   = normalize(v_Normal);
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

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
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

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float PointLightShadowCalculation(vec3 fragPos, pointLight light, int  lightnumber)
{
    vec3 fragToLight = fragPos - light.position.xyz;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos.xyz - fragPos);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
	float closestDepth;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowCubeMap[lightnumber], fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= farPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);   

	FragColor = vec4(vec3(closestDepth / farPlane), 1.0);
        
    return shadow;
}

float attenuate(vec3 lightDirection, float radius) {
	float cutoff = 0.5;
	float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius);
	attenuation = 1.0 / (attenuation * 15.0 + 1.0);
	attenuation = (attenuation - cutoff) / (1.0 - cutoff);

	return clamp(attenuation, 0.0, 1.0);
}

vec3 PointLightCalculation(vec3 albedoValue, float metallicValue, float roughnessValue, vec3 reflectance, vec3 viewToFragPos, vec3 normalFromMap, pointLight light, int lightNumber)
{
     // calculate per-light radiance
        vec3 L = normalize(light.position.xyz - v_FragPos.xyz);
        vec3 H = normalize(viewToFragPos + L);
        float dist = length(light.position.xyz - v_FragPos);
        //float attenuation = 1.0 / (1.0 + (light.radius * dist) + (light.edgeIntensity * dist * dist));
        //float attenuation = attenuate(L, light.radius);
        float attenuation = 1.0f - smoothstep(light.radiusIntensity.z, light.radiusIntensity.w, dist);
        vec3 radiance = light.lightColor.xyz * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(normalFromMap, H, roughnessValue);   
        float G   = GeometrySmith(normalFromMap, viewToFragPos, L, roughnessValue);      
        vec3 F    = fresnelSchlick(max(dot(H, viewToFragPos), 0.0), reflectance);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(normalFromMap, viewToFragPos), 0.0) * max(dot(normalFromMap, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallicValue;	  

        // scale light by NdotL
        float NdotL = max(dot(normalFromMap, L), 0.0);   
        
        float shadow = 0.0f;//PointLightShadowCalculation(v_FragPos.xyz, light, lightNumber); 

        // add to outgoing radiance Lo
        vec3 Lo = (kD * albedoValue / PI + specular) * radiance * NdotL * (1.0f - shadow);  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    return Lo;
}


vec3 CalculateDirectionalLight(vec3 albedoValue, float metallicValue, float roughnessValue, vec3 reflectance, vec3 normalFromMap, vec3 viewToFragPos, directionLight light)
{
    vec3 L = normalize(-light.direction.xyz);

    float NdotL = max(0.0f, dot(normalFromMap, L));
    float NdotV = max(0.0f, dot(normalFromMap, viewToFragPos));

    vec3 H = normalize(viewToFragPos + L);
    float NdotH = max(0.0f, dot(normalFromMap, H));

    vec3 F  = FresnelSchlickRoughness(max(0.0f, dot(H, viewToFragPos)), reflectance, roughnessValue);
    float D = DistributionGGX(normalFromMap, H, roughnessValue);
    float G = GeometrySmith(normalFromMap, viewToFragPos, L, roughnessValue);

    vec3 kd = (1.0f - F) * (1.0f - metallicValue);
    vec3 diffuse = kd * albedoValue;

    vec3 nominator = F * G * D;
    float denominator = max(epsilon, 4.0f * NdotV * NdotL);
    vec3 specular = nominator / denominator;
    specular = clamp(specular, vec3(0.0f), vec3(10.0f));

    vec3 result = (diffuse + specular) * (light.colorIntensity * light.lightColor.xyz) * NdotL;

        //shadow calculation
	float shadow = 0.0f;
	vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
	if(lightCoords.z <= 1.0f)
	{
	// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;
		// Prevents shadow acne
		float bias = max(0.025f * (1.0f - dot(normalize(v_Normal), L)), 0.0005f);

		// Smoothens out the shadows
		int sampleRadius = 5;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
			for(int x = -sampleRadius; x <= sampleRadius; x++)
			{
				float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
						shadow += 1.0f;     
			}    
		}
		// Get average shadow
		shadow /= pow((sampleRadius * 2 + 1), 2);
	}


    return result * (1.0f - shadow);
}

void main()
{
    vec3 albedo     = pow(texture(material.albedo, v_TexCoord).rgb, vec3(2.2));

    float metallic = 0.0f;
    if(material.isMetal == 1)
        metallic  = texture(material.metallic, v_TexCoord).r;

    float roughness = texture(material.roughness, v_TexCoord).r;
    float ao        = texture(material.ao, v_TexCoord).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(viewPos.xyz - v_FragPos.xyz);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    //float skip = 255.0f/64.0f;
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        Lo += PointLightCalculation(albedo, metallic, roughness, F0, V, N, ptLights[i], i);
    } 
    
    // Ambient Light Calculation 
    vec3 ambient = ambLight.lightColor.xyz * ambLight.colorIntensity * albedo * ao;
    vec3 dirLi = CalculateDirectionalLight(albedo, metallic, roughness, F0, N, V, dirLight);
    vec3 finalColor = (ambient + Lo + dirLi);

    // HDR tonemapping
    finalColor = finalColor / (finalColor + vec3(1.0));
    // gamma correct
    finalColor = pow(finalColor, vec3(1.0/2.2)); 

    FragColor = vec4(finalColor, 1.0f);
}