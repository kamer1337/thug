///////////////////////////////////////////////////////////////////////////
// sceneasset.cpp
//
// Asset dependent code for loading, unloading and reloading scene data
//

#include <gel/assman/sceneasset.h>
#include <gel/assman/assettypes.h>
#include <core/string/stringutils.h>
#include <gfx/nx.h>

namespace Ass
{

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int CSceneAsset::Load(const char *p_file, bool async_load, bool use_pip, void* pExtraData, Script::CStruct *pStruct)
{
	Dbg_MsgAssert(!async_load, ("Async load not supported on CSceneAsset"));

	// Load scene/level geometry through the graphics engine
	// Note: CTexDict parameter is NULL for now - will need proper texture dictionary management
	Nx::CScene* p_scene = Nx::CEngine::sLoadScene(p_file, NULL, true, false, false, false);
	
	if (!p_scene)
	{
		Dbg_MsgAssert(0, ("Scene %s doesn't exist.", p_file));
		return -1;
	}

	SetData((void*)p_scene);
	return 0;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int CSceneAsset::Unload()
{
	if (GetData())
	{
		Nx::CEngine::sUnloadScene((Nx::CScene*)GetData());
		SetData(NULL);
	}
	return 0;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int CSceneAsset::Reload(const char *p_file)
{
	Unload();
	return Load(p_file, false, false, NULL, NULL);
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

bool CSceneAsset::LoadFinished()
{
	return (GetData() != NULL);
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

const char * CSceneAsset::Name()
{
	return "Scene Asset";
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

EAssetType CSceneAsset::GetType()
{
	return ASSET_SCENE;
}

} // namespace Ass
