////////////////////////////////////////////////////////////////////////////
// binaryasset.h - interface for generic binary file assets
// provides a common interface to the asset manager
#ifndef	__GEL_BINARYASSET_H__
#define	__GEL_BINARYASSET_H__

#ifndef __CORE_DEFINES_H
#include <core/defines.h>
#endif

#include <gel/assman/asset.h>

namespace Ass
{

class CBinaryAsset : public CAsset
{
public:
	virtual int 				Load(const char *p_file, bool async_load, bool use_pip, void* pExtraData, Script::CStruct *pStruct);
	virtual int 				Unload();
	virtual int 				Reload(const char *p_file);
	virtual bool				LoadFinished();
	virtual const char *  		Name();
	virtual EAssetType 			GetType();
};

} // end namespace Ass

#endif // __GEL_BINARYASSET_H__
