///////////////////////////////////////////////////////////////////////////
// collisionasset.cpp
//
// Asset dependent code for loading, unloading and reloading collision data
//

#include <gel/assman/collisionasset.h>
#include <gel/assman/assettypes.h>
#include <core/string/stringutils.h>
#include <sys/file/filesys.h>

namespace Ass
{

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int CCollisionAsset::Load(const char *p_file, bool async_load, bool use_pip, void* pExtraData, Script::CStruct *pStruct)
{
	Dbg_MsgAssert(!async_load, ("Async load not supported on CCollisionAsset"));

	// Load collision data file
	int file_size = 0;
	void* p_collision = File::LoadAlloc(p_file, &file_size);
	
	if (!p_collision)
	{
		Dbg_MsgAssert(0, ("Collision file %s doesn't exist.", p_file));
		return -1;
	}

	SetData(p_collision);
	return 0;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int CCollisionAsset::Unload()
{
	if (GetData())
	{
		Mem::Free(GetData());
		SetData(NULL);
	}
	return 0;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int CCollisionAsset::Reload(const char *p_file)
{
	Unload();
	return Load(p_file, false, false, NULL, NULL);
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

bool CCollisionAsset::LoadFinished()
{
	return (GetData() != NULL);
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

const char * CCollisionAsset::Name()
{
	return "Collision Asset";
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

EAssetType CCollisionAsset::GetType()
{
	return ASSET_COLLISION;
}

} // namespace Ass
