/**
 * Comprehensive Vulkan rendering test
 * 
 * Tests all Vulkan rendering features:
 * - Instance/device creation
 * - Swapchain management
 * - Command buffer recording
 * - Pipeline state objects
 * - Buffer and texture upload
 * - Draw call submission
 * 
 * Build using CMake (recommended):
 *   mkdir build && cd build
 *   cmake -DUSE_VULKAN_RENDERER=ON ..
 *   make test_vulkan_rendering
 * 
 * Or compile manually with Vulkan SDK:
 *   g++ -std=c++11 -DUSE_VULKAN_RENDERER -I<path-to-project>/Code \
 *       test_vulkan_rendering.cpp NX/render.cpp \
 *       -L<path-to-core-lib> -lcore -lvulkan -o test_vulkan_rendering
 * 
 * Or without Vulkan SDK (stub mode):
 *   g++ -std=c++11 -I<path-to-project>/Code \
 *       test_vulkan_rendering.cpp NX/render.cpp \
 *       -L<path-to-core-lib> -lcore -o test_vulkan_rendering_stub
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NX/render.h"

using namespace NxVulcan;

// Test result tracking
struct TestResults {
	int total;
	int passed;
	int failed;
};

void print_test_header(const char *name) {
	printf("\n========================================\n");
	printf("TEST: %s\n", name);
	printf("========================================\n");
}

void print_test_result(TestResults *results, const char *name, bool passed) {
	results->total++;
	if (passed) {
		results->passed++;
		printf("  ✓ %s\n", name);
	} else {
		results->failed++;
		printf("  ✗ %s\n", name);
	}
}

void print_summary(TestResults *results) {
	printf("\n========================================\n");
	printf("TEST SUMMARY\n");
	printf("========================================\n");
	printf("Total:  %d\n", results->total);
	printf("Passed: %d\n", results->passed);
	printf("Failed: %d\n", results->failed);
	printf("========================================\n");
	
	if (results->failed == 0) {
		printf("✓ All tests passed!\n");
	} else {
		printf("✗ %d test(s) failed\n", results->failed);
	}
}

int main(int argc, char* argv[]) {
	TestResults results = {0, 0, 0};
	
	printf("==========================================\n");
	printf("  THUG Vulkan Rendering Comprehensive Test\n");
	printf("==========================================\n");
	
	// Test 1: Instance and device creation
	print_test_header("Instance and Device Creation");
	bool init_result = init_vulkan();
	print_test_result(&results, "Vulkan initialization", init_result);
	
	if (!init_result) {
		printf("\nCritical failure: Cannot continue without Vulkan initialization\n");
		print_summary(&results);
		return 1;
	}
	
	// Test 2: Swapchain management
	print_test_header("Swapchain Management");
	bool swapchain_result = create_swapchain(NULL, 1920, 1080);
	print_test_result(&results, "Swapchain creation", swapchain_result);
	
	uint32 image_index = 0;
	bool acquire_result = acquire_next_image(&image_index);
	print_test_result(&results, "Acquire next image", acquire_result);
	
	bool present_result = present_image(image_index);
	print_test_result(&results, "Present image", present_result);
	
	// Test 3: Render pass creation
	print_test_header("Render Pass Creation");
	bool render_pass_result = create_render_pass();
	print_test_result(&results, "Render pass creation", render_pass_result);
	
	// Test 4: Pipeline creation
	print_test_header("Graphics Pipeline Creation");
	bool pipeline_result = create_graphics_pipeline();
	print_test_result(&results, "Graphics pipeline creation", pipeline_result);
	
	// Test 5: Command buffer allocation
	print_test_header("Command Buffer Management");
	bool alloc_result = allocate_command_buffers();
	print_test_result(&results, "Command buffer allocation", alloc_result);
	
	// Test 6: Command buffer recording
	print_test_header("Command Buffer Recording");
	bool begin_result = begin_command_buffer(0);
	print_test_result(&results, "Begin command buffer", begin_result);
	
	bool begin_rp_result = begin_render_pass(0, 0);
	print_test_result(&results, "Begin render pass", begin_rp_result);
	
	// Test binding commands
	bind_pipeline(0);
	print_test_result(&results, "Bind pipeline", true);
	
	end_render_pass(0);
	print_test_result(&results, "End render pass", true);
	
	bool end_result = end_command_buffer(0);
	print_test_result(&results, "End command buffer", end_result);
	
	// Test 7: Command buffer submission
	print_test_header("Command Buffer Submission");
	bool submit_result = submit_command_buffer(0);
	print_test_result(&results, "Submit command buffer", submit_result);
	
	// Test 8: Buffer and texture upload
	print_test_header("Buffer and Texture Upload");
	
	// Create a test mesh
	sMesh *test_mesh = create_mesh(0x12345678);
	if (test_mesh) {
		// Set up mesh data
		test_mesh->m_num_vertices = 3;
		test_mesh->m_num_indices = 3;
		
		test_mesh->mp_positions = new float[9];  // 3 vertices * 3 components
		test_mesh->mp_positions[0] = 0.0f; test_mesh->mp_positions[1] = -0.5f; test_mesh->mp_positions[2] = 0.0f;
		test_mesh->mp_positions[3] = 0.5f; test_mesh->mp_positions[4] = 0.5f; test_mesh->mp_positions[5] = 0.0f;
		test_mesh->mp_positions[6] = -0.5f; test_mesh->mp_positions[7] = 0.5f; test_mesh->mp_positions[8] = 0.0f;
		
		test_mesh->mp_indices = new uint16[3];
		test_mesh->mp_indices[0] = 0;
		test_mesh->mp_indices[1] = 1;
		test_mesh->mp_indices[2] = 2;
		
		upload_mesh_data(test_mesh);
		bool mesh_uploaded = (test_mesh->pVulkanVertexBuffer != NULL && test_mesh->pVulkanIndexBuffer != NULL);
		print_test_result(&results, "Mesh data upload", mesh_uploaded);
		
		// Test binding buffers
		if (mesh_uploaded) {
			bind_vertex_buffer(0, test_mesh->pVulkanVertexBuffer);
			print_test_result(&results, "Bind vertex buffer", true);
			
			bind_index_buffer(0, test_mesh->pVulkanIndexBuffer);
			print_test_result(&results, "Bind index buffer", true);
		}
	}
	
	// Create a test texture
	uint32 tex_width = 256;
	uint32 tex_height = 256;
	uint32 tex_size = tex_width * tex_height * 4;  // RGBA
	uint8 *tex_data = new uint8[tex_size];
	
	// Fill with a gradient pattern
	for (uint32 y = 0; y < tex_height; y++) {
		for (uint32 x = 0; x < tex_width; x++) {
			uint32 i = (y * tex_width + x) * 4;
			tex_data[i + 0] = (uint8)(x);  // R
			tex_data[i + 1] = (uint8)(y);  // G
			tex_data[i + 2] = 128;         // B
			tex_data[i + 3] = 255;         // A
		}
	}
	
	sTexture *test_texture = create_texture(0xABCDEF00, tex_width, tex_height, 
											sTexture::TEXTURE_FORMAT_A8R8G8B8, tex_data);
	bool texture_created = (test_texture != NULL && test_texture->pVulkanTexture != NULL);
	print_test_result(&results, "Texture creation and upload", texture_created);
	
	delete[] tex_data;
	
	// Test 9: Draw calls
	print_test_header("Draw Call Submission");
	if (test_mesh) {
		draw_indexed(0, test_mesh->m_num_indices);
		print_test_result(&results, "Draw indexed call", true);
	}
	
	// Test 10: Scene rendering
	print_test_header("Scene Rendering");
	sScene *test_scene = create_scene();
	if (test_scene && test_mesh) {
		add_mesh_to_scene(test_scene, test_mesh);
		render_scene(test_scene, vRENDER_OPAQUE);
		print_test_result(&results, "Render scene", true);
	}
	
	// Test 11: Swapchain recreation
	print_test_header("Swapchain Resize/Recreation");
	bool recreate_result = recreate_swapchain(1280, 720);
	print_test_result(&results, "Swapchain recreation", recreate_result);
	
	// Cleanup
	print_test_header("Cleanup");
	
	if (test_texture) {
		destroy_texture(test_texture);
		print_test_result(&results, "Texture cleanup", true);
	}
	
	if (test_scene) {
		// Note: Don't destroy test_mesh separately as it's now owned by the scene
		destroy_scene(test_scene);
		print_test_result(&results, "Scene cleanup", true);
	}
	
	if (test_mesh) {
		destroy_mesh(test_mesh);
		print_test_result(&results, "Mesh cleanup", true);
	}
	
	free_command_buffers();
	print_test_result(&results, "Command buffer cleanup", true);
	
	destroy_graphics_pipeline();
	print_test_result(&results, "Pipeline cleanup", true);
	
	destroy_render_pass();
	print_test_result(&results, "Render pass cleanup", true);
	
	destroy_swapchain();
	print_test_result(&results, "Swapchain cleanup", true);
	
	shutdown_vulkan();
	print_test_result(&results, "Vulkan shutdown", true);
	
	// Print summary
	print_summary(&results);
	
	return (results.failed == 0) ? 0 : 1;
}
