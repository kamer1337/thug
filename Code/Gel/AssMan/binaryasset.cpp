///////////////////////////////////////////////////////////////////////////
// binaryasset.cpp
//
// Asset dependent code for loading, unloading and reloading binary files
//

#include <gel/assman/binaryasset.h>
#include <gel/assman/assettypes.h>
#include <core/string/stringutils.h>
#include <sys/file/filesys.h>

namespace Ass
{

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int CBinaryAsset::Load(const char *p_file, bool async_load, bool use_pip, void* pExtraData, Script::CStruct *pStruct)
{
	Dbg_MsgAssert(!async_load, ("Async load not supported on CBinaryAsset"));

	// Load generic binary file
	int file_size = 0;
	void* p_file_data = File::LoadAlloc(p_file, &file_size);
	
	if (!p_file_data)
	{
		Dbg_MsgAssert(0, ("Binary file %s doesn't exist or couldn't be loaded.", p_file));
		return -1;
	}

	SetData(p_file_data);
	return 0;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int CBinaryAsset::Unload()
{
	if (GetData())
	{
		// Free the loaded binary data
		Mem::Free(GetData());
		SetData(NULL);
	}
	return 0;
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

int CBinaryAsset::Reload(const char *p_file)
{
	Unload();
	return Load(p_file, false, false, NULL, NULL);
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

bool CBinaryAsset::LoadFinished()
{
	return (GetData() != NULL);
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

const char * CBinaryAsset::Name()
{
	return "Binary Asset";
}

/******************************************************************/
/*                                                                */
/*                                                                */
/******************************************************************/

EAssetType CBinaryAsset::GetType()
{
	return ASSET_BINARY;
}

} // namespace Ass
