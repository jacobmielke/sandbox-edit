#version 330 core
layout (location = 0) in vec3 aPos;
out float Height;
out vec3 Position;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Height = aPos.y;
    Position = (view * model * vec4(aPos, 1.0)).xyz;
    TexCoord.x = aPos.x / 8.0f;
    TexCoord.y = aPos.z / 8.0f;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}