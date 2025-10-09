/*****************************************************************************
**																			**
**			              Neversoft Entertainment			                **
**																		   	**
**				   Copyright (C) 1999 - All Rights Reserved				   	**
**																			**
******************************************************************************
**																			**
**	Project:		GFX (Graphics Library)									**
**																			**
**	Module:			Graphics (DirectX) 										**
**																			**
**	File name:		p_nxmodel.cpp											**
**																			**
**	Created:		PC Port													**
**																			**
**	Description:	DirectX platform-specific model implementation stub	**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <core/defines.h>
#include <gfx/DirectX/p_nxmodel.h>

namespace Nx
{

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
CDirectXModel::CDirectXModel() : CModel()
{
	// DirectX stub - model initialization not implemented
	// In a full implementation, this would initialize DirectX resources
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
CDirectXModel::~CDirectXModel()
{
	// DirectX stub - cleanup not implemented
	// In a full implementation, this would release DirectX resources
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
bool CDirectXModel::plat_init_skeleton( int numBones )
{
	// DirectX stub - skeleton initialization not implemented
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
bool CDirectXModel::plat_prepare_materials( void )
{
	// DirectX stub - material preparation not implemented
	// In a full implementation, this would:
	// 1. Create DirectX materials for each mesh
	// 2. Set up textures
	// 3. Configure blend states
	
	return true;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
bool CDirectXModel::plat_refresh_materials( void )
{
	// DirectX stub - material refresh not implemented
	// Called when materials need to be updated
	
	return true;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
Mth::Vector CDirectXModel::plat_get_bounding_sphere()
{
	// DirectX stub - return default bounding sphere
	return Mth::Vector( 0.0f, 0.0f, 0.0f, 0.0f );
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
void CDirectXModel::plat_set_bounding_sphere( const Mth::Vector& boundingSphere )
{
	// DirectX stub - bounding sphere setting not implemented
	// In a full implementation, store the bounding sphere
}

} // namespace Nx
