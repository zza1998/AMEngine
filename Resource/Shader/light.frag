#version 450
#extension GL_GOOGLE_include_directive : enable
#include "00_ad_shader_defines.comp"
#include "00_ad_light.comp"

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput samplerGbufferA;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput samplerGbufferB;
layout (input_attachment_index = 2, set = 0, binding = 2) uniform subpassInput samplerGbufferC;
layout (input_attachment_index = 3, set = 0, binding = 3) uniform subpassInput samplerDepth;




layout (set = 1, binding = 0) uniform LightUbo {
    PointLight pointLights[LIGHT_MAX_COUNT];
}lightUbo;

layout(set=2, binding=0, std140) uniform FrameUbo{
    mat4  projMat;
    mat4  viewMat;
    ivec2 resolution;
    uint  frameId;
    float exposure;
    float gamma;
    float time;
    vec3 cameraPos;
} frameUbo;



layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

vec3 calPos(){
    //return vec3(1.0,1.0,1.0);
    // 获取深度值 [0, 1]
    float depth = subpassLoad(samplerDepth).r;

    // 计算片元在 NDC（归一化设备坐标系）中的坐标 (x, y, z) (-1 到 1)
    vec4 ndcPos = vec4((inUV * 2.0 - 1.0), depth * 2.0 - 1.0, 1.0);

    // 使用逆投影矩阵将 NDC 坐标转换到视空间
    vec4 viewPos = inverse(frameUbo.projMat) * ndcPos;
    viewPos /= viewPos.w;// 透视除法得到视空间坐标

    // 返回视空间坐标的 xyz
    return viewPos.xyz;
}

vec3 BRDF(vec3 L, vec3 V, vec3 N, float metallic, float roughness, vec3 materialcolor)
{
    // Precalculate vectors and dot products
    vec3 H = normalize (V + L);
    float dotNV = clamp(dot(N, V), 0.0, 1.0);
    float dotNL = clamp(dot(N, L), 0.0, 1.0);
    float dotLH = clamp(dot(L, H), 0.0, 1.0);
    float dotNH = clamp(dot(N, H), 0.0, 1.0);


    // Light color fixed
    vec3 lightColor = vec3(1.0);

    vec3 color = vec3(0.0);

    if (dotNL > 0.0)
    {
        float rroughness = max(0.05, roughness);
        // D = Normal distribution (Distribution of the microfacets)
        float D = D_GGX(dotNH, roughness);
        // G = Geometric shadowing term (Microfacets shadowing)
        float G = G_SchlicksmithGGX(dotNL, dotNV, rroughness);
        // F = Fresnel factor (Reflectance depending on angle of incidence)
        vec3 F = F_Schlick(dotNV, metallic, materialcolor);
        //        if(D<=0){
        //            return vec3(D,0.0,0.0);
        //        }
        //        if(G<=0){
        //            return vec3(0.0,G,0.0);
        //        }
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;
        vec3 spec = D * F * G / (4.0 * dotNL * dotNV + 0.01f);

        color += (kD * materialcolor / PI +  spec) * dotNL * lightColor;
    }

    return color;
}

void main() {
    vec4 bufferA = subpassLoad(samplerGbufferA);//
    vec4 bufferB = subpassLoad(samplerGbufferB);//
    vec4 bufferC = subpassLoad(samplerGbufferC);//
    vec4 depth = subpassLoad(samplerDepth);//

    vec3 N = normalize(bufferA.rgb);
    float metallic = bufferB.a;
    float specular = bufferB.g;
    float roughness = bufferB.b;
    int shadingModel = int(bufferB.a * 16.0f);
    vec3 baseColor = bufferC.rgb;
    vec3 materialcolor = baseColor;

    vec3 camsPos = frameUbo.cameraPos;
    vec3 worldPos = calPos();
    vec3 V = normalize(camsPos - worldPos);
    if (shadingModel == SHADING_MODEL_PBR_LIGHT) {
        // need to update campos
        //vec3 V = normalize(ubo.camPos - inWorldPos);
        //vec3 V = normalize(-inEyePos);


        // Add striped pattern to roughness based on vertex position
        #ifdef ROUGHNESS_PATTERN
        roughness = max(roughness, step(fract(inWorldPos.y * 2.02), 0.5));
        #endif

        // Specular contribution
        vec3 Lo = vec3(0.0, 0.0, 0.);
        for (int i = 0; i < lightUbo.pointLights.length(); i++) {
            vec3 L = normalize(lightUbo.pointLights[i].position - worldPos);
            Lo += lightUbo.pointLights[i].used * BRDF(L, V, N, metallic, roughness, materialcolor);
        };

        // Combine with ambient
        vec3 color = materialcolor * 0.02;
        color += Lo;

        // Gamma correct
        color = pow(color, vec3(0.4545));

        outFragcolor = vec4(color, 1.0);
        return;
    }else if (shadingModel == SHADING_MODEL_PHONG_LIGHT){

        vec3 Lo = vec3(0.0);
        vec3 diff = baseColor;
        Lo+=diff;
        for (int i = 0; i < lightUbo.pointLights.length(); i++) {
            vec3 L = normalize(lightUbo.pointLights[i].position - worldPos);
            vec3 Reflected = normalize(reflect(-L, N));
            //vec4 diff = vec4(Color, 1.0) * max(dot(inNormal, inLightVec), 0.0);
            vec3 spec = lightUbo.pointLights[i].used * vec3(specular * pow(max(dot(Reflected, V), 0.0), 2.0) * 2.0);
            //Lo += spec;
        };
        outFragcolor = vec4(Lo, 1.0);
        return;
    }
    outFragcolor = vec4(0.2, 0.5, 0.1, 1.0);
}
