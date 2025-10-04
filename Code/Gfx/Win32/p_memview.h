/*****************************************************************************
**																			**
**					   	  Neversoft Entertainment							**
**																		   	**
**				   Copyright (C) 1999 - All Rights Reserved				   	**
**																			**
******************************************************************************
**																			**
**	Project:		GFX (Graphics Engine Library)							**
**																			**
**	Module:			Memory Viewer (MemView)									**
**																			**
**	File name:		p_memview.h												**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	PC platform memory viewer stub							**
**																			**
*****************************************************************************/

#ifndef	__P_MEMVIEW_H__
#define	__P_MEMVIEW_H__

// Stub functions for PC platform memory debugging
inline void MemView_Display() {}
inline void MemView_Input(uint buttons, uint makes, uint breaks) {}
inline void MemView_Alloc(void *v) {}
inline void MemView_Free(void *v) {}
inline void MemViewToggle() {}
inline void MemView_FindLeaks() {}
inline int DumpUnwindStack(int iMaxDepth, int *pDest) { return 0; }
inline char *MemView_GetFunctionName(int pc, int *p_size) { return nullptr; }
inline void MemView_DumpFragments(Mem::Heap *pHeap) {}
inline void MemView_AnalyzeBlocks(uint32 mask = 0) {}
inline void MemView_MarkBlocks(uint32 flags = 1) {}
inline void MemView_DumpHeap(Mem::Heap *pHeap) {}
inline void MemView_AnalyzeHeap(Mem::Heap *pHeap) {}

#endif	// __P_MEMVIEW_H__
