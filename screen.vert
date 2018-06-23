#version 330 core

uniform mat4 MVP;

in vec3 in_Vertex;
in vec2 in_TexCoord;

out vec3 normal;
out vec2 texCoord;

void main()
{
    gl_Position = vec4(in_Vertex, 1.0);
    texCoord = in_TexCoord;
}
