#version 450

layout(location=0)      in vec3 a_Pos;
layout(location=1)      in vec2 a_Texcoord;
layout(location=2)      in vec3 a_Normal;

out gl_PerVertex{
    vec4 gl_Position;
};

layout(set=0, binding=0, std140) uniform FrameUbo{
    mat4  projMat;
    mat4  viewMat;
    ivec2 resolution;
    uint  frameId;
    float time;
} frameUbo;

layout(push_constant) uniform PushConstants{
    mat4 modelMat;
    mat3 normalMat;
} PC;

out layout(location=1) vec2 v_Texcoord;

void main(){
    gl_Position = frameUbo.projMat * frameUbo.viewMat * PC.modelMat * vec4(a_Pos.x, a_Pos.y, a_Pos.z, 1.f);
    v_Texcoord = a_Texcoord;
}
