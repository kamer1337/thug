/****************************************************************************
 *
 * Example usage of the Vulcan (Vulkan) Renderer
 *
 * This file demonstrates how to use the Vulcan renderer API for rendering
 * 3D scenes. This is a standalone example showing the typical workflow.
 *
 * NOTE: This example shows the API usage. Actual GPU rendering requires
 * implementing the Vulkan operations marked as stubs in render.cpp
 *
 ****************************************************************************/

#include "NX/render.h"
#include "p_nxmodel.h"
#include <stdio.h>

namespace VulcanExample
{

/**
 * Example 1: Basic Initialization and Cleanup
 */
bool Example_BasicInitialization()
{
    printf("=== Example 1: Basic Initialization ===\n");
    
    // Initialize the Vulkan renderer
    if (!NxVulcan::init_vulkan())
    {
        printf("ERROR: Failed to initialize Vulkan renderer\n");
        return false;
    }
    printf("✓ Vulkan renderer initialized\n");
    
    // Do rendering work here...
    
    // Shutdown the renderer
    NxVulcan::shutdown_vulkan();
    printf("✓ Vulkan renderer shut down\n");
    
    return true;
}

/**
 * Example 2: Creating and Managing a Scene
 */
bool Example_SceneManagement()
{
    printf("\n=== Example 2: Scene Management ===\n");
    
    NxVulcan::init_vulkan();
    
    // Create a scene
    NxVulcan::sScene* pScene = NxVulcan::create_scene();
    printf("✓ Scene created (meshes: %d)\n", pScene->m_num_meshes);
    
    // Scene is now ready to have meshes added to it
    
    // Clean up
    NxVulcan::destroy_scene(pScene);
    printf("✓ Scene destroyed\n");
    
    NxVulcan::shutdown_vulkan();
    return true;
}

/**
 * Example 3: Creating a Simple Mesh
 */
bool Example_MeshCreation()
{
    printf("\n=== Example 3: Mesh Creation ===\n");
    
    NxVulcan::init_vulkan();
    
    // Create a mesh with a checksum identifier
    uint32 meshChecksum = 0x12345678;
    NxVulcan::sMesh* pMesh = NxVulcan::create_mesh(meshChecksum);
    printf("✓ Mesh created (checksum: 0x%08X)\n", meshChecksum);
    
    // Set up a simple triangle
    const int numVertices = 3;
    pMesh->m_num_vertices = numVertices;
    
    // Allocate vertex positions (3 floats per vertex: x, y, z)
    pMesh->mp_positions = new float[numVertices * 3];
    pMesh->mp_positions[0] = -1.0f; pMesh->mp_positions[1] = -1.0f; pMesh->mp_positions[2] = 0.0f; // Vertex 0
    pMesh->mp_positions[3] =  1.0f; pMesh->mp_positions[4] = -1.0f; pMesh->mp_positions[5] = 0.0f; // Vertex 1
    pMesh->mp_positions[6] =  0.0f; pMesh->mp_positions[7] =  1.0f; pMesh->mp_positions[8] = 0.0f; // Vertex 2
    printf("✓ Vertex positions set (%d vertices)\n", numVertices);
    
    // Allocate vertex normals (3 floats per vertex: nx, ny, nz)
    pMesh->mp_normals = new float[numVertices * 3];
    for (int i = 0; i < numVertices; i++)
    {
        pMesh->mp_normals[i * 3 + 0] = 0.0f;
        pMesh->mp_normals[i * 3 + 1] = 0.0f;
        pMesh->mp_normals[i * 3 + 2] = 1.0f; // All normals point up (Z+)
    }
    printf("✓ Vertex normals set\n");
    
    // Allocate UV coordinates (2 floats per vertex: u, v)
    pMesh->mp_uvs = new float[numVertices * 2];
    pMesh->mp_uvs[0] = 0.0f; pMesh->mp_uvs[1] = 0.0f; // Vertex 0 UVs
    pMesh->mp_uvs[2] = 1.0f; pMesh->mp_uvs[3] = 0.0f; // Vertex 1 UVs
    pMesh->mp_uvs[4] = 0.5f; pMesh->mp_uvs[5] = 1.0f; // Vertex 2 UVs
    printf("✓ UV coordinates set\n");
    
    // Allocate vertex colors (RGBA as uint32)
    pMesh->mp_colors = new uint32[numVertices];
    pMesh->mp_colors[0] = 0xFFFF0000; // Red
    pMesh->mp_colors[1] = 0xFF00FF00; // Green
    pMesh->mp_colors[2] = 0xFF0000FF; // Blue
    printf("✓ Vertex colors set\n");
    
    // Set up indices (one triangle = 3 indices)
    pMesh->m_num_indices = 3;
    pMesh->mp_indices = new uint16[3];
    pMesh->mp_indices[0] = 0;
    pMesh->mp_indices[1] = 1;
    pMesh->mp_indices[2] = 2;
    printf("✓ Indices set (%d indices)\n", pMesh->m_num_indices);
    
    // Upload mesh data to GPU (in a full implementation)
    NxVulcan::upload_mesh_data(pMesh);
    printf("✓ Mesh data uploaded to GPU\n");
    
    // Set mesh flags
    pMesh->m_flags = NxVulcan::sMesh::MESH_FLAG_ACTIVE | NxVulcan::sMesh::MESH_FLAG_VISIBLE;
    pMesh->m_visibility_mask = 0xFFFFFFFF;
    
    // Clean up
    NxVulcan::destroy_mesh(pMesh);
    printf("✓ Mesh destroyed\n");
    
    NxVulcan::shutdown_vulkan();
    return true;
}

/**
 * Example 4: Adding Meshes to a Scene
 */
bool Example_SceneWithMeshes()
{
    printf("\n=== Example 4: Scene with Meshes ===\n");
    
    NxVulcan::init_vulkan();
    
    // Create a scene
    NxVulcan::sScene* pScene = NxVulcan::create_scene();
    
    // Create multiple meshes
    NxVulcan::sMesh* pMesh1 = NxVulcan::create_mesh(0x11111111);
    NxVulcan::sMesh* pMesh2 = NxVulcan::create_mesh(0x22222222);
    NxVulcan::sMesh* pMesh3 = NxVulcan::create_mesh(0x33333333);
    
    // Add meshes to scene
    NxVulcan::add_mesh_to_scene(pScene, pMesh1);
    printf("✓ Added mesh 1 to scene (total meshes: %d)\n", pScene->m_num_meshes);
    
    NxVulcan::add_mesh_to_scene(pScene, pMesh2);
    printf("✓ Added mesh 2 to scene (total meshes: %d)\n", pScene->m_num_meshes);
    
    NxVulcan::add_mesh_to_scene(pScene, pMesh3);
    printf("✓ Added mesh 3 to scene (total meshes: %d)\n", pScene->m_num_meshes);
    
    // Meshes are now part of the scene
    // Note: Don't destroy meshes directly; they're owned by the scene
    
    // Clean up (this will clean up the meshes too if properly implemented)
    NxVulcan::destroy_mesh(pMesh1);
    NxVulcan::destroy_mesh(pMesh2);
    NxVulcan::destroy_mesh(pMesh3);
    NxVulcan::destroy_scene(pScene);
    
    NxVulcan::shutdown_vulkan();
    return true;
}

/**
 * Example 5: Texture Management
 */
bool Example_TextureManagement()
{
    printf("\n=== Example 5: Texture Management ===\n");
    
    NxVulcan::init_vulkan();
    
    // Create a simple 4x4 texture
    uint32 texChecksum = 0xABCD1234;
    uint16 width = 4;
    uint16 height = 4;
    uint8 format = NxVulcan::sTexture::TEXTURE_FORMAT_RGBA32;
    
    // Create texture data (4x4 RGBA = 16 pixels * 4 bytes = 64 bytes)
    uint8* pTexData = new uint8[64];
    for (int i = 0; i < 64; i++)
    {
        pTexData[i] = (i * 4) % 256; // Simple gradient pattern
    }
    
    // Create texture
    NxVulcan::sTexture* pTexture = NxVulcan::create_texture(
        texChecksum, width, height, format, pTexData
    );
    printf("✓ Texture created (checksum: 0x%08X, size: %dx%d)\n", 
           texChecksum, width, height);
    
    // Retrieve texture by checksum
    NxVulcan::sTexture* pRetrieved = NxVulcan::get_texture(texChecksum);
    if (pRetrieved == pTexture)
    {
        printf("✓ Texture retrieved successfully\n");
    }
    
    // Clean up
    NxVulcan::destroy_texture(pTexture);
    delete[] pTexData;
    printf("✓ Texture destroyed\n");
    
    NxVulcan::shutdown_vulkan();
    return true;
}

/**
 * Example 6: Camera Setup
 */
bool Example_CameraSetup()
{
    printf("\n=== Example 6: Camera Setup ===\n");
    
    NxVulcan::init_vulkan();
    
    // Set up camera position and orientation
    Mth::Matrix viewMatrix;
    viewMatrix.Ident();
    
    Mth::Vector cameraPos(0.0f, 5.0f, 10.0f, 1.0f);
    float fieldOfView = 60.0f; // degrees
    float aspectRatio = 16.0f / 9.0f; // widescreen
    
    // Configure camera
    NxVulcan::set_camera(&viewMatrix, &cameraPos, fieldOfView, aspectRatio);
    printf("✓ Camera configured (pos: %.1f, %.1f, %.1f, FOV: %.1f°, aspect: %.2f)\n",
           cameraPos[X], cameraPos[Y], cameraPos[Z], fieldOfView, aspectRatio);
    
    NxVulcan::shutdown_vulkan();
    return true;
}

/**
 * Example 7: Render States and Blend Modes
 */
bool Example_RenderStates()
{
    printf("\n=== Example 7: Render States ===\n");
    
    NxVulcan::init_vulkan();
    
    // Enable depth testing and writing
    NxVulcan::set_render_state(RS_ZTESTENABLE, 1);
    printf("✓ Depth testing enabled\n");
    
    NxVulcan::set_render_state(RS_ZWRITEENABLE, 1);
    printf("✓ Depth writing enabled\n");
    
    // Set alpha cutoff for alpha testing
    NxVulcan::set_render_state(RS_ALPHACUTOFF, 128);
    printf("✓ Alpha cutoff set to 128/255\n");
    
    // Set blend mode for transparency
    NxVulcan::set_blend_mode(NxVulcan::vBLEND_MODE_BLEND);
    printf("✓ Blend mode set to BLEND\n");
    
    NxVulcan::shutdown_vulkan();
    return true;
}

/**
 * Example 8: Complete Rendering Pipeline
 */
bool Example_CompleteRenderingPipeline()
{
    printf("\n=== Example 8: Complete Rendering Pipeline ===\n");
    
    // 1. Initialize renderer
    if (!NxVulcan::init_vulkan())
    {
        printf("ERROR: Failed to initialize renderer\n");
        return false;
    }
    printf("✓ Step 1: Renderer initialized\n");
    
    // 2. Create scene
    NxVulcan::sScene* pScene = NxVulcan::create_scene();
    printf("✓ Step 2: Scene created\n");
    
    // 3. Create and configure mesh
    NxVulcan::sMesh* pMesh = NxVulcan::create_mesh(0x99999999);
    pMesh->m_num_vertices = 3;
    pMesh->mp_positions = new float[9] { -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f };
    pMesh->m_flags = NxVulcan::sMesh::MESH_FLAG_ACTIVE | NxVulcan::sMesh::MESH_FLAG_VISIBLE;
    NxVulcan::upload_mesh_data(pMesh);
    printf("✓ Step 3: Mesh created and configured\n");
    
    // 4. Add mesh to scene
    NxVulcan::add_mesh_to_scene(pScene, pMesh);
    printf("✓ Step 4: Mesh added to scene\n");
    
    // 5. Create texture
    uint8 texData[64];
    NxVulcan::sTexture* pTexture = NxVulcan::create_texture(
        0x88888888, 4, 4, NxVulcan::sTexture::TEXTURE_FORMAT_RGBA32, texData
    );
    printf("✓ Step 5: Texture created\n");
    
    // 6. Set up camera
    Mth::Matrix viewMatrix;
    viewMatrix.Ident();
    Mth::Vector cameraPos(0.0f, 0.0f, 5.0f, 1.0f);
    NxVulcan::set_camera(&viewMatrix, &cameraPos, 60.0f, 16.0f/9.0f);
    printf("✓ Step 6: Camera configured\n");
    
    // 7. Configure render states
    NxVulcan::set_render_state(RS_ZTESTENABLE, 1);
    NxVulcan::set_render_state(RS_ZWRITEENABLE, 1);
    NxVulcan::set_blend_mode(NxVulcan::vBLEND_MODE_DIFFUSE);
    printf("✓ Step 7: Render states configured\n");
    
    // 8. Render the scene
    NxVulcan::render_scene(pScene, NxVulcan::vRENDER_OPAQUE | NxVulcan::vRENDER_SEMITRANSPARENT);
    printf("✓ Step 8: Scene rendered\n");
    
    // 9. Clean up
    NxVulcan::destroy_texture(pTexture);
    NxVulcan::destroy_mesh(pMesh);
    NxVulcan::destroy_scene(pScene);
    printf("✓ Step 9: Resources cleaned up\n");
    
    // 10. Shutdown renderer
    NxVulcan::shutdown_vulkan();
    printf("✓ Step 10: Renderer shut down\n");
    
    return true;
}

/**
 * Example 9: Frustum Culling
 */
bool Example_FrustumCulling()
{
    printf("\n=== Example 9: Frustum Culling ===\n");
    
    NxVulcan::init_vulkan();
    
    // Set up camera first
    Mth::Matrix viewMatrix;
    viewMatrix.Ident();
    Mth::Vector cameraPos(0.0f, 0.0f, 10.0f, 1.0f);
    NxVulcan::set_camera(&viewMatrix, &cameraPos, 60.0f, 16.0f/9.0f);
    
    // Test sphere visibility
    Mth::Vector sphere1Center(0.0f, 0.0f, 0.0f);
    float sphere1Radius = 1.0f;
    bool visible1 = NxVulcan::IsVisible(sphere1Center, sphere1Radius);
    printf("✓ Sphere 1 (center: 0,0,0, radius: 1.0) - Visible: %s\n", visible1 ? "YES" : "NO");
    
    Mth::Vector sphere2Center(1000.0f, 1000.0f, 1000.0f);
    float sphere2Radius = 1.0f;
    bool visible2 = NxVulcan::IsVisible(sphere2Center, sphere2Radius);
    printf("✓ Sphere 2 (center: 1000,1000,1000, radius: 1.0) - Visible: %s\n", visible2 ? "YES" : "NO");
    
    NxVulcan::shutdown_vulkan();
    return true;
}

/**
 * Example 10: Using CVulcanModel
 */
bool Example_VulcanModel()
{
    printf("\n=== Example 10: Using CVulcanModel ===\n");
    
    NxVulcan::init_vulkan();
    
    // Create a Vulcan model
    Nx::CVulcanModel* pModel = new Nx::CVulcanModel();
    printf("✓ CVulcanModel created\n");
    
    // Create a scene for the model
    NxVulcan::sScene* pScene = NxVulcan::create_scene();
    pModel->SetScene(pScene);
    printf("✓ Scene assigned to model\n");
    
    // Set bounding sphere
    Mth::Vector boundingSphere(0.0f, 0.0f, 0.0f, 5.0f); // Center at origin, radius 5
    pModel->plat_set_bounding_sphere(boundingSphere);
    printf("✓ Bounding sphere set\n");
    
    // Get bounding sphere
    Mth::Vector retrieved = pModel->plat_get_bounding_sphere();
    printf("✓ Bounding sphere retrieved (radius: %.1f)\n", retrieved[W]);
    
    // Clean up (scene is destroyed by model destructor)
    delete pModel;
    printf("✓ Model destroyed\n");
    
    NxVulcan::shutdown_vulkan();
    return true;
}

} // namespace VulcanExample

/**
 * Main function to run all examples
 */
int main(int argc, char* argv[])
{
    printf("================================================\n");
    printf("  Vulcan Renderer Example Programs\n");
    printf("================================================\n\n");
    
    printf("NOTE: These examples demonstrate API usage.\n");
    printf("      Actual GPU rendering requires implementing\n");
    printf("      the Vulkan operations in render.cpp\n\n");
    
    // Run all examples
    VulcanExample::Example_BasicInitialization();
    VulcanExample::Example_SceneManagement();
    VulcanExample::Example_MeshCreation();
    VulcanExample::Example_SceneWithMeshes();
    VulcanExample::Example_TextureManagement();
    VulcanExample::Example_CameraSetup();
    VulcanExample::Example_RenderStates();
    VulcanExample::Example_CompleteRenderingPipeline();
    VulcanExample::Example_FrustumCulling();
    VulcanExample::Example_VulcanModel();
    
    printf("\n================================================\n");
    printf("  All examples completed successfully!\n");
    printf("================================================\n");
    
    return 0;
}
