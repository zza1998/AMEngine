#version 450

layout(location=0)      in vec3 a_Pos;
layout(location=1)      in vec2 a_Texcoord;
layout(location=2)      in vec3 a_Normal;

out gl_PerVertex{
    vec4 gl_Position;
};

layout(set=0, binding=0, std140) uniform GlobalUbo{
    mat4 projMat;
    mat4 viewMat;
} globalUbo;

layout(set=0, binding=1, std140) uniform InstanceUbo{
    mat4 modelMat;
} instanceUbo;

out layout(location=1) vec2 v_Texcoord;

void main(){
    // FIXME projMat[1][1] *= 1.f;
    gl_Position = globalUbo.projMat * globalUbo.viewMat * instanceUbo.modelMat * vec4(a_Pos.x, a_Pos.y, a_Pos.z, 1.f);
    v_Texcoord = a_Texcoord;
}