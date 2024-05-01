#include "DataBuffer.h"
#include <vulkanbase/VulkanBase.h>

DataBuffer::DataBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties):
    m_Usage{usage},
    m_Properties{properties}
    {}

void DataBuffer::BindAsVertexBuffer(VkCommandBuffer commandBuffer) {
    VkBuffer vertexBuffers[] = {m_VkBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
}

void DataBuffer::BindAsIndexBuffer(VkCommandBuffer commandBuffer) {
    vkCmdBindIndexBuffer(commandBuffer, m_VkBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void DataBuffer::Map(VkDeviceSize size, void *data) {
    m_Size = size;
    CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_Properties, m_StagingBuffer, m_StagingBufferMemory);

    void* mappedData;
    vkMapMemory(VulkanBase::device, m_StagingBufferMemory, 0, size, 0, &mappedData);
    memcpy(mappedData, data, (size_t)size);
    vkUnmapMemory(VulkanBase::device, m_StagingBufferMemory);

    CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | m_Usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VkBuffer, m_VkBufferMemory);
    m_HasBeenMapped = true;
}

void DataBuffer::Upload(VkCommandPool const &commandPool, VkQueue const &graphicsQueue) {
    if (!m_HasBeenMapped)
        throw std::runtime_error("DataBuffer::Upload: DataBuffer has not been mapped");

    CopyBuffer(commandPool, graphicsQueue, m_StagingBuffer, m_VkBuffer, m_Size);

    vkDestroyBuffer(VulkanBase::device, m_StagingBuffer, nullptr);
    vkFreeMemory(VulkanBase::device, m_StagingBufferMemory, nullptr);
    m_HasBeenMapped = false;
}

void DataBuffer::Destroy() {
    vkDestroyBuffer(VulkanBase::device, m_VkBuffer, nullptr);
    vkFreeMemory(VulkanBase::device, m_VkBufferMemory, nullptr);

//    vkDestroyBuffer(VulkanBase::device, m_StagingBuffer, nullptr);
//    vkFreeMemory(VulkanBase::device, m_StagingBufferMemory, nullptr);
}

uint32_t DataBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(VulkanBase::physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void DataBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(VulkanBase::device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(VulkanBase::device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(VulkanBase::device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(VulkanBase::device, buffer, bufferMemory, 0); //0 is the offset in memory. If its not 0 it needs to be divisible by memRequirements.alignment
}

void DataBuffer::CopyBuffer(VkCommandPool const &commandPool, VkQueue const &graphicsQueue, VkBuffer srcBuffer,
                            VkBuffer dstBuffer, VkDeviceSize size) {
    CommandBuffer commandBufferClass{ commandPool };

    //TODO: create a separate command pool for short lived objects using the VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag

    commandBufferClass.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBufferClass.GetVkCommandBuffer(), srcBuffer, dstBuffer, 1, &copyRegion);

    commandBufferClass.EndRecording();

    commandBufferClass.Submit();
    vkQueueWaitIdle(graphicsQueue);

    commandBufferClass.FreeCommandBuffer(commandPool);

}




