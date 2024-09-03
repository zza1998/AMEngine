#include "AdGeometryUtil.h"

namespace ade{
    void AdGeometryUtil::CreateCube(float leftPlane, float rightPlane, float bottomPlane, float topPlane, float nearPlane, float farPlane,
                                    std::vector<AdVertex> &vertices, std::vector<uint32_t> &indices, const bool bUseTextcoords,
                                    const bool bUseNormals, const glm::mat4 &relativeMat){
        glm::mat4 normalMat = glm::transpose(glm::inverse(relativeMat));
        //    v6----- v5
        //   /|      /|
        //  v1------v0|
        //  | |     | |
        //  | |v7---|-|v4
        //  |/      |/
        //  v2------v3
        vertices = {
                // v0-v1-v2-v3 front
                { relativeMat * glm::vec4(rightPlane, topPlane, nearPlane, 1.f) },
                { relativeMat * glm::vec4(leftPlane, topPlane, nearPlane, 1.f) },
                { relativeMat * glm::vec4(leftPlane, bottomPlane, nearPlane, 1.f) },
                { relativeMat * glm::vec4(rightPlane, bottomPlane, nearPlane, 1.f) },
                // v0-v3-v4-v5 right
                { relativeMat * glm::vec4(rightPlane, topPlane, nearPlane, 1.f) },
                { relativeMat * glm::vec4(rightPlane, bottomPlane, nearPlane, 1.f) },
                { relativeMat * glm::vec4(rightPlane, bottomPlane, farPlane, 1.f) },
                { relativeMat * glm::vec4(rightPlane, topPlane, farPlane, 1.f) },
                // v0-v5-v6-v1 up
                { relativeMat * glm::vec4(rightPlane, topPlane, nearPlane, 1.f) },
                { relativeMat * glm::vec4(rightPlane, topPlane, farPlane, 1.f) },
                { relativeMat * glm::vec4(leftPlane, topPlane, farPlane, 1.f) },
                { relativeMat * glm::vec4(leftPlane, topPlane, nearPlane, 1.f) },
                // v1-v6-v7-v2 left
                { relativeMat * glm::vec4(leftPlane, topPlane, nearPlane, 1.f) },
                { relativeMat * glm::vec4(leftPlane, topPlane, farPlane, 1.f) },
                { relativeMat * glm::vec4(leftPlane, bottomPlane, farPlane, 1.f) },
                { relativeMat * glm::vec4(leftPlane, bottomPlane, nearPlane, 1.f) },
                // v7-v4-v3-v2 bottom
                { relativeMat * glm::vec4(leftPlane, bottomPlane, farPlane, 1.f) },
                { relativeMat * glm::vec4(rightPlane, bottomPlane, farPlane, 1.f) },
                { relativeMat * glm::vec4(rightPlane, bottomPlane, nearPlane, 1.f) },
                { relativeMat * glm::vec4(leftPlane, bottomPlane, nearPlane, 1.f) },
                // v4-v7-v6-v5 back
                { relativeMat * glm::vec4(rightPlane, bottomPlane, farPlane, 1.f) },
                { relativeMat * glm::vec4(leftPlane, bottomPlane, farPlane, 1.f) },
                { relativeMat * glm::vec4(leftPlane, topPlane, farPlane, 1.f) },
                { relativeMat * glm::vec4(rightPlane, topPlane, farPlane, 1.f) }
        };

        if(bUseTextcoords){
            vertices[0].texcoord0 = glm::vec2(0.0f, 0.0f);
            vertices[1].texcoord0 = glm::vec2(1.0f, 0.0f);
            vertices[2].texcoord0 = glm::vec2(1.0f, 1.0f);
            vertices[3].texcoord0 = glm::vec2(0.0f, 1.0f);

            vertices[4].texcoord0 = glm::vec2(1.0f, 0.0f);
            vertices[5].texcoord0 = glm::vec2(1.0f, 1.0f);
            vertices[6].texcoord0 = glm::vec2(0.0f, 1.0f);
            vertices[7].texcoord0 = glm::vec2(0.0f, 0.0f);

            vertices[8].texcoord0 = glm::vec2(1.0f, 0.0f);
            vertices[9].texcoord0 = glm::vec2(1.0f, 1.0f);
            vertices[10].texcoord0 = glm::vec2(0.0f, 1.0f);
            vertices[11].texcoord0 = glm::vec2(0.0f, 0.0f);

            vertices[12].texcoord0 = glm::vec2(0.0f, 0.0f);
            vertices[13].texcoord0 = glm::vec2(1.0f, 0.0f);
            vertices[14].texcoord0 = glm::vec2(1.0f, 1.0f);
            vertices[15].texcoord0 = glm::vec2(0.0f, 1.0f);

            vertices[16].texcoord0 = glm::vec2(0.0f, 0.0f);
            vertices[17].texcoord0 = glm::vec2(1.0f, 0.0f);
            vertices[18].texcoord0 = glm::vec2(1.0f, 1.0f);
            vertices[19].texcoord0 = glm::vec2(0.0f, 1.0f);

            vertices[20].texcoord0 = glm::vec2(1.0f, 1.0f);
            vertices[21].texcoord0 = glm::vec2(0.0f, 1.0f);
            vertices[22].texcoord0 = glm::vec2(0.0f, 0.0f);
            vertices[23].texcoord0 = glm::vec2(1.0f, 0.0f);
        }

        if(bUseNormals){
            vertices[0].normal = normalMat * glm::vec4(0, 0, 1, 1);
            vertices[1].normal = normalMat * glm::vec4(0, 0, 1, 1);
            vertices[2].normal = normalMat * glm::vec4(0, 0, 1, 1);
            vertices[3].normal = normalMat * glm::vec4(0, 0, 1, 1);
            vertices[4].normal = normalMat * glm::vec4(1, 0, 0, 1);
            vertices[5].normal = normalMat * glm::vec4(1, 0, 0, 1);
            vertices[6].normal = normalMat * glm::vec4(1, 0, 0, 1);
            vertices[7].normal = normalMat * glm::vec4(1, 0, 0, 1);
            vertices[8].normal = normalMat * glm::vec4(0, 1, 0, 1);
            vertices[9].normal = normalMat * glm::vec4(0, 1, 0, 1);
            vertices[10].normal = normalMat * glm::vec4(0, 1, 0, 1);
            vertices[11].normal = normalMat * glm::vec4(0, 1, 0, 1);
            vertices[12].normal = normalMat * glm::vec4(-1, 0, 0, 1);
            vertices[13].normal = normalMat * glm::vec4(-1, 0, 0, 1);
            vertices[14].normal = normalMat * glm::vec4(-1, 0, 0, 1);
            vertices[15].normal = normalMat * glm::vec4(-1, 0, 0, 1);
            vertices[16].normal = normalMat * glm::vec4(0, -1, 0, 1);
            vertices[17].normal = normalMat * glm::vec4(0, -1, 0, 1);
            vertices[18].normal = normalMat * glm::vec4(0, -1, 0, 1);
            vertices[19].normal = normalMat * glm::vec4(0, -1, 0, 1);
            vertices[20].normal = normalMat * glm::vec4(0, 0, -1, 1);
            vertices[21].normal = normalMat * glm::vec4(0, 0, -1, 1);
            vertices[22].normal = normalMat * glm::vec4(0, 0, -1, 1);
            vertices[23].normal = normalMat * glm::vec4(0, 0, -1, 1);
        }

        indices = {
                0, 1, 2, 0, 2, 3,
                4, 5, 6, 4, 6, 7,
                8, 9, 10, 8, 10, 11,
                12, 13, 14, 12, 14, 15,
                16, 17, 18, 16, 18, 19,
                20, 21, 22, 20, 22, 23
        };
    }
}