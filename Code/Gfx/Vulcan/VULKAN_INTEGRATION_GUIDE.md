# Vulkan Backend Integration Guide

## Quick Start

This guide explains how to complete the Vulkan graphics backend implementation for THUG.

## What's Already Implemented

✅ **Core Infrastructure**
- Vulkan instance/device creation
- Physical device selection
- Queue family detection
- Command pool management
- Proper resource cleanup

✅ **Data Structures**
- VulkanState with all necessary handles
- Buffer placeholders in sMesh
- Texture image placeholders in sTexture
- Scene rendering framework

✅ **Resource Management**
- Texture format mapping (DXT, RGBA, etc.)
- Buffer creation stubs
- Texture creation stubs
- Mesh/texture lifecycle management

✅ **Build System**
- CMake option for Vulkan renderer
- Conditional compilation (works with or without SDK)
- Stub fallback for non-Vulkan builds

## What Needs to Be Implemented

To get actual GPU rendering working, you need to implement:

### 1. Swapchain Creation (Required for Rendering)

**Where**: Add to `init_vulkan()` in `render.cpp`

**What to Add**:
```cpp
// After creating device
VkSwapchainCreateInfoKHR swapchain_info = {};
swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
swapchain_info.surface = surface; // Need window surface first
swapchain_info.minImageCount = 2; // Double buffering
swapchain_info.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
swapchain_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
swapchain_info.imageExtent = extent; // Window size
swapchain_info.imageArrayLayers = 1;
swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
swapchain_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;

vkCreateSwapchainKHR(device, &swapchain_info, NULL, &swapchain);

// Get swapchain images
vkGetSwapchainImagesKHR(device, swapchain, &image_count, NULL);
VkImage* swapchain_images = new VkImage[image_count];
vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images);

// Create image views for each swapchain image
for (uint32_t i = 0; i < image_count; i++) {
    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = swapchain_images[i];
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = VK_FORMAT_B8G8R8A8_UNORM;
    view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    
    vkCreateImageView(device, &view_info, NULL, &image_views[i]);
}
```

**Prerequisites**: Need a window surface (VkSurfaceKHR) from platform layer

### 2. Render Pass Creation

**Where**: Add to `init_vulkan()` after swapchain

**What to Add**:
```cpp
VkAttachmentDescription color_attachment = {};
color_attachment.format = swapchain_format;
color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

VkAttachmentReference color_attachment_ref = {};
color_attachment_ref.attachment = 0;
color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

VkSubpassDescription subpass = {};
subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
subpass.colorAttachmentCount = 1;
subpass.pColorAttachments = &color_attachment_ref;

VkSubpassDependency dependency = {};
dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
dependency.dstSubpass = 0;
dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
dependency.srcAccessMask = 0;
dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

VkRenderPassCreateInfo render_pass_info = {};
render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
render_pass_info.attachmentCount = 1;
render_pass_info.pAttachments = &color_attachment;
render_pass_info.subpassCount = 1;
render_pass_info.pSubpasses = &subpass;
render_pass_info.dependencyCount = 1;
render_pass_info.pDependencies = &dependency;

vkCreateRenderPass(device, &render_pass_info, NULL, &g_vulkan_state.render_pass);
```

### 3. Graphics Pipeline Creation

**Where**: New function `create_graphics_pipeline()` called from `init_vulkan()`

**What to Add**:
```cpp
// Load shaders (need to create these first)
VkShaderModule vert_shader = create_shader_module("shaders/vert.spv");
VkShaderModule frag_shader = create_shader_module("shaders/frag.spv");

VkPipelineShaderStageCreateInfo vert_stage_info = {};
vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
vert_stage_info.module = vert_shader;
vert_stage_info.pName = "main";

VkPipelineShaderStageCreateInfo frag_stage_info = {};
frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
frag_stage_info.module = frag_shader;
frag_stage_info.pName = "main";

VkPipelineShaderStageCreateInfo shader_stages[] = {vert_stage_info, frag_stage_info};

// Vertex input (positions, normals, UVs, colors)
VkVertexInputBindingDescription binding_desc = {};
binding_desc.binding = 0;
binding_desc.stride = sizeof(float) * 3; // Just positions for now
binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

VkVertexInputAttributeDescription attribute_desc = {};
attribute_desc.binding = 0;
attribute_desc.location = 0;
attribute_desc.format = VK_FORMAT_R32G32B32_SFLOAT;
attribute_desc.offset = 0;

VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
vertex_input_info.vertexBindingDescriptionCount = 1;
vertex_input_info.pVertexBindingDescriptions = &binding_desc;
vertex_input_info.vertexAttributeDescriptionCount = 1;
vertex_input_info.pVertexAttributeDescriptions = &attribute_desc;

// Input assembly
VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
input_assembly.primitiveRestartEnable = VK_FALSE;

// Viewport and scissor
VkViewport viewport = {};
viewport.x = 0.0f;
viewport.y = 0.0f;
viewport.width = (float)swapchain_extent.width;
viewport.height = (float)swapchain_extent.height;
viewport.minDepth = 0.0f;
viewport.maxDepth = 1.0f;

VkRect2D scissor = {};
scissor.offset = {0, 0};
scissor.extent = swapchain_extent;

VkPipelineViewportStateCreateInfo viewport_state = {};
viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
viewport_state.viewportCount = 1;
viewport_state.pViewports = &viewport;
viewport_state.scissorCount = 1;
viewport_state.pScissors = &scissor;

// Rasterizer
VkPipelineRasterizationStateCreateInfo rasterizer = {};
rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
rasterizer.depthClampEnable = VK_FALSE;
rasterizer.rasterizerDiscardEnable = VK_FALSE;
rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
rasterizer.lineWidth = 1.0f;
rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
rasterizer.depthBiasEnable = VK_FALSE;

// Multisampling
VkPipelineMultisampleStateCreateInfo multisampling = {};
multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
multisampling.sampleShadingEnable = VK_FALSE;
multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

// Color blending
VkPipelineColorBlendAttachmentState color_blend_attachment = {};
color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | 
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
color_blend_attachment.blendEnable = VK_TRUE;
color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

VkPipelineColorBlendStateCreateInfo color_blending = {};
color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
color_blending.logicOpEnable = VK_FALSE;
color_blending.attachmentCount = 1;
color_blending.pAttachments = &color_blend_attachment;

// Pipeline layout
VkPipelineLayoutCreateInfo pipeline_layout_info = {};
pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
pipeline_layout_info.setLayoutCount = 0; // No descriptor sets yet
pipeline_layout_info.pushConstantRangeCount = 0; // No push constants yet

vkCreatePipelineLayout(device, &pipeline_layout_info, NULL, &g_vulkan_state.pipeline_layout);

// Create graphics pipeline
VkGraphicsPipelineCreateInfo pipeline_info = {};
pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
pipeline_info.stageCount = 2;
pipeline_info.pStages = shader_stages;
pipeline_info.pVertexInputState = &vertex_input_info;
pipeline_info.pInputAssemblyState = &input_assembly;
pipeline_info.pViewportState = &viewport_state;
pipeline_info.pRasterizationState = &rasterizer;
pipeline_info.pMultisampleState = &multisampling;
pipeline_info.pColorBlendState = &color_blending;
pipeline_info.layout = g_vulkan_state.pipeline_layout;
pipeline_info.renderPass = g_vulkan_state.render_pass;
pipeline_info.subpass = 0;

vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, 
                         &g_vulkan_state.graphics_pipeline);

// Clean up shader modules
vkDestroyShaderModule(device, vert_shader, NULL);
vkDestroyShaderModule(device, frag_shader, NULL);
```

### 4. Buffer Creation Implementation

**Where**: Replace stubs in `upload_mesh_data()`

**What to Add**:
```cpp
void upload_mesh_data(sMesh* p_mesh) {
    if (!p_mesh || !g_vulkan_state.device) return;
    
    VkDeviceSize vertex_size = p_mesh->m_num_vertices * 3 * sizeof(float);
    
    // Create staging buffer
    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;
    create_buffer(vertex_size, 
                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  staging_buffer, staging_memory);
    
    // Copy vertex data to staging
    void* data;
    vkMapMemory(g_vulkan_state.device, staging_memory, 0, vertex_size, 0, &data);
    memcpy(data, p_mesh->mp_positions, vertex_size);
    vkUnmapMemory(g_vulkan_state.device, staging_memory);
    
    // Create device-local vertex buffer
    VkBuffer* vertex_buffer = new VkBuffer();
    VkDeviceMemory* vertex_memory = new VkDeviceMemory();
    create_buffer(vertex_size,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                  *vertex_buffer, *vertex_memory);
    
    // Copy from staging to device
    copy_buffer(staging_buffer, *vertex_buffer, vertex_size);
    
    // Clean up staging
    vkDestroyBuffer(g_vulkan_state.device, staging_buffer, NULL);
    vkFreeMemory(g_vulkan_state.device, staging_memory, NULL);
    
    p_mesh->pVulkanVertexBuffer = vertex_buffer;
    // Store memory handle somewhere to free later
}

// Helper function to create buffers
void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, 
                   VkMemoryPropertyFlags properties,
                   VkBuffer& buffer, VkDeviceMemory& memory) {
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    vkCreateBuffer(g_vulkan_state.device, &buffer_info, NULL, &buffer);
    
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(g_vulkan_state.device, buffer, &mem_requirements);
    
    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);
    
    vkAllocateMemory(g_vulkan_state.device, &alloc_info, NULL, &memory);
    vkBindBufferMemory(g_vulkan_state.device, buffer, memory, 0);
}

// Helper to find suitable memory type
uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) {
    for (uint32_t i = 0; i < g_vulkan_state.memory_properties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) && 
            (g_vulkan_state.memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return 0;
}
```

### 5. Texture Creation Implementation

**Where**: Replace stubs in `create_texture()`

**What to Add**:
```cpp
// Create VkImage
VkImageCreateInfo image_info = {};
image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
image_info.imageType = VK_IMAGE_TYPE_2D;
image_info.extent.width = width;
image_info.extent.height = height;
image_info.extent.depth = 1;
image_info.mipLevels = 1;
image_info.arrayLayers = 1;
image_info.format = vk_format; // From format mapping
image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
image_info.samples = VK_SAMPLE_COUNT_1_BIT;
image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

VkImage* image = new VkImage();
vkCreateImage(g_vulkan_state.device, &image_info, NULL, image);

// Allocate memory
VkMemoryRequirements mem_requirements;
vkGetImageMemoryRequirements(g_vulkan_state.device, *image, &mem_requirements);

VkMemoryAllocateInfo alloc_info = {};
alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
alloc_info.allocationSize = mem_requirements.size;
alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

VkDeviceMemory image_memory;
vkAllocateMemory(g_vulkan_state.device, &alloc_info, NULL, &image_memory);
vkBindImageMemory(g_vulkan_state.device, *image, image_memory, 0);

// Upload texture data (create staging buffer, copy, transition layout)
// ... similar to buffer upload ...

// Create image view
VkImageViewCreateInfo view_info = {};
view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
view_info.image = *image;
view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
view_info.format = vk_format;
view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
view_info.subresourceRange.baseMipLevel = 0;
view_info.subresourceRange.levelCount = 1;
view_info.subresourceRange.baseArrayLayer = 0;
view_info.subresourceRange.layerCount = 1;

VkImageView image_view;
vkCreateImageView(g_vulkan_state.device, &view_info, NULL, &image_view);

p_texture->pVulkanTexture = image;
// Store image_view and image_memory to free later
```

### 6. Rendering Implementation

**Where**: Replace stubs in `render_scene()`

**What to Add**:
```cpp
// Allocate command buffer
VkCommandBufferAllocateInfo alloc_info = {};
alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
alloc_info.commandPool = g_vulkan_state.command_pool;
alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
alloc_info.commandBufferCount = 1;

VkCommandBuffer command_buffer;
vkAllocateCommandBuffers(g_vulkan_state.device, &alloc_info, &command_buffer);

// Begin recording
VkCommandBufferBeginInfo begin_info = {};
begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

vkBeginCommandBuffer(command_buffer, &begin_info);

// Begin render pass
VkRenderPassBeginInfo render_pass_info = {};
render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
render_pass_info.renderPass = g_vulkan_state.render_pass;
render_pass_info.framebuffer = current_framebuffer; // Need to create framebuffers
render_pass_info.renderArea.offset = {0, 0};
render_pass_info.renderArea.extent = swapchain_extent;

VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
render_pass_info.clearValueCount = 1;
render_pass_info.pClearValues = &clear_color;

vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

// Bind pipeline
vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 
                  g_vulkan_state.graphics_pipeline);

// Render each mesh
for (int i = 0; i < p_scene->m_num_meshes; i++) {
    sMesh* mesh = p_scene->mpp_mesh_list[i];
    if (!mesh || !mesh->pVulkanVertexBuffer) continue;
    
    VkBuffer vertex_buffers[] = {*(VkBuffer*)mesh->pVulkanVertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
    
    if (mesh->pVulkanIndexBuffer) {
        vkCmdBindIndexBuffer(command_buffer, *(VkBuffer*)mesh->pVulkanIndexBuffer, 
                            0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(command_buffer, mesh->m_num_indices, 1, 0, 0, 0);
    } else {
        vkCmdDraw(command_buffer, mesh->m_num_vertices, 1, 0, 0);
    }
}

// End render pass
vkCmdEndRenderPass(command_buffer);

// End command buffer
vkEndCommandBuffer(command_buffer);

// Submit to queue
VkSubmitInfo submit_info = {};
submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
submit_info.commandBufferCount = 1;
submit_info.pCommandBuffers = &command_buffer;

vkQueueSubmit(g_vulkan_state.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
vkQueueWaitIdle(g_vulkan_state.graphics_queue); // Better to use fences

// Free command buffer
vkFreeCommandBuffers(g_vulkan_state.device, g_vulkan_state.command_pool, 1, &command_buffer);
```

### 7. Shaders

**Create**: `shaders/shader.vert` and `shaders/shader.frag`

**Vertex Shader (GLSL)**:
```glsl
#version 450

layout(location = 0) in vec3 inPosition;

void main() {
    gl_Position = vec4(inPosition, 1.0);
}
```

**Fragment Shader (GLSL)**:
```glsl
#version 450

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
}
```

**Compile to SPIR-V**:
```bash
glslc shader.vert -o vert.spv
glslc shader.frag -o frag.spv
```

## Testing Strategy

1. **Test stub mode**: Already working (see standalone test)
2. **Test with Vulkan SDK**: Install SDK and rebuild with `-DUSE_VULKAN_RENDERER=ON`
3. **Test initialization**: Verify device creation succeeds
4. **Test buffer creation**: Create test mesh with vertices
5. **Test texture creation**: Load test texture
6. **Test rendering**: Clear screen to verify render pass works
7. **Test mesh rendering**: Draw simple triangle
8. **Test full scene**: Render complete game scene

## Troubleshooting

### Validation Errors

Enable Vulkan validation layers for debugging:
```cpp
const char* validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
create_info.enabledLayerCount = 1;
create_info.ppEnabledLayerNames = validation_layers;
```

### Memory Leaks

Track all VkDeviceMemory allocations and ensure they're freed in shutdown.

### Rendering Issues

- Check render pass configuration
- Verify pipeline state matches data format
- Check vertex attribute descriptions
- Verify descriptor set bindings

## Performance Tips

1. **Batch draw calls**: Minimize state changes
2. **Use staging buffers**: For efficient uploads
3. **Implement descriptor set caching**: Reuse descriptor sets
4. **Multiple frames in flight**: 2-3 frames for better GPU utilization
5. **Memory pooling**: Allocate from larger blocks
6. **Indirect drawing**: For large numbers of draw calls

## References

- [Vulkan Tutorial](https://vulkan-tutorial.com/) - Comprehensive guide
- [Vulkan Guide](https://github.com/KhronosGroup/Vulkan-Guide) - Best practices
- [Vulkan Samples](https://github.com/KhronosGroup/Vulkan-Samples) - Example code
- Existing implementation: `Code/Gfx/Vulcan/VULKAN_IMPLEMENTATION_NOTES.md`

## Support

Questions? Check:
1. Implementation notes: `VULKAN_IMPLEMENTATION_NOTES.md`
2. Standalone test: `test_vulkan_standalone.cpp`
3. Vulkan specification: https://www.khronos.org/vulkan/
