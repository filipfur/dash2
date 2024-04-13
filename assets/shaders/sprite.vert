#version 330 core

layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aTexCoords;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform vec4 u_sprite_uv;

out vec2 texCoords;

void main()
{
    texCoords = aTexCoords * u_sprite_uv.xy + u_sprite_uv.zw;
    gl_Position = u_projection * u_view * u_model * vec4(aPos, 0.0, 1.0);
}