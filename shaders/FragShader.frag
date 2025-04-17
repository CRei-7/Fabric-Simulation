// FragShader.frag
#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

in float FurLength;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 Color;
uniform vec3 lightColor;
uniform sampler2D clothTexture;  
uniform int useTexture;  

uniform int isBackFace;

uniform int isFur; 
uniform float furLayer;
uniform float furShadowStrength;
uniform float furAlpha;

// New uniform to identify fabric type
uniform int fabricType;

// Fabric-specific lighting properties
struct FabricLighting {
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    float shininess;
    vec3 reflectanceColor;
};

FabricLighting getFabricLighting(int type) {
    FabricLighting lighting;
    
    switch(type) {
        case 0: //halaMadrid
            lighting.ambientStrength = 0.4;
            lighting.diffuseStrength = 0.7;
            lighting.specularStrength = 0.3;
            lighting.shininess = 32.0;
            lighting.reflectanceColor = vec3(1.0, 1.0, 1.0);
            break;

        case 1: // Linen
            lighting.ambientStrength = 0.2;
            lighting.diffuseStrength = 0.7;
            lighting.specularStrength = 0.3;
            lighting.shininess = 32.0;
            lighting.reflectanceColor = vec3(0.9, 0.9, 0.85); // Soft, natural off-white
            break;
        
        case 2: // Cotton
            lighting.ambientStrength = 0.2;
            lighting.diffuseStrength = 0.6;
            lighting.specularStrength = 0.2;
            lighting.shininess = 16.0;
            lighting.reflectanceColor = vec3(0.95, 0.95, 0.95); // Bright, matte white
            break;
        
        case 3: // Silk
            lighting.ambientStrength = 0.3;
            lighting.diffuseStrength = 0.5;
            lighting.specularStrength = 0.6;
            lighting.shininess = 64.0;
            lighting.reflectanceColor = vec3(1.0, 0.95, 0.9); // Soft, luminous sheen
            break;
        
        case 4: // Wool
            lighting.ambientStrength = 0.25;
            lighting.diffuseStrength = 0.65;
            lighting.specularStrength = 0.25;
            lighting.shininess = 24.0;
            lighting.reflectanceColor = vec3(0.85, 0.82, 0.75); // Warm, muted earth tones
            break;
        
        case 5: // Polyester
            lighting.ambientStrength = 0.2;
            lighting.diffuseStrength = 0.7;
            lighting.specularStrength = 0.4;
            lighting.shininess = 48.0;
            lighting.reflectanceColor = vec3(0.92, 0.92, 1.0); // Cool, slightly blue-tinted
            break;
        
        case 6: // Denim
            lighting.ambientStrength = 0.15;
            lighting.diffuseStrength = 0.8;
            lighting.specularStrength = 0.15;
            lighting.shininess = 16.0;
            lighting.reflectanceColor = vec3(0.6, 0.65, 0.7); // Cool blue-gray
            break;
        
        case 7: // Lycra
            lighting.ambientStrength = 0.3;
            lighting.diffuseStrength = 0.5;
            lighting.specularStrength = 0.5;
            lighting.shininess = 48.0;
            lighting.reflectanceColor = vec3(0.95, 0.95, 1.0); // Bright, slightly glossy
            break;
        
        default:
            lighting.ambientStrength = 0.4;
            lighting.diffuseStrength = 0.7;
            lighting.specularStrength = 0.3;
            lighting.shininess = 32.0;
            lighting.reflectanceColor = vec3(1.0, 1.0, 1.0);
            break;
    }
    
    return lighting;
}

void main()
{
    vec3 norm = normalize(Normal);
    if (isBackFace == 1) {
        norm = -norm;
    }
    
    FabricLighting fabricLighting = getFabricLighting(fabricType);


    // ambient
    vec3 ambient = fabricLighting.ambientStrength * lightColor;
  	
    // diffuse 
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = fabricLighting.diffuseStrength * diff * lightColor;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), fabricLighting.shininess);
    vec3 specular = fabricLighting.specularStrength * spec * lightColor;
    
    vec3 baseColor;
    if (useTexture == 1) {
        baseColor = texture(clothTexture, TexCoord).rgb;
    } else {
        baseColor = Color;
    }
    
    // Apply fabric reflectance color
    baseColor *= fabricLighting.reflectanceColor;

    // Apply fur-specific processing
    float alpha = 1.0;
    
    if (isFur == 1) {
        // For fur, reduce alpha based on fur length to create tapered look
        alpha = 1.0 - FurLength * furAlpha;
        
        // Darken fur towards the tips
        baseColor *= (1.0 - FurLength * 0.2);
    }
    else {
        // For cloth, check if it should receive fur shadows
        // Calculate shadow value based on angle between normal and light direction
        // This simulates shadows cast by fur onto the cloth
        float shadowFactor = max(0.0, 1.0 - max(0.0, dot(norm, lightDir)));
        baseColor *= (1.0 - shadowFactor * furShadowStrength);
    }
    
    vec3 result = (ambient + diffuse + specular) * baseColor;
    // Prevent over-bright areas
    result = min(result, vec3(1.0));
    FragColor = vec4(result, alpha);
}