#version 450


struct TextureParam{
    bool  enable;
    float uvRotation;
    vec4  uvTransform;   // x,y --> scale, z,w --> translation
};



layout(set=1, binding=0, std140) uniform MaterialUbo{
    vec3 baseColor0;
    vec3 specs;
    TextureParam textureParam0;
    float shininess;
} materialUbo;


layout(set=2, binding=0) uniform sampler2D texture0;


layout (location = 0) in vec2 inUV;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec3 inEyePos;
layout (location = 4) in vec3 inLightVec;

layout (location = 0) out vec4 outFragColor;

void main()
{
    vec3 Color = materialUbo.baseColor0;
    if(materialUbo.textureParam0.enable){
        Color = texture(texture0, inUV).rgb;
    }
    vec3 Eye = normalize(-inEyePos);
    vec3 Reflected = normalize(reflect(-inLightVec, inNormal));

    //vec4 diff = vec4(Color, 1.0) * max(dot(inNormal, inLightVec), 0.0);
    vec4 diff = vec4(Color, 1.0);
    vec4 spec = vec4(specs, 1) * pow(max(dot(Reflected, Eye), 0.0), 2.0) * materialUbo.shininess;

    outFragColor = diff + spec;
    outFragColor.a = 1.0;
}