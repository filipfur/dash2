#version 330 core

layout(location=0) in vec4 aVertex;

out vec2 texCoords;
out vec3 color;
out vec3 position;
out vec3 normal;
out vec3 eyePos;

layout (std140) uniform CameraBlock
{
    mat4 u_projection;
    mat4 u_view;
    vec3 u_eye_pos;
};
uniform mat4 u_model;

void main()
{
    vec3 aPos = vec3(aVertex.xy, 0.0);
    texCoords = aVertex.zw;
    position = vec3(u_model * vec4(aPos, 1.0));
    normal = normalize(mat3(u_model) * vec3(0,0,1));
    eyePos = u_eye_pos;
    color = vec3(1.0);
    gl_Position = u_projection * u_view * vec4(position, 1.0);
}