#version 450
#extension GL_GOOGLE_include_directive : enable
#include "00_ad_shader_defines.comp"

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inEyePos;
layout (location = 3) in vec2 inUV;

layout(set=1, binding=0, std140) uniform MaterialUbo{
    float roughness;
    float metallic;
    vec3 ambient;
    bool useUV;
} materialUbo;

layout(set=2, binding=0) uniform sampler2D texture0;
/////----------------------------
const float PI = 3.14159265359;

//#define ROUGHNESS_PATTERN 1

vec3 materialcolor()
{
    if(useUV){
        return texture(texture0,inUV);
    }
    return materialUbo.ambient;
}

layout (location = 0) out vec4 outPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outAlbedo;
layout (location = 3) out vec4 outRoughnessMetallicDepth;//{roughness,metallic,depth,0}


// ----------------------------------------------------------------------------
void main()
{
    outAlbedo = vec4(materialcolor(),1.0);
    outNormal = vec4(normalize(inNormal)*0.5+0.5,1.0);
    outPosition = vec4(inWorldPos,1.0);
    outRoughnessMetallicDepth = vec4(materialUbo.roughness,materialUbo.metallic,outPosition.z,1.0);
}