#version 450
#extension GL_GOOGLE_include_directive : enable
#include "00_ad_shader_defines.comp"
#include "00_ad_light.comp"

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput samplerGbufferA;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput samplerGbufferB;
layout (input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput samplerGbufferC;
layout (input_attachment_index = 3, set = 0, binding = 3) uniform subpassInput samplerDepth;

layout(set=1, binding=0, std140) uniform FrameUbo{
    mat4  projMat;
    mat4  viewMat;
    ivec2 resolution;
    uint  frameId;
    float exposure;
    float gamma;
    float time;
} frameUbo;

layout (set = 1,binding = 0) uniform LightUbo {
    PointLight pointLights[LIGHT_MAX_COUNT];
}lightUbo;

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

vec3 calPos(){
    return vec3(1.0,1.0,1.0);
    // 获取深度值 [0, 1]
//    float depth = subpassLoad(samplerDepth).r;
//
//    // 计算片元在 NDC（归一化设备坐标系）中的坐标 (x, y, z) (-1 到 1)
//    vec4 ndcPos = vec4((inUV * 2.0 - 1.0), depth * 2.0 - 1.0, 1.0);
//
//    // 使用逆投影矩阵将 NDC 坐标转换到视空间
//    vec4 viewPos = inverse(projMat) * ndcPos;
//    viewPos /= viewPos.w;  // 透视除法得到视空间坐标
//
//    // 返回视空间坐标的 xyz
//    return viewPos.xyz;
}
void main() {
    vec4 bufferA = subpassLoad(samplerGbufferA);//
    vec4 bufferB = subpassLoad(samplerGbufferB);//
    vec4 bufferC = subpassLoad(samplerGbufferC);//
    vec4 depth = subpassLoad(samplerDepth);//

    vec3 N = bufferA.rgb;
    float metallic = bufferB.a;
    float specular = bufferB.g;
    float roughness = bufferB.b;
    int shadingModel = int(bufferB.a);
    vec3 baseColor = bufferC.rgb;
    vec3 materialcolor = baseColor;


    if (int(bufferB.a) == SHADING_MODEL_UN_LIGHT) {
        // need to update campos
        //vec3 V = normalize(ubo.camPos - inWorldPos);
        //vec3 V = normalize(-inEyePos);
        vec3 camsPos = vec3(0.,0.,0.);
        vec3 worldPos = calPos();
        vec3 V = normalize(camsPos - worldPos);
        // Add striped pattern to roughness based on vertex position
        #ifdef ROUGHNESS_PATTERN
        roughness = max(roughness, step(fract(inWorldPos.y * 2.02), 0.5));
        #endif

        // Specular contribution
        vec3 Lo = vec3(0.0);
        for (int i = 0; i < lightUbo.pointLights.length(); i++) {
            vec3 L = normalize(lightUbo.pointLights[i].position - worldPos);
            Lo += BRDF(L, V, N, metallic, roughness, materialcolor);
        };

        // Combine with ambient
        vec3 color = materialcolor * 0.02;
        color += Lo;

        // Gamma correct
        color = pow(color, vec3(0.4545));

        outFragcolor = vec4(color, 1.0);
    }
    outFragcolor = vec4(materialcolor, 1.0);
}
