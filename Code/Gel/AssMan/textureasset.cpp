///////////////////////////////////////////////////////////////////////////
// textureasset.cpp
//
// Asset dependent code for loading, unloading and reloading texture data
//

#include <gel/assman/textureasset.h>
#include <gel/assman/assettypes.h>
#include <core/string/stringutils.h>
#include <sys/file/filesys.h>

namespace Ass
{

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int CTextureAsset::Load(const char *p_file, bool async_load, bool use_pip, void* pExtraData, Script::CStruct *pStruct)
{
	Dbg_MsgAssert(!async_load, ("Async load not supported on CTextureAsset"));

	// Load texture dictionary file
	int file_size = 0;
	void* p_textures = File::LoadAlloc(p_file, &file_size);
	
	if (!p_textures)
	{
		Dbg_MsgAssert(0, ("Texture file %s doesn't exist.", p_file));
		return -1;
	}

	SetData(p_textures);
	return 0;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int CTextureAsset::Unload()
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

int CTextureAsset::Reload(const char *p_file)
{
	Unload();
	return Load(p_file, false, false, NULL, NULL);
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

bool CTextureAsset::LoadFinished()
{
	return (GetData() != NULL);
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

const char * CTextureAsset::Name()
{
	return "Texture Asset";
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

EAssetType CTextureAsset::GetType()
{
	return ASSET_TEXTURES;
}

} // namespace Ass
