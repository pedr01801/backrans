#version 330 core

in vec2 textureCoord;
out vec4 FragColor;

uniform sampler2D text;

void main()
{
    FragColor = texture(text, textureCoord);
}