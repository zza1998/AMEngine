#include "Render/AdMesh.h"
#include "Render/AdRenderContext.h"
#include "AdApplication.h"

namespace ade{
    AdMesh::AdMesh(const std::vector<ade::AdVertex> &vertices, const std::vector<uint32_t> &indices) {
        if(vertices.empty()){
            return;
        }
        ade::AdRenderContext *renderCxt = AdApplication::GetAppContext()->renderCxt;
        ade::AdVKDevice *device = renderCxt->GetDevice();

        mVertexCount = vertices.size();
        mIndexCount = indices.size();
        mVertexBuffer = std::make_shared<ade::AdVKBuffer>(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, sizeof(vertices[0]) * vertices.size(), (void*)vertices.data());
        if(mIndexCount > 0){
            mIndexBuffer = std::make_shared<ade::AdVKBuffer>(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, sizeof(indices[0]) * indices.size(), (void*)indices.data());
        }
    }

    AdMesh::~AdMesh() {

    }

    void AdMesh::Draw(VkCommandBuffer cmdBuffer) {
        VkBuffer vertexBuffers[] = { mVertexBuffer->GetHandle() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

        if(mIndexCount > 0){
            vkCmdBindIndexBuffer(cmdBuffer, mIndexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(cmdBuffer, mIndexCount, 1, 0, 0, 0);
        } else {
            vkCmdDraw(cmdBuffer, mVertexCount, 1, 0, 0);
        }
    }
}