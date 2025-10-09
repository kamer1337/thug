#ifndef __GFX_OPENGL_P_NX_MODEL_H
#define __GFX_OPENGL_P_NX_MODEL_H

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
// OpenGL-specific implementation of the CModel
// This is a stub implementation - requires OpenGL API integration
    
class COpenGLModel : public CModel
{
public:
					COpenGLModel();
	virtual 		~COpenGLModel();
	
private:				// Platform-specific implementation
	bool			plat_init_skeleton( int numBones );
	
	bool			plat_prepare_materials( void );
	bool			plat_refresh_materials( void );
	
	Mth::Vector 	plat_get_bounding_sphere();
	void			plat_set_bounding_sphere( const Mth::Vector& boundingSphere );

	// OpenGL-specific data members would go here
	// For example: GLuint m_vao, m_vbo, m_ibo;
	// For full implementation, see Code/Gfx/OpenGL/README.md
};

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/
	
} // namespace Nx

#endif // __GFX_OPENGL_P_NX_MODEL_H
