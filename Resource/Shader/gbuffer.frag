#version 450
#extension GL_GOOGLE_include_directive : enable
#include "00_ad_shader_defines.comp"

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inEyePos;
layout (location = 3) in vec2 inUV;

layout(set=1, binding=0, std140) uniform MaterialUbo{
    vec3 ambient;
    float roughness;
    float specular;
    float metallic;
    int shadingModelId;
    bool useUV;
} materialUbo;

//layout(set=2, binding=0) uniform sampler2D texture0;
/////----------------------------
const float PI = 3.14159265359;

//#define ROUGHNESS_PATTERN 1

vec3 materialcolor()
{
//    if(materialUbo.useUV){
//        return texture(texture0,inUV).xyz;
//    }
    return materialUbo.ambient;
}

layout (location = 0) out vec4 gufferA; // world normal perObjectGbufferData
layout (location = 1) out vec4 gufferB; // metallic specular roughness shadingmodelId
layout (location = 2) out vec4 gufferC; // BaseColor AO


// ----------------------------------------------------------------------------
void main()
{
    gufferA.rgb = normalize(inNormal)*0.5+0.5;
    gufferB.r = materialUbo.metallic;
    gufferB.g = materialUbo.specular;
    gufferB.b = materialUbo.roughness;
    gufferB.a = materialUbo.shadingModelId;
    gufferC.rgb = materialcolor();

}