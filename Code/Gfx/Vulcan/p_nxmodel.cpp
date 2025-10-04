#include <core/defines.h>
#include <gfx/Vulcan/p_nxmodel.h>

namespace Nx
{

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
CVulcanModel::CVulcanModel() : CModel()
{
	mp_scene = NULL;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
CVulcanModel::~CVulcanModel()
{
	if( mp_scene )
	{
		NxVulcan::destroy_scene( mp_scene );
		mp_scene = NULL;
	}
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
bool CVulcanModel::plat_init_skeleton( int numBones )
{
	// Initialize skeleton with specified number of bones
	// In a full implementation, this would:
	// 1. Allocate bone transformation matrices
	// 2. Set up skinning buffers
	// 3. Configure vertex shader for skeletal animation
	
	return true;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
bool CVulcanModel::plat_prepare_materials( void )
{
	// Prepare materials for rendering
	// In a full implementation, this would:
	// 1. Create Vulkan pipeline for each material
	// 2. Set up descriptor sets for textures
	// 3. Configure blend states
	
	return true;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
bool CVulcanModel::plat_refresh_materials( void )
{
	// Refresh material properties
	// Called when materials need to be updated
	
	return true;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
Mth::Vector CVulcanModel::plat_get_bounding_sphere()
{
	if( mp_scene )
	{
		return mp_scene->m_sphere;
	}
	
	return Mth::Vector( 0.0f, 0.0f, 0.0f, 0.0f );
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void CVulcanModel::plat_set_bounding_sphere( const Mth::Vector& boundingSphere )
{
	if( mp_scene )
	{
		mp_scene->m_sphere = boundingSphere;
	}
}

} // namespace Nx
