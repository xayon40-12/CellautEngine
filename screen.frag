#version 330 core

uniform sampler2D texture0;

in vec2 texCoord;

out vec4 out_Color;

void main()
{
    out_Color = texture(texture0, texCoord);
}
