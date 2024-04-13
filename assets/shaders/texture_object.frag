#version 330 core

precision highp float;

in vec2 texCoords;
in vec3 color;
in vec3 position;
in vec3 normal;
in vec3 eyePos;
out vec4 fragColor;

uniform sampler2D u_texture;
uniform vec4 u_base_color;
uniform float u_n_light_vfx;

const vec3 lightPos = vec3(0.0, 6.0, -2.0);
//const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const vec3 lightColor = vec3(1.0, 0.86, 0.78);

const float k_a = 0.14;
const float k_d = 0.3;
const float k_s = 0.4;
const float alpha = 128.0;

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 diffusePass(in vec3 N, in vec3 L)
{
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = k_d * diff * mix(lightColor, normal, u_n_light_vfx);
    return diffuse;
}

void main()
{
    vec4 texDiffuse = texture(u_texture, texCoords);

    if(texDiffuse.a < 0.1)
    {
        discard;
    }
    vec4 baseColor = /*texDiffuse * */ vec4(pow(u_base_color.rgb, vec3(2.2)), 1.0);

    vec3 N = normalize(normal);
    //vec3 L = normalize(lightPos - position);
    vec3 L = normalize(vec3(1.0, 1.0, -1.0));
    vec3 L2 = normalize(vec3(-1.0, 1.0, -1.0));
    vec3 V = (eyePos.xyz - position);
    vec3 H = normalize(L + V);

    vec3 ambient = k_a * lightColor;

    vec3 diffuse = diffusePass(N, L) + diffusePass(N, L2);

    vec3 F0 = vec3(0.04);
    float cosTheta = dot(N, V);
    vec3 F = fresnelSchlick(cosTheta, F0);
    float spec = pow(max(dot(N, H), 0.0), alpha);
    vec3 specular = k_s * spec * lightColor * max(F, F0);

    vec3 result = (ambient + diffuse) * baseColor.rgb + specular;

    fragColor = vec4(result, baseColor.a);
    //fragColor.rgb = baseColor.rgb;
}