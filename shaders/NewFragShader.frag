#version 330 core
in vec3 FragPos; // Position of the fragment
in vec3 Normal; // Normal vector of the fragment

out vec4 FragColor; // Output color

// Uniforms for light and material properties
uniform vec3 lightPos; // Light position
uniform vec3 viewPos; // Camera position (for specular reflection)
uniform vec3 lightColor; // Light color
uniform vec3 Color; // Color of the object

void main() {
    // Ambient lighting
    float ambientStrength = 0.8;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting (Blinn-Phong)
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir); // Blinn-Phong uses halfway vector
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0); //64 is the shininess factor
    vec3 specular = specularStrength * spec * lightColor;

    // Combine all components
    vec3 result = (ambient + diffuse + specular) * Color;
    FragColor = vec4(result, 1.0);
}

// #version 330 core
// out vec4 FragColor;

// uniform vec3 particleColor;

// void main() {
//     FragColor = vec4(particleColor, 1.0);
// }
