#version 330 core
layout(location = 0) in vec3 aPos; // Position of the vertex
layout(location = 1) in vec3 aNormal; // Normal vector of the vertex

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos; // Position of the fragment (for lighting calculation)
out vec3 Normal; // Normal vector (for lighting calculation)

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal; // Adjust normal to world space

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

// #version 330 core
// layout(location = 0) in vec3 aPos;

// uniform mat4 projection;
// uniform mat4 view;
// uniform mat4 model;

// void main() {
//     gl_Position = projection * view * model * vec4(aPos, 1.0);
//     gl_PointSize = 0.05; // Size of the particle (you can adjust this value)
// }
