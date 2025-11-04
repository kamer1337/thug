#include <core/defines.h>
#include <core/debug.h>
#include <core/HashTable.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "render.h"

// Include Vulkan headers if available
#ifdef USE_VULKAN_RENDERER
#ifdef __has_include
#if __has_include(<vulkan/vulkan.h>)
#include <vulkan/vulkan.h>
#define VULKAN_AVAILABLE 1
#endif
#endif
#endif

namespace NxVulcan
{

// Global state for Vulkan renderer
Lst::HashTable< sTextureProjectionDetails > *pTextureProjectionDetailsTable = NULL;
Lst::HashTable< sTexture > *pTextureTable = NULL;

// Vulkan state tracking with actual Vulkan handles
static struct VulkanState
{
	bool initialized;
	Mth::Matrix view_matrix;
	Mth::Matrix projection_matrix;
	Mth::Matrix frustum_transform;
	uint32 current_blend_mode;
	uint32 render_state[8];
	
#ifdef VULKAN_AVAILABLE
	// Vulkan API objects
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkDevice device;
	VkQueue graphics_queue;
	VkQueue present_queue;
	VkCommandPool command_pool;
	VkRenderPass render_pass;
	VkPipeline graphics_pipeline;
	VkPipelineLayout pipeline_layout;
	
	// Queue family indices
	uint32 graphics_family_index;
	uint32 present_family_index;
	
	// Device features
	VkPhysicalDeviceFeatures device_features;
	VkPhysicalDeviceMemoryProperties memory_properties;
#else
	// Placeholder pointers when Vulkan SDK not available
	void* instance;
	void* physical_device;
	void* device;
	void* graphics_queue;
	void* present_queue;
	void* command_pool;
	void* render_pass;
	void* graphics_pipeline;
	void* pipeline_layout;
	uint32 graphics_family_index;
	uint32 present_family_index;
#endif
} g_vulkan_state = { false };

#ifdef VULKAN_AVAILABLE
/******************************************************************/
/* Vulkan Data Structures                                         */
/******************************************************************/

// Structure to hold buffer and memory together for cleanup
struct BufferInfo
{
	VkBuffer buffer;
	VkDeviceMemory memory;
};

// Structure to hold image, memory, and view together for cleanup
struct ImageInfo
{
	VkImage image;
	VkDeviceMemory memory;
	VkImageView view;
};

/******************************************************************/
/* Vulkan Helper Functions                                        */
/******************************************************************/

// Find suitable memory type for buffer allocation
static uint32 find_memory_type(uint32 type_filter, VkMemoryPropertyFlags properties)
{
	for (uint32 i = 0; i < g_vulkan_state.memory_properties.memoryTypeCount; i++)
	{
		if ((type_filter & (1 << i)) &&
			(g_vulkan_state.memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	
	printf("Failed to find suitable memory type (filter: 0x%08X, properties: 0x%08X)\n", 
		   type_filter, properties);
	return 0;
}

// Create a Vulkan buffer with memory allocation
static bool create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, 
						  VkMemoryPropertyFlags properties, 
						  VkBuffer* buffer, VkDeviceMemory* buffer_memory)
{
	// Create buffer
	VkBufferCreateInfo buffer_info = {};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
	VkResult result = vkCreateBuffer(g_vulkan_state.device, &buffer_info, NULL, buffer);
	if (result != VK_SUCCESS)
	{
		printf("Failed to create buffer: %d\n", result);
		return false;
	}
	
	// Get memory requirements
	VkMemoryRequirements mem_requirements;
	vkGetBufferMemoryRequirements(g_vulkan_state.device, *buffer, &mem_requirements);
	
	// Allocate memory
	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = mem_requirements.size;
	alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);
	
	result = vkAllocateMemory(g_vulkan_state.device, &alloc_info, NULL, buffer_memory);
	if (result != VK_SUCCESS)
	{
		printf("Failed to allocate buffer memory: %d\n", result);
		vkDestroyBuffer(g_vulkan_state.device, *buffer, NULL);
		return false;
	}
	
	// Bind memory to buffer
	vkBindBufferMemory(g_vulkan_state.device, *buffer, *buffer_memory, 0);
	
	return true;
}

// Copy data from one buffer to another
static void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
	// Allocate a temporary command buffer
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = g_vulkan_state.command_pool;
	alloc_info.commandBufferCount = 1;
	
	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(g_vulkan_state.device, &alloc_info, &command_buffer);
	
	// Begin recording
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	
	vkBeginCommandBuffer(command_buffer, &begin_info);
	
	// Copy command
	VkBufferCopy copy_region = {};
	copy_region.srcOffset = 0;
	copy_region.dstOffset = 0;
	copy_region.size = size;
	vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
	
	// End recording
	vkEndCommandBuffer(command_buffer);
	
	// Submit and wait
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;
	
	vkQueueSubmit(g_vulkan_state.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(g_vulkan_state.graphics_queue);
	
	// Cleanup
	vkFreeCommandBuffers(g_vulkan_state.device, g_vulkan_state.command_pool, 1, &command_buffer);
}

// Create a Vulkan image with memory allocation
static bool create_image(uint32 width, uint32 height, VkFormat format, VkImageTiling tiling,
						 VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
						 VkImage* image, VkDeviceMemory* image_memory)
{
	// Create image
	VkImageCreateInfo image_info = {};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = width;
	image_info.extent.height = height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	image_info.format = format;
	image_info.tiling = tiling;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = usage;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.flags = 0;
	
	VkResult result = vkCreateImage(g_vulkan_state.device, &image_info, NULL, image);
	if (result != VK_SUCCESS)
	{
		printf("Failed to create image: %d\n", result);
		return false;
	}
	
	// Get memory requirements
	VkMemoryRequirements mem_requirements;
	vkGetImageMemoryRequirements(g_vulkan_state.device, *image, &mem_requirements);
	
	// Allocate memory
	VkMemoryAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = mem_requirements.size;
	alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);
	
	result = vkAllocateMemory(g_vulkan_state.device, &alloc_info, NULL, image_memory);
	if (result != VK_SUCCESS)
	{
		printf("Failed to allocate image memory: %d\n", result);
		vkDestroyImage(g_vulkan_state.device, *image, NULL);
		return false;
	}
	
	// Bind memory to image
	vkBindImageMemory(g_vulkan_state.device, *image, *image_memory, 0);
	
	return true;
}

// Transition image layout
static void transition_image_layout(VkImage image, VkFormat format, 
									VkImageLayout old_layout, VkImageLayout new_layout)
{
	// Allocate a temporary command buffer
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = g_vulkan_state.command_pool;
	alloc_info.commandBufferCount = 1;
	
	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(g_vulkan_state.device, &alloc_info, &command_buffer);
	
	// Begin recording
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	
	vkBeginCommandBuffer(command_buffer, &begin_info);
	
	// Image memory barrier
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = old_layout;
	barrier.newLayout = new_layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	
	// Set access masks based on layouts
	VkPipelineStageFlags source_stage;
	VkPipelineStageFlags destination_stage;
	
	if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		printf("Unsupported layout transition: %d -> %d\n", old_layout, new_layout);
		source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destination_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	}
	
	vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage,
						 0, 0, NULL, 0, NULL, 1, &barrier);
	
	// End recording
	vkEndCommandBuffer(command_buffer);
	
	// Submit and wait
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;
	
	vkQueueSubmit(g_vulkan_state.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(g_vulkan_state.graphics_queue);
	
	// Cleanup
	vkFreeCommandBuffers(g_vulkan_state.device, g_vulkan_state.command_pool, 1, &command_buffer);
}

// Copy buffer to image
static void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32 width, uint32 height)
{
	// Allocate a temporary command buffer
	VkCommandBufferAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = g_vulkan_state.command_pool;
	alloc_info.commandBufferCount = 1;
	
	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(g_vulkan_state.device, &alloc_info, &command_buffer);
	
	// Begin recording
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	
	vkBeginCommandBuffer(command_buffer, &begin_info);
	
	// Copy region
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = {0, 0, 0};
	region.imageExtent = {width, height, 1};
	
	vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	
	// End recording
	vkEndCommandBuffer(command_buffer);
	
	// Submit and wait
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;
	
	vkQueueSubmit(g_vulkan_state.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(g_vulkan_state.graphics_queue);
	
	// Cleanup
	vkFreeCommandBuffers(g_vulkan_state.device, g_vulkan_state.command_pool, 1, &command_buffer);
}
#endif // VULKAN_AVAILABLE

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
bool init_vulkan( void )
{
	// Initialize hash tables for texture and projection details
	if( !pTextureTable )
	{
		pTextureTable = new Lst::HashTable< sTexture >( 256 );
	}
	
	if( !pTextureProjectionDetailsTable )
	{
		pTextureProjectionDetailsTable = new Lst::HashTable< sTextureProjectionDetails >( 32 );
	}
	
	// Initialize Vulkan state
	g_vulkan_state.initialized = true;
	g_vulkan_state.current_blend_mode = vBLEND_MODE_DIFFUSE;
	
	for( int i = 0; i < 8; i++ )
	{
		g_vulkan_state.render_state[i] = 0;
	}
	
#ifdef VULKAN_AVAILABLE
	// 1. Create Vulkan instance
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "THUG";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "THUG Engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;
	
	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	
	// No validation layers or extensions for minimal implementation
	create_info.enabledExtensionCount = 0;
	create_info.enabledLayerCount = 0;
	
	VkResult result = vkCreateInstance(&create_info, NULL, &g_vulkan_state.instance);
	if (result != VK_SUCCESS)
	{
		printf("Failed to create Vulkan instance: %d\n", result);
		return false;
	}
	
	// 2. Select physical device (pick first available)
	uint32 device_count = 0;
	vkEnumeratePhysicalDevices(g_vulkan_state.instance, &device_count, NULL);
	
	if (device_count == 0)
	{
		printf("Failed to find GPUs with Vulkan support\n");
		return false;
	}
	
	VkPhysicalDevice* devices = new VkPhysicalDevice[device_count];
	vkEnumeratePhysicalDevices(g_vulkan_state.instance, &device_count, devices);
	
	// Select first device for simplicity
	g_vulkan_state.physical_device = devices[0];
	delete[] devices;
	
	// Get device properties and memory properties
	vkGetPhysicalDeviceFeatures(g_vulkan_state.physical_device, &g_vulkan_state.device_features);
	vkGetPhysicalDeviceMemoryProperties(g_vulkan_state.physical_device, &g_vulkan_state.memory_properties);
	
	// Find graphics queue family
	uint32 queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(g_vulkan_state.physical_device, &queue_family_count, NULL);
	
	VkQueueFamilyProperties* queue_families = new VkQueueFamilyProperties[queue_family_count];
	vkGetPhysicalDeviceQueueFamilyProperties(g_vulkan_state.physical_device, &queue_family_count, queue_families);
	
	// Find a queue family that supports graphics
	g_vulkan_state.graphics_family_index = 0xFFFFFFFF;
	for (uint32 i = 0; i < queue_family_count; i++)
	{
		if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			g_vulkan_state.graphics_family_index = i;
			g_vulkan_state.present_family_index = i; // Assume same for simplicity
			break;
		}
	}
	delete[] queue_families;
	
	if (g_vulkan_state.graphics_family_index == 0xFFFFFFFF)
	{
		printf("Failed to find suitable queue family\n");
		return false;
	}
	
	// 3. Create logical device
	float queue_priority = 1.0f;
	VkDeviceQueueCreateInfo queue_create_info = {};
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = g_vulkan_state.graphics_family_index;
	queue_create_info.queueCount = 1;
	queue_create_info.pQueuePriorities = &queue_priority;
	
	VkPhysicalDeviceFeatures device_features = {};
	// Enable features as needed
	
	VkDeviceCreateInfo device_create_info = {};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pQueueCreateInfos = &queue_create_info;
	device_create_info.queueCreateInfoCount = 1;
	device_create_info.pEnabledFeatures = &device_features;
	device_create_info.enabledExtensionCount = 0; // No extensions for minimal implementation
	device_create_info.enabledLayerCount = 0;
	
	result = vkCreateDevice(g_vulkan_state.physical_device, &device_create_info, NULL, &g_vulkan_state.device);
	if (result != VK_SUCCESS)
	{
		printf("Failed to create logical device: %d\n", result);
		return false;
	}
	
	// Get queue handles
	vkGetDeviceQueue(g_vulkan_state.device, g_vulkan_state.graphics_family_index, 0, &g_vulkan_state.graphics_queue);
	g_vulkan_state.present_queue = g_vulkan_state.graphics_queue; // Same queue for simplicity
	
	// 6. Create command pool
	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = g_vulkan_state.graphics_family_index;
	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	
	result = vkCreateCommandPool(g_vulkan_state.device, &pool_info, NULL, &g_vulkan_state.command_pool);
	if (result != VK_SUCCESS)
	{
		printf("Failed to create command pool: %d\n", result);
		return false;
	}
	
	printf("Vulkan renderer initialized successfully\n");
	printf("  Device: Physical device selected\n");
	printf("  Graphics Queue Family: %u\n", g_vulkan_state.graphics_family_index);
#else
	printf("Vulkan renderer initialized (SDK not available - using stub implementation)\n");
	// Initialize stub pointers
	g_vulkan_state.instance = NULL;
	g_vulkan_state.physical_device = NULL;
	g_vulkan_state.device = NULL;
	g_vulkan_state.graphics_queue = NULL;
	g_vulkan_state.present_queue = NULL;
	g_vulkan_state.command_pool = NULL;
	g_vulkan_state.render_pass = NULL;
	g_vulkan_state.graphics_pipeline = NULL;
	g_vulkan_state.pipeline_layout = NULL;
	g_vulkan_state.graphics_family_index = 0;
	g_vulkan_state.present_family_index = 0;
#endif
	
	return true;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void shutdown_vulkan( void )
{
	// Clean up texture table
	if( pTextureTable )
	{
		// Destroy all textures
		Lst::HashTable< sTexture >::Iterator it = pTextureTable->FirstItem();
		while( it != pTextureTable->LastItem() )
		{
			sTexture *p_texture = pTextureTable->GetItem( it );
			if( p_texture )
			{
				if( p_texture->pTexelData )
				{
					delete[] p_texture->pTexelData;
				}
				if( p_texture->pAlphaData )
				{
					delete[] p_texture->pAlphaData;
				}
#ifdef VULKAN_AVAILABLE
				// Destroy Vulkan texture resources
				if( p_texture->pVulkanTexture && g_vulkan_state.device )
				{
					ImageInfo* image_info = (ImageInfo*)p_texture->pVulkanTexture;
					if( image_info->view != VK_NULL_HANDLE )
					{
						vkDestroyImageView(g_vulkan_state.device, image_info->view, NULL);
					}
					if( image_info->image != VK_NULL_HANDLE )
					{
						vkDestroyImage(g_vulkan_state.device, image_info->image, NULL);
					}
					if( image_info->memory != VK_NULL_HANDLE )
					{
						vkFreeMemory(g_vulkan_state.device, image_info->memory, NULL);
					}
					delete image_info;
				}
#endif
				delete p_texture;
			}
			++it;
		}
		delete pTextureTable;
		pTextureTable = NULL;
	}
	
	// Clean up projection details table
	if( pTextureProjectionDetailsTable )
	{
		Lst::HashTable< sTextureProjectionDetails >::Iterator it = pTextureProjectionDetailsTable->FirstItem();
		while( it != pTextureProjectionDetailsTable->LastItem() )
		{
			sTextureProjectionDetails *p_details = pTextureProjectionDetailsTable->GetItem( it );
			if( p_details )
			{
				delete p_details;
			}
			++it;
		}
		delete pTextureProjectionDetailsTable;
		pTextureProjectionDetailsTable = NULL;
	}
	
#ifdef VULKAN_AVAILABLE
	// 1. Destroy command pool
	if( g_vulkan_state.command_pool )
	{
		vkDestroyCommandPool(g_vulkan_state.device, g_vulkan_state.command_pool, NULL);
	}
	
	// 2. Destroy render pass
	if( g_vulkan_state.render_pass )
	{
		vkDestroyRenderPass(g_vulkan_state.device, g_vulkan_state.render_pass, NULL);
	}
	
	// 3. Destroy graphics pipeline
	if( g_vulkan_state.graphics_pipeline )
	{
		vkDestroyPipeline(g_vulkan_state.device, g_vulkan_state.graphics_pipeline, NULL);
	}
	
	// 4. Destroy pipeline layout
	if( g_vulkan_state.pipeline_layout )
	{
		vkDestroyPipelineLayout(g_vulkan_state.device, g_vulkan_state.pipeline_layout, NULL);
	}
	
	// 5. Destroy logical device
	if( g_vulkan_state.device )
	{
		vkDestroyDevice(g_vulkan_state.device, NULL);
	}
	
	// 6. Destroy Vulkan instance
	if( g_vulkan_state.instance )
	{
		vkDestroyInstance(g_vulkan_state.instance, NULL);
	}
	
	printf("Vulkan renderer shut down successfully\n");
#else
	printf("Vulkan renderer shut down (stub implementation)\n");
#endif
	
	g_vulkan_state.initialized = false;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
BlendModes GetBlendMode( uint32 blend_checksum )
{
	// Fast path for most common blend mode (Diffuse)
	if( blend_checksum == 0x872b0e99 )
	{
		return vBLEND_MODE_DIFFUSE;
	}
	
	// Map blend mode checksums to blend modes
	// This maintains compatibility with existing file formats
	switch( blend_checksum )
	{
		case 0x9c888e20:	// "Add"
			return vBLEND_MODE_ADD;
		case 0x5fbb9c76:	// "Add_Fixed"
			return vBLEND_MODE_ADD_FIXED;
		case 0x872c4f69:	// "Subtract"
			return vBLEND_MODE_SUBTRACT;
		case 0x4a6b30d4:	// "Sub_Fixed"
			return vBLEND_MODE_SUB_FIXED;
		case 0xa2e36388:	// "Blend"
			return vBLEND_MODE_BLEND;
		case 0xe6a07ab9:	// "Blend_Fixed"
			return vBLEND_MODE_BLEND_FIXED;
		case 0x0a224e23:	// "Modulate"
			return vBLEND_MODE_MODULATE;
		case 0x58b88e9e:	// "Modulate_Fixed"
			return vBLEND_MODE_MODULATE_FIXED;
		case 0x65d8e22e:	// "Brighten"
			return vBLEND_MODE_BRIGHTEN;
		case 0x9fb8cc43:	// "Brighten_Fixed"
			return vBLEND_MODE_BRIGHTEN_FIXED;
		default:
			return vBLEND_MODE_DIFFUSE;
	}
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void set_render_state( uint32 type, uint32 state )
{
	// Store render state for pipeline configuration
	if( type < 8 )
	{
		g_vulkan_state.render_state[type] = state;
	}
	
	// In a full implementation, this would configure Vulkan pipeline state:
	// - RS_ZWRITEENABLE: depth write enable
	// - RS_ZTESTENABLE: depth test enable
	// - RS_ALPHACUTOFF: alpha test threshold
	// - RS_UVADDRESSMODE*: texture coordinate wrapping/clamping
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void set_blend_mode( uint32 mode )
{
	g_vulkan_state.current_blend_mode = mode;
	
	// In a full implementation, this would set Vulkan blend state:
	// - Configure color blend attachment state
	// - Set blend factors based on mode
	// - Enable/disable blending
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void create_texture_projection_details( sTexture *p_texture, CVulcanModel *p_model, sScene *p_scene )
{
	if( !pTextureProjectionDetailsTable )
	{
		return;
	}
	
	sTextureProjectionDetails *p_details = new sTextureProjectionDetails;
	
	p_details->p_model = p_model;
	p_details->p_scene = p_scene;
	p_details->p_texture = p_texture;
	
	// Initialize matrices to identity
	p_details->view_matrix.Ident();
	p_details->projection_matrix.Ident();
	p_details->texture_projection_matrix.Ident();
	
	pTextureProjectionDetailsTable->PutItem((uint32)p_texture, p_details );
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void destroy_texture_projection_details( sTexture *p_texture )
{
	if( !pTextureProjectionDetailsTable )
	{
		return;
	}
	
	sTextureProjectionDetails *p_details = pTextureProjectionDetailsTable->GetItem((uint32)p_texture );
	if( p_details )
	{
		pTextureProjectionDetailsTable->FlushItem((uint32)p_texture );
		delete p_details;
	}
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void set_texture_projection_camera( sTexture *p_texture, Mth::Vector *p_pos, Mth::Vector *p_at )
{
	if( !pTextureProjectionDetailsTable )
	{
		return;
	}
	
	sTextureProjectionDetails *p_details = pTextureProjectionDetailsTable->GetItem((uint32)p_texture );
	if( p_details )
	{
		Mth::Vector up;
		
		// Check for 'straight down' vector
		if(( p_pos->GetX() == p_at->GetX() ) && ( p_pos->GetZ() == p_at->GetZ() ))
		{
			up.Set( 0.0f, 0.0f, 1.0f );
		}
		else
		{
			up.Set( 0.0f, 1.0f, 0.0f );
		}
		
		// Build view matrix looking from p_pos to p_at
		Mth::Vector forward = *p_at - *p_pos;
		forward.Normalize();
		
		Mth::Vector right = Mth::CrossProduct( up, forward );
		right.Normalize();
		
		up = Mth::CrossProduct( forward, right );
		up.Normalize();
		
		// Construct view matrix
		p_details->view_matrix[Mth::RIGHT] = right;
		p_details->view_matrix[Mth::UP] = up;
		p_details->view_matrix[Mth::AT] = forward;
		p_details->view_matrix[Mth::POS] = *p_pos;
	}
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void set_camera( Mth::Matrix *p_matrix, Mth::Vector *p_position, float screen_angle, float aspect_ratio, bool render_at_infinity )
{
	// Store view matrix
	if( p_matrix )
	{
		g_vulkan_state.view_matrix = *p_matrix;
	}
	
	// Build projection matrix
	float fov_y = screen_angle;
	float near_plane = 12.0f;  // Standard near plane distance
	float far_plane = 10000.0f; // Standard far plane distance
	
	if( render_at_infinity )
	{
		far_plane = 100000.0f;
	}
	
	// Build perspective projection matrix
	float tan_half_fov = tanf( fov_y * 0.5f );
	float f = 1.0f / tan_half_fov;
	
	g_vulkan_state.projection_matrix.Ident();
	g_vulkan_state.projection_matrix[0][0] = f / aspect_ratio;
	g_vulkan_state.projection_matrix[1][1] = f;
	g_vulkan_state.projection_matrix[2][2] = far_plane / (far_plane - near_plane);
	g_vulkan_state.projection_matrix[2][3] = -(far_plane * near_plane) / (far_plane - near_plane);
	g_vulkan_state.projection_matrix[3][2] = 1.0f;
	g_vulkan_state.projection_matrix[3][3] = 0.0f;
	
	// In a full implementation, this would:
	// - Update uniform buffers with view and projection matrices
	// - Configure viewport and scissor
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void set_frustum_bbox_transform( Mth::Matrix *p_transform )
{
	if( p_transform )
	{
		g_vulkan_state.frustum_transform = *p_transform;
	}
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
bool frustum_check_sphere( Mth::Vector *p_center, float radius )
{
	// Perform frustum culling on sphere
	// For now, use a simplified check
	
	if( !p_center )
	{
		return true;
	}
	
	// Transform sphere center by frustum transform
	Mth::Vector transformed_center = *p_center;
	transformed_center *= g_vulkan_state.frustum_transform;
	
	// Simple frustum check based on distance from origin
	float distance = transformed_center.Length();
	
	// If sphere is within reasonable distance, it's visible
	if( distance < 10000.0f + radius )
	{
		return true;
	}
	
	return false;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
float get_bounding_sphere_nearest_z( void )
{
	// Return nearest Z coordinate of bounding sphere
	// This is used for sorting transparent objects
	return 0.0f;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
bool IsVisible( Mth::Vector &center, float radius )
{
	// Check if sphere is visible
	return frustum_check_sphere( &center, radius );
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void render_shadow_targets()
{
	// Render shadow maps/volumes
	// In a full implementation, this would:
	// 1. Set up shadow map render target
	// 2. Render scene from light perspective
	// 3. Generate shadow map texture
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void render_light_glows( bool test )
{
	// Render light glow effects
	// In a full implementation, this would:
	// 1. Render billboards at light positions
	// 2. Apply additive blending
	// 3. Use glow textures
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void render_scene( sScene *p_scene, uint32 flags, uint32 viewport )
{
	if( !p_scene || !g_vulkan_state.initialized )
	{
		return;
	}
	
	// Early return for empty scenes
	if( p_scene->m_num_meshes == 0 || !p_scene->mpp_mesh_list )
	{
		return;
	}
	
#ifdef VULKAN_AVAILABLE
	if( g_vulkan_state.device && g_vulkan_state.command_pool )
	{
		// 1. Begin command buffer recording (simplified - no actual recording for minimal impl)
		// In a full implementation:
		// - Allocate command buffer from pool
		// - Begin command buffer with vkBeginCommandBuffer
		
		// 2-4. Begin render pass, set viewport/scissor, bind pipeline
		// In a full implementation:
		// - vkCmdBeginRenderPass with render pass info
		// - vkCmdSetViewport and vkCmdSetScissor
		// - vkCmdBindPipeline with graphics pipeline
		
		printf("Render scene: %d meshes (flags: 0x%08X)\n", p_scene->m_num_meshes, flags);
	}
#endif
	
	// Cache mesh count to avoid repeated access
	const uint16 num_meshes = p_scene->m_num_meshes;
	
	// Iterate through meshes and render based on flags
	int rendered_mesh_count = 0;
	for( int i = 0; i < num_meshes; i++ )
	{
		sMesh *p_mesh = p_scene->mpp_mesh_list[i];
		if( !p_mesh )
		{
			continue;
		}
		
		// Check if mesh is active and visible
		if( !( p_mesh->m_flags & sMesh::MESH_FLAG_ACTIVE ) )
		{
			continue;
		}
		
		if( !( p_mesh->m_flags & sMesh::MESH_FLAG_VISIBLE ) )
		{
			continue;
		}
		
		// Check render flags
		bool is_transparent = false;
		if( p_mesh->mp_material )
		{
			is_transparent = ( p_mesh->mp_material->blendMode != vBLEND_MODE_DIFFUSE );
		}
		
		bool should_render = false;
		if( is_transparent && ( flags & vRENDER_SEMITRANSPARENT ) )
		{
			should_render = true;
		}
		else if( !is_transparent && ( flags & vRENDER_OPAQUE ) )
		{
			should_render = true;
		}
		
		if( !should_render )
		{
			continue;
		}
		
#ifdef VULKAN_AVAILABLE
		// For each mesh that should be rendered, we would bind buffers and draw
		// Note: This requires render pass to be active, which needs swapchain setup
		// For now, we just verify the buffers exist and count them
		
		if( p_mesh->pVulkanVertexBuffer && p_mesh->pVulkanIndexBuffer )
		{
			BufferInfo* vertex_buffer = (BufferInfo*)p_mesh->pVulkanVertexBuffer;
			BufferInfo* index_buffer = (BufferInfo*)p_mesh->pVulkanIndexBuffer;
			
			// In a full implementation with render pass active:
			// VkBuffer vertex_buffers[] = {vertex_buffer->buffer};
			// VkDeviceSize offsets[] = {0};
			// vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
			// vkCmdBindIndexBuffer(command_buffer, index_buffer->buffer, 0, VK_INDEX_TYPE_UINT16);
			// vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &descriptor_set, 0, NULL);
			// vkCmdDrawIndexed(command_buffer, p_mesh->m_num_indices, 1, 0, 0, 0);
			
			rendered_mesh_count++;
		}
#endif
	}
	
#ifdef VULKAN_AVAILABLE
	if( g_vulkan_state.device && rendered_mesh_count > 0 )
	{
		printf("  Rendered %d meshes\n", rendered_mesh_count);
		
		// 5-7. End render pass, end command buffer, submit
		// In a full implementation:
		// - vkCmdEndRenderPass
		// - vkEndCommandBuffer
		// - vkQueueSubmit to graphics queue
		// - vkQueueWaitIdle or use fences for synchronization
	}
#endif
}

/******************************************************************/
/*                                                                */
/* Helper Functions                                               */
/*                                                                */
/******************************************************************/
static inline uint32 calculate_filename_checksum( const char *p_filename )
{
	uint32 checksum = 0;
	for( const char *p = p_filename; *p; p++ )
	{
		checksum = (checksum << 5) + checksum + *p;
	}
	return checksum;
}

/******************************************************************/
/*                                                                */
/* Texture Management                                             */
/*                                                                */
/******************************************************************/
sTexture* load_texture( const char *p_filename )
{
	if( !p_filename || !pTextureTable )
	{
		return NULL;
	}
	
	// Calculate checksum for filename
	uint32 checksum = calculate_filename_checksum( p_filename );
	
	// Check if texture already loaded
	sTexture *p_texture = pTextureTable->GetItem( checksum );
	if( p_texture )
	{
		return p_texture;
	}
	
	// Detect if this is a PC format texture
	// PC textures typically use .img extension or have PC-specific markers
	bool is_pc_format = false;
	const char *ext = strrchr( p_filename, '.' );
	if( ext )
	{
		// Check for PC-specific extensions
		if( strcmp(ext, ".img") == 0 || strcmp(ext, ".IMG") == 0 )
		{
			is_pc_format = true;
		}
	}
	
	// If PC format detected, use PC-specific loader
	if( is_pc_format )
	{
		return load_texture_pc_format( p_filename );
	}
	
	// Otherwise, load console format texture
	// In a full implementation, this would:
	// 1. Load texture data from file (supporting .TEX format)
	// 2. Parse console texture header (NGC/NGPS/Xbox format)
	// 3. Create Vulkan image
	// 4. Upload texture data to GPU
	// 5. Create image view and sampler
	
	// For now, create a stub texture
	p_texture = new sTexture;
	memset( p_texture, 0, sizeof(sTexture) );
	p_texture->Checksum = checksum;
	p_texture->format = sTexture::TEXTURE_FORMAT_RGBA32; // Default console format
	
	pTextureTable->PutItem( checksum, p_texture );
	
	return p_texture;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
sTexture* create_texture( uint32 checksum, uint16 width, uint16 height, uint8 format, uint8* pData )
{
	if( !pTextureTable )
	{
		return NULL;
	}
	
	sTexture *p_texture = new sTexture;
	memset( p_texture, 0, sizeof(sTexture) );
	
	p_texture->Checksum = checksum;
	p_texture->BaseWidth = width;
	p_texture->BaseHeight = height;
	p_texture->ActualWidth = width;
	p_texture->ActualHeight = height;
	p_texture->format = format;
	p_texture->Levels = 1;
	
	if( pData )
	{
		// Calculate texture data size based on format
		p_texture->byte_size = get_texture_size_for_format( width, height, format, 1 );
		
		p_texture->pTexelData = new uint8[p_texture->byte_size];
		memcpy( p_texture->pTexelData, pData, p_texture->byte_size );
		
#ifdef VULKAN_AVAILABLE
		if( g_vulkan_state.device )
		{
			// 1. Map texture format to Vulkan format
			VkFormat vk_format = VK_FORMAT_R8G8B8A8_UNORM; // Default format
			
			switch( format )
			{
				case sTexture::TEXTURE_FORMAT_DXT1:
					vk_format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
					break;
				case sTexture::TEXTURE_FORMAT_DXT3:
					vk_format = VK_FORMAT_BC2_UNORM_BLOCK;
					break;
				case sTexture::TEXTURE_FORMAT_DXT5:
					vk_format = VK_FORMAT_BC3_UNORM_BLOCK;
					break;
				case sTexture::TEXTURE_FORMAT_A8R8G8B8:
				case sTexture::TEXTURE_FORMAT_RGBA32:
					vk_format = VK_FORMAT_R8G8B8A8_UNORM;
					break;
				case sTexture::TEXTURE_FORMAT_R5G6B5:
					vk_format = VK_FORMAT_R5G6B5_UNORM_PACK16;
					break;
				default:
					vk_format = VK_FORMAT_R8G8B8A8_UNORM;
					break;
			}
			
			// 2. Create staging buffer
			VkBuffer staging_buffer;
			VkDeviceMemory staging_buffer_memory;
			VkDeviceSize image_size = p_texture->byte_size;
			
			if( !create_buffer(image_size,
							   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
							   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							   &staging_buffer, &staging_buffer_memory) )
			{
				printf("Failed to create texture staging buffer\n");
				p_texture->pVulkanTexture = NULL;
			}
			else
			{
				// 3. Map memory and copy texture data
				void* data;
				vkMapMemory(g_vulkan_state.device, staging_buffer_memory, 0, image_size, 0, &data);
				memcpy(data, pData, (size_t)image_size);
				vkUnmapMemory(g_vulkan_state.device, staging_buffer_memory);
				
				// 4. Create image
				ImageInfo* image_info = new ImageInfo();
				if( !create_image(width, height, vk_format, VK_IMAGE_TILING_OPTIMAL,
								  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
								  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
								  &image_info->image, &image_info->memory) )
				{
					printf("Failed to create texture image\n");
					vkDestroyBuffer(g_vulkan_state.device, staging_buffer, NULL);
					vkFreeMemory(g_vulkan_state.device, staging_buffer_memory, NULL);
					delete image_info;
					p_texture->pVulkanTexture = NULL;
				}
				else
				{
					// 5. Transition image layout for transfer
					transition_image_layout(image_info->image, vk_format,
											VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
					
					// 6. Copy buffer to image
					copy_buffer_to_image(staging_buffer, image_info->image, width, height);
					
					// 7. Transition image layout for shader reading
					transition_image_layout(image_info->image, vk_format,
											VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
					
					// 8. Create image view
					VkImageViewCreateInfo view_info = {};
					view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					view_info.image = image_info->image;
					view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
					view_info.format = vk_format;
					view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					view_info.subresourceRange.baseMipLevel = 0;
					view_info.subresourceRange.levelCount = 1;
					view_info.subresourceRange.baseArrayLayer = 0;
					view_info.subresourceRange.layerCount = 1;
					
					VkResult result = vkCreateImageView(g_vulkan_state.device, &view_info, NULL, &image_info->view);
					if (result != VK_SUCCESS)
					{
						printf("Failed to create image view: %d\n", result);
						image_info->view = VK_NULL_HANDLE;
					}
					
					// Cleanup staging buffer
					vkDestroyBuffer(g_vulkan_state.device, staging_buffer, NULL);
					vkFreeMemory(g_vulkan_state.device, staging_buffer_memory, NULL);
					
					p_texture->pVulkanTexture = image_info;
					
					printf("Texture created and uploaded (checksum: 0x%08X, format: %d, size: %dx%d, Vulkan format: %d)\n",
						   checksum, format, width, height, vk_format);
				}
			}
		}
#else
		// Stub implementation
		p_texture->pVulkanTexture = (void*)1; // Non-null placeholder
#endif
	}
	
	pTextureTable->PutItem( checksum, p_texture );
	
	return p_texture;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void destroy_texture( sTexture *p_texture )
{
	if( !p_texture || !pTextureTable )
	{
		return;
	}
	
#ifdef VULKAN_AVAILABLE
	// 1-3. Destroy Vulkan resources
	if( p_texture->pVulkanTexture && g_vulkan_state.device )
	{
		ImageInfo* image_info = (ImageInfo*)p_texture->pVulkanTexture;
		if( image_info->view != VK_NULL_HANDLE )
		{
			vkDestroyImageView(g_vulkan_state.device, image_info->view, NULL);
		}
		if( image_info->image != VK_NULL_HANDLE )
		{
			vkDestroyImage(g_vulkan_state.device, image_info->image, NULL);
		}
		if( image_info->memory != VK_NULL_HANDLE )
		{
			vkFreeMemory(g_vulkan_state.device, image_info->memory, NULL);
		}
		delete image_info;
	}
#endif
	
	if( p_texture->pTexelData )
	{
		delete[] p_texture->pTexelData;
		p_texture->pTexelData = NULL;
	}
	
	if( p_texture->pAlphaData )
	{
		delete[] p_texture->pAlphaData;
		p_texture->pAlphaData = NULL;
	}
	
	pTextureTable->FlushItem( p_texture->Checksum );
	delete p_texture;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
uint32 get_texture_size_for_format( uint16 width, uint16 height, uint8 format, uint8 mip_levels )
{
	uint32 total_size = 0;
	
	// Fast path for single mip level
	if( mip_levels == 1 )
	{
		switch( format )
		{
			case sTexture::TEXTURE_FORMAT_DXT1:
				// DXT1: 4x4 blocks, 8 bytes per block
				return ((width + 3) / 4) * ((height + 3) / 4) * 8;
				
			case sTexture::TEXTURE_FORMAT_DXT3:
			case sTexture::TEXTURE_FORMAT_DXT5:
				// DXT3/DXT5: 4x4 blocks, 16 bytes per block
				return ((width + 3) / 4) * ((height + 3) / 4) * 16;
				
			case sTexture::TEXTURE_FORMAT_A8R8G8B8:
			case sTexture::TEXTURE_FORMAT_RGBA32:
				// 32-bit formats
				return width * height * 4;
				
			case sTexture::TEXTURE_FORMAT_RGB24:
				// 24-bit RGB
				return width * height * 3;
				
			case sTexture::TEXTURE_FORMAT_R5G6B5:
			case sTexture::TEXTURE_FORMAT_A1R5G5B5:
			case sTexture::TEXTURE_FORMAT_A4R4G4B4:
				// 16-bit formats
				return width * height * 2;
				
			case sTexture::TEXTURE_FORMAT_PALETTE8:
				// 8-bit palette
				return width * height;
				
			case sTexture::TEXTURE_FORMAT_PALETTE4:
				// 4-bit palette
				return (width * height + 1) / 2;
				
			default:
				// Default to 32-bit
				return width * height * 4;
		}
	}
	
	// Multi-level mipmap calculation
	for( uint8 mip = 0; mip < mip_levels; mip++ )
	{
		uint16 mip_width = width >> mip;
		uint16 mip_height = height >> mip;
		
		if( mip_width == 0 ) mip_width = 1;
		if( mip_height == 0 ) mip_height = 1;
		
		uint32 mip_size = 0;
		
		switch( format )
		{
			case sTexture::TEXTURE_FORMAT_DXT1:
				// DXT1: 4x4 blocks, 8 bytes per block
				mip_size = ((mip_width + 3) / 4) * ((mip_height + 3) / 4) * 8;
				break;
				
			case sTexture::TEXTURE_FORMAT_DXT3:
			case sTexture::TEXTURE_FORMAT_DXT5:
				// DXT3/DXT5: 4x4 blocks, 16 bytes per block
				mip_size = ((mip_width + 3) / 4) * ((mip_height + 3) / 4) * 16;
				break;
				
			case sTexture::TEXTURE_FORMAT_A8R8G8B8:
				// 32-bit ARGB
				mip_size = mip_width * mip_height * 4;
				break;
				
			case sTexture::TEXTURE_FORMAT_R5G6B5:
			case sTexture::TEXTURE_FORMAT_A1R5G5B5:
			case sTexture::TEXTURE_FORMAT_A4R4G4B4:
				// 16-bit formats
				mip_size = mip_width * mip_height * 2;
				break;
				
			case sTexture::TEXTURE_FORMAT_RGBA32:
				// Console 32-bit RGBA
				mip_size = mip_width * mip_height * 4;
				break;
				
			case sTexture::TEXTURE_FORMAT_RGB24:
				// Console 24-bit RGB
				mip_size = mip_width * mip_height * 3;
				break;
				
			case sTexture::TEXTURE_FORMAT_PALETTE8:
				// 8-bit palette
				mip_size = mip_width * mip_height;
				break;
				
			case sTexture::TEXTURE_FORMAT_PALETTE4:
				// 4-bit palette
				mip_size = (mip_width * mip_height + 1) / 2;
				break;
				
			default:
				// Default to 32-bit
				mip_size = mip_width * mip_height * 4;
				break;
		}
		
		total_size += mip_size;
	}
	
	return total_size;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
sTexture* load_texture_pc_format( const char *p_filename )
{
	if( !p_filename || !pTextureTable )
	{
		return NULL;
	}
	
	// Calculate checksum for filename
	uint32 checksum = calculate_filename_checksum( p_filename );
	
	// Check if texture already loaded
	sTexture *p_texture = pTextureTable->GetItem( checksum );
	if( p_texture )
	{
		return p_texture;
	}
	
	// In a full implementation, this would:
	// 1. Open PC texture file (.img or .tex with PC header)
	// 2. Read PC-specific texture header containing:
	//    - DirectX format identifier (DXT1/DXT3/DXT5/A8R8G8B8/etc)
	//    - Width and height
	//    - Mipmap count
	//    - Compressed data size
	// 3. Allocate buffer and read compressed texture data
	// 4. Create Vulkan image with appropriate format:
	//    - DXT1 -> VK_FORMAT_BC1_RGB_UNORM_BLOCK or VK_FORMAT_BC1_RGBA_UNORM_BLOCK
	//    - DXT3 -> VK_FORMAT_BC2_UNORM_BLOCK
	//    - DXT5 -> VK_FORMAT_BC3_UNORM_BLOCK
	//    - A8R8G8B8 -> VK_FORMAT_B8G8R8A8_UNORM
	// 5. Upload compressed data directly to GPU (no decompression needed)
	// 6. Create image view and sampler
	
	// For now, create a stub texture with PC format flag
	p_texture = new sTexture;
	memset( p_texture, 0, sizeof(sTexture) );
	p_texture->Checksum = checksum;
	p_texture->format = sTexture::TEXTURE_FORMAT_DXT1; // Default to DXT1 for PC
	
	pTextureTable->PutItem( checksum, p_texture );
	
	return p_texture;
}

/******************************************************************/
/*                                                                */
/* Mesh Management                                                */
/*                                                                */
/******************************************************************/
sMesh* create_mesh( uint32 checksum )
{
	sMesh *p_mesh = new sMesh;
	memset( p_mesh, 0, sizeof(sMesh) );
	
	p_mesh->Checksum = checksum;
	p_mesh->m_flags = sMesh::MESH_FLAG_ACTIVE;
	p_mesh->m_visibility_mask = 0xFFFFFFFF;
	
	return p_mesh;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void destroy_mesh( sMesh *p_mesh )
{
	if( !p_mesh )
	{
		return;
	}
	
#ifdef VULKAN_AVAILABLE
	// 1. Destroy Vulkan vertex buffer
	if( p_mesh->pVulkanVertexBuffer && g_vulkan_state.device )
	{
		BufferInfo* vertex_buffer = (BufferInfo*)p_mesh->pVulkanVertexBuffer;
		if( vertex_buffer->buffer != VK_NULL_HANDLE )
		{
			vkDestroyBuffer(g_vulkan_state.device, vertex_buffer->buffer, NULL);
		}
		if( vertex_buffer->memory != VK_NULL_HANDLE )
		{
			vkFreeMemory(g_vulkan_state.device, vertex_buffer->memory, NULL);
		}
		delete vertex_buffer;
	}
	
	// 2. Destroy Vulkan index buffer
	if( p_mesh->pVulkanIndexBuffer && g_vulkan_state.device )
	{
		BufferInfo* index_buffer = (BufferInfo*)p_mesh->pVulkanIndexBuffer;
		if( index_buffer->buffer != VK_NULL_HANDLE )
		{
			vkDestroyBuffer(g_vulkan_state.device, index_buffer->buffer, NULL);
		}
		if( index_buffer->memory != VK_NULL_HANDLE )
		{
			vkFreeMemory(g_vulkan_state.device, index_buffer->memory, NULL);
		}
		delete index_buffer;
	}
#endif
	
	if( p_mesh->mp_positions )
	{
		delete[] p_mesh->mp_positions;
	}
	if( p_mesh->mp_normals )
	{
		delete[] p_mesh->mp_normals;
	}
	if( p_mesh->mp_uvs )
	{
		delete[] p_mesh->mp_uvs;
	}
	if( p_mesh->mp_colors )
	{
		delete[] p_mesh->mp_colors;
	}
	if( p_mesh->mp_indices )
	{
		delete[] p_mesh->mp_indices;
	}
	
	delete p_mesh;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void upload_mesh_data( sMesh *p_mesh )
{
	if( !p_mesh )
	{
		return;
	}
	
#ifdef VULKAN_AVAILABLE
	if( !g_vulkan_state.device || !p_mesh->mp_positions )
	{
		return;
	}
	
	// 1-3. Create and upload vertex buffer
	VkDeviceSize vertex_buffer_size = p_mesh->m_num_vertices * 3 * sizeof(float); // positions only for simplicity
	
	if( vertex_buffer_size > 0 )
	{
		// Create staging buffer
		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		
		if( !create_buffer(vertex_buffer_size, 
						   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						   &staging_buffer, &staging_buffer_memory) )
		{
			printf("Failed to create vertex staging buffer\n");
			return;
		}
		
		// Map memory and copy vertex data
		void* data;
		vkMapMemory(g_vulkan_state.device, staging_buffer_memory, 0, vertex_buffer_size, 0, &data);
		memcpy(data, p_mesh->mp_positions, (size_t)vertex_buffer_size);
		vkUnmapMemory(g_vulkan_state.device, staging_buffer_memory);
		
		// Create device-local vertex buffer
		BufferInfo* vertex_buffer_info = new BufferInfo();
		if( !create_buffer(vertex_buffer_size,
						   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
						   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						   &vertex_buffer_info->buffer, &vertex_buffer_info->memory) )
		{
			printf("Failed to create device vertex buffer\n");
			vkDestroyBuffer(g_vulkan_state.device, staging_buffer, NULL);
			vkFreeMemory(g_vulkan_state.device, staging_buffer_memory, NULL);
			delete vertex_buffer_info;
			return;
		}
		
		// Copy from staging to device buffer
		copy_buffer(staging_buffer, vertex_buffer_info->buffer, vertex_buffer_size);
		
		// Cleanup staging buffer
		vkDestroyBuffer(g_vulkan_state.device, staging_buffer, NULL);
		vkFreeMemory(g_vulkan_state.device, staging_buffer_memory, NULL);
		
		p_mesh->pVulkanVertexBuffer = vertex_buffer_info;
		
		printf("Mesh vertex buffer uploaded (size: %lu bytes)\n", (unsigned long)vertex_buffer_size);
	}
	
	// 4-5. Create and upload index buffer
	if( p_mesh->mp_indices && p_mesh->m_num_indices > 0 )
	{
		VkDeviceSize index_buffer_size = p_mesh->m_num_indices * sizeof(uint16);
		
		// Create staging buffer
		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		
		if( !create_buffer(index_buffer_size,
						   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						   &staging_buffer, &staging_buffer_memory) )
		{
			printf("Failed to create index staging buffer\n");
			return;
		}
		
		// Map memory and copy index data
		void* data;
		vkMapMemory(g_vulkan_state.device, staging_buffer_memory, 0, index_buffer_size, 0, &data);
		memcpy(data, p_mesh->mp_indices, (size_t)index_buffer_size);
		vkUnmapMemory(g_vulkan_state.device, staging_buffer_memory);
		
		// Create device-local index buffer
		BufferInfo* index_buffer_info = new BufferInfo();
		if( !create_buffer(index_buffer_size,
						   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
						   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						   &index_buffer_info->buffer, &index_buffer_info->memory) )
		{
			printf("Failed to create device index buffer\n");
			vkDestroyBuffer(g_vulkan_state.device, staging_buffer, NULL);
			vkFreeMemory(g_vulkan_state.device, staging_buffer_memory, NULL);
			delete index_buffer_info;
			return;
		}
		
		// Copy from staging to device buffer
		copy_buffer(staging_buffer, index_buffer_info->buffer, index_buffer_size);
		
		// Cleanup staging buffer
		vkDestroyBuffer(g_vulkan_state.device, staging_buffer, NULL);
		vkFreeMemory(g_vulkan_state.device, staging_buffer_memory, NULL);
		
		p_mesh->pVulkanIndexBuffer = index_buffer_info;
		
		printf("Mesh index buffer uploaded (size: %lu bytes)\n", (unsigned long)index_buffer_size);
	}
#else
	// Stub implementation - just mark as uploaded
	if( p_mesh->mp_positions )
	{
		p_mesh->pVulkanVertexBuffer = (void*)1; // Non-null placeholder
	}
	if( p_mesh->mp_indices )
	{
		p_mesh->pVulkanIndexBuffer = (void*)1; // Non-null placeholder
	}
#endif
}

/******************************************************************/
/*                                                                */
/* Scene Management                                               */
/*                                                                */
/******************************************************************/
sScene* create_scene( void )
{
	sScene *p_scene = new sScene;
	memset( p_scene, 0, sizeof(sScene) );
	
	p_scene->m_flags = 0;
	p_scene->m_num_meshes = 0;
	p_scene->mpp_mesh_list = NULL;
	
	return p_scene;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void destroy_scene( sScene *p_scene )
{
	if( !p_scene )
	{
		return;
	}
	
	if( p_scene->mpp_mesh_list )
	{
		delete[] p_scene->mpp_mesh_list;
	}
	
	delete p_scene;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void add_mesh_to_scene( sScene *p_scene, sMesh *p_mesh )
{
	if( !p_scene || !p_mesh )
	{
		return;
	}
	
	// Use realloc-style approach for better performance with multiple additions
	// Allocate mesh list with grow-by-factor strategy
	sMesh **new_list = new sMesh*[p_scene->m_num_meshes + 1];
	
	if( p_scene->mpp_mesh_list )
	{
		// Use memcpy for efficient bulk copy
		memcpy( new_list, p_scene->mpp_mesh_list, p_scene->m_num_meshes * sizeof(sMesh*) );
		delete[] p_scene->mpp_mesh_list;
	}
	
	new_list[p_scene->m_num_meshes] = p_mesh;
	p_scene->mpp_mesh_list = new_list;
	p_scene->m_num_meshes++;
}

} // namespace NxVulcan
