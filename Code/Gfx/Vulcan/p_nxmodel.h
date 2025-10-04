#ifndef __GFX_P_NX_MODEL_H
#define __GFX_P_NX_MODEL_H

#include <core/defines.h>
#include <core/math.h>
#include <gfx/nxmodel.h>
#include "NX/render.h"

namespace Nx
{

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
	
/////////////////////////////////////////////////////////////////////////////////////
//
// Here's a Vulkan-specific implementation of the CModel
    
class CVulcanModel : public CModel
{
public:
					CVulcanModel();
	virtual 		~CVulcanModel();

	NxVulcan::sScene*	GetScene() { return mp_scene; }
	void				SetScene( NxVulcan::sScene * p ) { mp_scene = p; }
	
private:				// Platform-specific implementation
	bool			plat_init_skeleton( int numBones );
	
	bool			plat_prepare_materials( void );
	bool			plat_refresh_materials( void );
	
	Mth::Vector 	plat_get_bounding_sphere();
	void			plat_set_bounding_sphere( const Mth::Vector& boundingSphere );

	NxVulcan::sScene*	mp_scene;
};

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
	
} // namespace Nx

#endif // __GFX_P_NX_MODEL_H
