#version 330 core

layout (location = 0) in vec3 coord;
layout (location = 1) in vec2 textureC;

out vec2 textureCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(coord.x, coord.y, coord.z, 1.0);
    textureCoord = vec2(textureC.x, textureC.y);
}