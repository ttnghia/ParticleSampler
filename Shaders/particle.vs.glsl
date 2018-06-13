//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2018 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// vertex shader, particle render
#version 410 core

#define COLOR_MODE_UNIFORM_MATERIAL 0
#define COLOR_MODE_RANDOM           1
#define COLOR_MODE_RAMP             2
#define COLOR_MODE_OBJ_INDEX        3

#define UNIT_SPHERE_ISOLATED_PARTICLE

uniform vec3 colorRamp[] = vec3[] (vec3(1.0, 0.0, 0.0),
                                   vec3(1.0, 0.5, 0.0),
                                   vec3(1.0, 1.0, 0.0),
                                   vec3(1.0, 0.0, 1.0),
                                   vec3(0.0, 1.0, 0.0),
                                   vec3(0.0, 1.0, 1.0),
                                   vec3(0.0, 0.0, 1.0));

layout(std140) uniform CameraData
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 invViewMatrix;
    mat4 invProjectionMatrix;
    mat4 shadowMatrix;
    vec4 camPosition;
};

uniform int   u_Dimension;
uniform uint  u_nParticles;
uniform int   u_ColorMode;
uniform float u_vColorMin;
uniform float u_vColorMax;
uniform vec3  u_ColorMinVal;
uniform vec3  u_ColorMaxVal;

uniform vec4  u_ClipPlane;
uniform float u_PointRadius;
uniform float u_PointScale;
uniform int   u_ScreenHeight;
uniform float u_DomainHeight;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
in vec3  v_Position;
in float v_fColor;
in int   v_iColor;

flat out vec3 f_ViewCenter;
flat out vec3 f_Color;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
float rand(vec2 co)
{
    float a  = 12.9898f;
    float b  = 78.233f;
    float c  = 43758.5453f;
    float dt = dot(co.xy, vec2(a, b));
    float sn = mod(dt, 3.14);
    return fract(sin(sn) * c);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
vec3 generateVertexColor()
{
    if(u_ColorMode == COLOR_MODE_RANDOM) {
        return vec3(rand(vec2(gl_VertexID, gl_VertexID)),
                    rand(vec2(gl_VertexID + 1, gl_VertexID)),
                    rand(vec2(gl_VertexID, gl_VertexID + 1)));
    } else if(u_ColorMode == COLOR_MODE_RAMP) {
        float segmentSize = float(u_nParticles) / 6.0f;
        float segment     = floor(float(gl_VertexID) / segmentSize);
        float t           = (float(gl_VertexID) - segmentSize * segment) / segmentSize;
        vec3  startVal    = colorRamp[int(segment)];
        vec3  endVal      = colorRamp[int(segment) + 1];
        return mix(startVal, endVal, t);
    } else if(u_ColorMode == COLOR_MODE_OBJ_INDEX) {
        float t = (float(v_iColor) - u_vColorMin) / (u_vColorMax - u_vColorMin);
        return mix(u_ColorMinVal, u_ColorMaxVal, t);
    } else {
        return vec3(1, 0, 0);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void main()
{
    vec4 eyeCoord = viewMatrix * vec4(v_Position, 1.0);
    vec3 posEye   = vec3(eyeCoord);
    /////////////////////////////////////////////////////////////////
    f_ViewCenter = posEye;
    f_Color      = generateVertexColor();

    gl_PointSize       = (u_Dimension == 3) ? u_PointRadius * u_PointScale / length(posEye) : u_PointRadius * 2.0 * float(u_ScreenHeight) / u_DomainHeight;
    gl_Position        = projectionMatrix * eyeCoord;
    gl_ClipDistance[0] = dot(vec4(v_Position, 1.0), u_ClipPlane);
}
