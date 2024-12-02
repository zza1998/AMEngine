#version 450
#extension GL_GOOGLE_include_directive : enable
#include "00_ad_shader_defines.comp"
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
    vec3 cameraPos;
} frameUbo;

layout(push_constant) uniform PushConstants{
    mat4 modelMat;
    mat3 normalMat; //unused
} PC;

layout (location = 0) out vec3 outWorldPos;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outEyePos;
layout (location = 3) out vec2 outUV;
//-----------------------------------------------g
void main()
{
    mat4 modelView = frameUbo.viewMat * PC.modelMat;
    vec3 locPos = vec3(PC.modelMat * vec4(a_Pos, 1.0));
//    outWorldPos = locPos;
    outWorldPos = vec3(locPos.x,locPos.y,-locPos.z);
    outEyePos = vec3(modelView * vec4(a_Pos,1.0));
    outNormal = normalize(mat3(inverse(transpose(modelView))) * a_Normal);
    gl_Position =  frameUbo.projMat * frameUbo.viewMat * vec4(outWorldPos, 1.0);
    outUV = a_Texcoord;
}
