#version 450
#extension GL_GOOGLE_include_directive : enable
#include "00_ad_shader_defines.comp"
#include "00_ad_light.comp"
layout (location = 0)      in vec3 a_Pos;
layout (location = 1)      in vec2 a_Texcoord;
layout (location = 2)      in vec3 a_Normal;
layout (location = 3)      in vec3 inColor;



layout(set=0, binding=0, std140) uniform FrameUbo{
    mat4  projMat;
    mat4  viewMat;
    ivec2 resolution;
    uint  frameId;
    float exposure;
    float gamma;
    float time;
} frameUbo;

layout(set=3,binding =0, std140) uniform LightUbo{
    PointLight light[LIGHT_MAX_COUNT];
}lightUbo;

layout(push_constant) uniform PushConstants{
    mat4 modelMat;
    mat3 normalMat; //unused
} PC;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outColor;
layout (location = 3) out vec3 outEyePos;
layout (location = 4) out vec3 outLightVec;


void main()
{
    mat4 modelView = frameUbo.viewMat * PC.modelMat;
    vec4 pos = modelView * vec4(a_Pos,1.0);
    outUV = a_Texcoord.st;
    outNormal = normalize(mat3(inverse(transpose(modelView))) * a_Normal);
    outColor = inColor;
    gl_Position = frameUbo.projMat * pos;
    outEyePos = vec3(modelView * pos);
    vec4 lightPos = vec4(lightUbo.light[0].position, 1.0);
    outLightVec = normalize(lightPos.xyz - outEyePos);
}
