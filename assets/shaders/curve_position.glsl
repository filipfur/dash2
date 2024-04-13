vec4 curve_position(in mat4 view, in vec4 worldPosition, in float curveFactor)
{
    vec3 preCurvePos = (view * worldPosition).xyz;
    float d = length(preCurvePos.xy);
    float curveAmount = d * d * curveFactor;
    preCurvePos.z += curveAmount;
    return vec4(preCurvePos, 1.0);
}