#version 450

layout (location = 0)      in vec3 inPos;
layout (location = 1)      in vec2 inUV;
layout (location = 2)      in vec3 inNormal;

layout(set=0, binding=0, std140) uniform FrameUbo{
    mat4  projMat;
    mat4  viewMat;
    ivec2 resolution;
    uint  frameId;
    float exposure;
    float gamma;
    float time;
} frameUbo;

layout(push_constant) uniform PushConstants{
    mat4 modelMat;
    mat3 normalMat; //unused
} PC;

layout (location = 0) out vec3 outUVW;
out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    outUVW = inPos;
    gl_Position = frameUbo.projMat * PC.modelMat * vec4(inPos.xyz, 1.0);
    gl_Position.z = (gl_Position.z + 0.3) * 0.5;
}
