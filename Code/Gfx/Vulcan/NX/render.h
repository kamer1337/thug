#ifndef __RENDER_H
#define __RENDER_H

#include <core/math.h>
#include <core/math/geometry.h>

#define		RS_ZWRITEENABLE			1
#define		RS_ZTESTENABLE			2
#define		RS_ALPHACUTOFF			3
#define		RS_UVADDRESSMODE0		4
#define		RS_UVADDRESSMODE1		5
#define		RS_UVADDRESSMODE2		6
#define		RS_UVADDRESSMODE3		7

namespace NxVulcan
{
	struct sTextureProjectionDetails
	{
		// Placeholder for Vulkan texture projection details
		void*		p_texture;
		void*		p_model;
		void*		p_scene;
	};

	extern void* pTextureProjectionDetailsTable;

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
	
	void		create_texture_projection_details( void *p_texture, void *p_model, void *p_scene );
	void		destroy_texture_projection_details( void *p_texture );
	void		set_texture_projection_camera( void *p_texture, Mth::Vector *p_pos, Mth::Vector *p_at );
	
	void		set_camera( Mth::Matrix *p_matrix, Mth::Vector *p_position, float screen_angle, float aspect_ratio, bool render_at_infinity = false );
	void		set_frustum_bbox_transform( Mth::Matrix *p_transform );
	bool		frustum_check_sphere( Mth::Vector *p_center, float radius );
	float		get_bounding_sphere_nearest_z( void );
	bool		IsVisible( Mth::Vector &center, float radius );
	void		render_shadow_targets();
	void		render_light_glows( bool test );
	void		render_scene( void *p_scene, uint32 flags = ( vRENDER_OPAQUE | vRENDER_SEMITRANSPARENT ), uint32 viewport = 0 );

} // namespace NxVulcan

#endif // __RENDER_H
