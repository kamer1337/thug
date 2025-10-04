#include <core/defines.h>
#include <core/debug.h>
#include <core/HashTable.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "render.h"

namespace NxVulcan
{

// Global state for Vulkan renderer
Lst::HashTable< sTextureProjectionDetails > *pTextureProjectionDetailsTable = NULL;
Lst::HashTable< sTexture > *pTextureTable = NULL;

// Vulkan state tracking
static struct VulkanState
{
	bool initialized;
	Mth::Matrix view_matrix;
	Mth::Matrix projection_matrix;
	Mth::Matrix frustum_transform;
	uint32 current_blend_mode;
	uint32 render_state[8];
} g_vulkan_state = { false };

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
	
	// In a full implementation, this would:
	// 1. Create Vulkan instance
	// 2. Select physical device
	// 3. Create logical device
	// 4. Create swapchain
	// 5. Create render passes
	// 6. Create command pools
	
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
	
	g_vulkan_state.initialized = false;
	
	// In a full implementation, this would:
	// 1. Destroy command pools
	// 2. Destroy render passes
	// 3. Destroy swapchain
	// 4. Destroy logical device
	// 5. Destroy Vulkan instance
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
BlendModes GetBlendMode( uint32 blend_checksum )
{
	// Map blend mode checksums to blend modes
	// This maintains compatibility with existing file formats
	switch( blend_checksum )
	{
		case 0x872b0e99:	// "Diffuse"
			return vBLEND_MODE_DIFFUSE;
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
	
	// In a full implementation, this would:
	// 1. Begin command buffer recording
	// 2. Begin render pass
	// 3. Set viewport and scissor
	// 4. Bind pipeline state
	
	// Iterate through meshes and render based on flags
	for( int i = 0; i < p_scene->m_num_meshes; i++ )
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
		
		// In a full implementation, for each mesh:
		// 1. Bind vertex and index buffers
		// 2. Bind material/textures
		// 3. Update push constants or descriptor sets
		// 4. Draw indexed
	}
	
	// 5. End render pass
	// 6. End command buffer recording
	// 7. Submit command buffer to queue
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
	uint32 checksum = 0;
	for( const char *p = p_filename; *p; p++ )
	{
		checksum = (checksum << 5) + checksum + *p;
	}
	
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
		
		// In a full implementation:
		// 1. Create Vulkan image with specified format
		//    - For DXT formats, use BC1/BC2/BC3 compressed formats
		//    - For uncompressed formats, use appropriate VkFormat
		// 2. Allocate device memory
		// 3. Upload texture data (compressed data stays compressed)
		// 4. Generate mipmaps if needed
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
	
	// In a full implementation:
	// 1. Destroy Vulkan image view
	// 2. Destroy Vulkan image
	// 3. Free device memory
	
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
sTexture* get_texture( uint32 checksum )
{
	if( !pTextureTable )
	{
		return NULL;
	}
	
	return pTextureTable->GetItem( checksum );
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
uint32 get_texture_size_for_format( uint16 width, uint16 height, uint8 format, uint8 mip_levels )
{
	uint32 total_size = 0;
	
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
	uint32 checksum = 0;
	for( const char *p = p_filename; *p; p++ )
	{
		checksum = (checksum << 5) + checksum + *p;
	}
	
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
	
	// In a full implementation:
	// 1. Destroy Vulkan vertex buffer
	// 2. Destroy Vulkan index buffer
	// 3. Free device memory
	
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
	
	// In a full implementation:
	// 1. Create staging buffer
	// 2. Copy vertex data to staging buffer
	// 3. Create device-local vertex buffer
	// 4. Copy from staging to device buffer
	// 5. Create index buffer similarly
	// 6. Destroy staging buffers
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
	
	// Allocate or reallocate mesh list
	sMesh **new_list = new sMesh*[p_scene->m_num_meshes + 1];
	
	if( p_scene->mpp_mesh_list )
	{
		memcpy( new_list, p_scene->mpp_mesh_list, p_scene->m_num_meshes * sizeof(sMesh*) );
		delete[] p_scene->mpp_mesh_list;
	}
	
	new_list[p_scene->m_num_meshes] = p_mesh;
	p_scene->mpp_mesh_list = new_list;
	p_scene->m_num_meshes++;
}

} // namespace NxVulcan
