#ifndef __GFX_DIRECTX_P_NX_MODEL_H
#define __GFX_DIRECTX_P_NX_MODEL_H

#include <core/defines.h>
#include <core/math.h>
#include <gfx/nxmodel.h>

namespace Nx
{

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
	
/////////////////////////////////////////////////////////////////////////////////////
//
// DirectX-specific implementation of the CModel
// This is a stub implementation - requires DirectX API integration
    
class CDirectXModel : public CModel
{
public:
					CDirectXModel();
	virtual 		~CDirectXModel();
	
private:				// Platform-specific implementation
	bool			plat_init_skeleton( int numBones );
	
	bool			plat_prepare_materials( void );
	bool			plat_refresh_materials( void );
	
	Mth::Vector 	plat_get_bounding_sphere();
	void			plat_set_bounding_sphere( const Mth::Vector& boundingSphere );

	// DirectX-specific data members would go here
	// For example: IDirect3DDevice9* mp_device;
	// For full implementation, see Code/Gfx/DirectX/README.md
};

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
	
} // namespace Nx

#endif // __GFX_DIRECTX_P_NX_MODEL_H
