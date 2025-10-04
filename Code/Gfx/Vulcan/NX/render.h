#ifndef __RENDER_H
#define __RENDER_H

#include <core/math.h>
#include <core/math/geometry.h>
#include <core/HashTable.h>

#define		RS_ZWRITEENABLE			1
#define		RS_ZTESTENABLE			2
#define		RS_ALPHACUTOFF			3
#define		RS_UVADDRESSMODE0		4
#define		RS_UVADDRESSMODE1		5
#define		RS_UVADDRESSMODE2		6
#define		RS_UVADDRESSMODE3		7

namespace NxVulcan
{
	// Forward declarations
	struct sTexture;
	struct sMesh;
	struct sScene;
	struct sMaterial;
	class CVulcanModel;

	// Texture structure compatible with existing file formats
	struct sTexture
	{
		enum ETextureFlags
		{
			TEXTURE_FLAG_HAS_HOLES				= (1<<0),
			TEXTURE_FLAG_HAS_ALPHA				= (1<<1),
			TEXTURE_FLAG_CHANNEL_GREEN			= (1<<2),
			TEXTURE_FLAG_CHANNEL_RED			= (1<<3),
			TEXTURE_FLAG_CHANNEL_BLUE			= (1<<4),
			TEXTURE_FLAG_SINGLE_OWNER			= (1<<5),
			TEXTURE_FLAG_OLD_DATA				= (1<<6),
			TEXTURE_FLAG_REPLACED				= (1<<7),
			TEXTURE_FLAG_CHANNEL_MASK			= ( TEXTURE_FLAG_CHANNEL_GREEN | TEXTURE_FLAG_CHANNEL_RED | TEXTURE_FLAG_CHANNEL_BLUE )
		};

		uint32				Checksum;
		uint16				BaseWidth, BaseHeight;
		uint16				ActualWidth, ActualHeight;
		uint8				Levels;
		uint8				format;
		uint16				flags;
		uint32				byte_size;
		uint8*				pTexelData;
		uint8*				pAlphaData;
		void*				pVulkanTexture;		// Vulkan texture handle
	};

	// Material structure for texture passes
	struct sMaterial
	{
		uint32				Checksum;
		uint8				Passes;
		uint8				AlphaCutoff;
		uint16				Flags;
		sTexture*			pTex;
		uint8				blendMode;
		uint8				fixAlpha;
		uint8				UVAddressing;
		float				K;
	};

	// Mesh structure compatible with existing file formats
	struct sMesh
	{
		enum EMeshFlags
		{
			MESH_FLAG_IS_INSTANCE				= (1<<0),
			MESH_FLAG_ACTIVE					= (1<<1),
			MESH_FLAG_IS_CLONED					= (1<<2),
			MESH_FLAG_VISIBLE					= (1<<3),
		};

		uint32				Checksum;
		uint16				m_flags;
		uint32				m_visibility_mask;
		sMaterial*			mp_material;
		uint16				m_num_vertices;
		uint16				m_num_indices;
		float*				mp_positions;
		float*				mp_normals;
		float*				mp_uvs;
		uint32*				mp_colors;
		uint16*				mp_indices;
		void*				pVulkanVertexBuffer;	// Vulkan buffer handle
		void*				pVulkanIndexBuffer;		// Vulkan buffer handle
		Mth::CBBox			m_bbox;
		Mth::Vector			m_sphere;
	};

	// Scene structure
	struct sScene
	{
		uint32				m_flags;
		uint16				m_num_meshes;
		sMesh**				mpp_mesh_list;
		Mth::CBBox			m_bbox;
		Mth::Vector			m_sphere;
	};

	struct sTextureProjectionDetails
	{
		sTexture*			p_texture;
		CVulcanModel*		p_model;
		sScene*				p_scene;
		Mth::Matrix			view_matrix;
		Mth::Matrix			projection_matrix;
		Mth::Matrix			texture_projection_matrix;
	};

	extern Lst::HashTable< sTextureProjectionDetails > *pTextureProjectionDetailsTable;
	extern Lst::HashTable< sTexture > *pTextureTable;

	typedef enum
	{
		vBLEND_MODE_DIFFUSE,						// ( 0 - 0 ) * 0 + Src
		vBLEND_MODE_ADD,							// ( Src - 0 ) * Src + Dst
		vBLEND_MODE_ADD_FIXED,						// ( Src - 0 ) * Fixed + Dst
		vBLEND_MODE_SUBTRACT,						// ( 0 - Src ) * Src + Dst
		vBLEND_MODE_SUB_FIXED,						// ( 0 - Src ) * Fixed + Dst
		vBLEND_MODE_BLEND,							// ( Src * Dst ) * Src + Dst	
		vBLEND_MODE_BLEND_FIXED,					// ( Src * Dst ) * Fixed + Dst	
		vBLEND_MODE_MODULATE,						// ( Dst - 0 ) * Src + 0
		vBLEND_MODE_MODULATE_FIXED,					// ( Dst - 0 ) * Fixed + 0	
		vBLEND_MODE_BRIGHTEN,						// ( Dst - 0 ) * Src + Dst
		vBLEND_MODE_BRIGHTEN_FIXED,					// ( Dst - 0 ) * Fixed + Dst	
		vBLEND_MODE_GLOSS_MAP,						// Specular = Specular * Src	- special mode for gloss mapping
		vBLEND_MODE_BLEND_PREVIOUS_MASK,			// ( Src - Dst ) * Dst + Dst
		vBLEND_MODE_BLEND_INVERSE_PREVIOUS_MASK,	// ( Dst - Src ) * Dst + Src

		vNUM_BLEND_MODES
	} BlendModes; 

	typedef enum
	{
		vRENDER_OPAQUE								= 1,						
		vRENDER_SEMITRANSPARENT						= 2,
		vRENDER_OCCLUDED							= 4,
		vRENDER_NO_CULLING							= 8,
		vRENDER_SORT_FRONT_TO_BACK					= 16,
		vRENDER_SHADOW_VOLUMES						= 32,
		vRENDER_BILLBOARDS							= 64,
		vRENDER_INSTANCE_PRE_WORLD_SEMITRANSPARENT	= 128,
		vRENDER_INSTANCE_POST_WORLD_SEMITRANSPARENT	= 256,
	} SceneRenderFlags; 

	BlendModes	GetBlendMode( uint32 blend_checksum );

	void		set_render_state( uint32 type, uint32 state );
	void		set_blend_mode( uint32 mode );
	
	void		create_texture_projection_details( sTexture *p_texture, CVulcanModel *p_model, sScene *p_scene );
	void		destroy_texture_projection_details( sTexture *p_texture );
	void		set_texture_projection_camera( sTexture *p_texture, Mth::Vector *p_pos, Mth::Vector *p_at );
	
	void		set_camera( Mth::Matrix *p_matrix, Mth::Vector *p_position, float screen_angle, float aspect_ratio, bool render_at_infinity = false );
	void		set_frustum_bbox_transform( Mth::Matrix *p_transform );
	bool		frustum_check_sphere( Mth::Vector *p_center, float radius );
	float		get_bounding_sphere_nearest_z( void );
	bool		IsVisible( Mth::Vector &center, float radius );
	void		render_shadow_targets();
	void		render_light_glows( bool test );
	void		render_scene( sScene *p_scene, uint32 flags = ( vRENDER_OPAQUE | vRENDER_SEMITRANSPARENT ), uint32 viewport = 0 );

	// Vulkan-specific initialization and management
	bool		init_vulkan( void );
	void		shutdown_vulkan( void );
	
	// Texture management
	sTexture*	load_texture( const char *p_filename );
	sTexture*	create_texture( uint32 checksum, uint16 width, uint16 height, uint8 format, uint8* pData );
	void		destroy_texture( sTexture *p_texture );
	sTexture*	get_texture( uint32 checksum );
	
	// Mesh management
	sMesh*		create_mesh( uint32 checksum );
	void		destroy_mesh( sMesh *p_mesh );
	void		upload_mesh_data( sMesh *p_mesh );
	
	// Scene management
	sScene*		create_scene( void );
	void		destroy_scene( sScene *p_scene );
	void		add_mesh_to_scene( sScene *p_scene, sMesh *p_mesh );

} // namespace NxVulcan

#endif // __RENDER_H
