#pragma once

//#define MEMMAN

#ifdef MEMMAN
#ifdef _DEBUG
#include "list"
#endif

#define _BYTE( x ) __int64( x )
#define KILOBYTE( x ) __int64( _BYTE( x ) * 1024 )
#define MEGABYTE( x ) __int64( KILOBYTE( x ) * 1024 )
#define GIGABYTE( x ) __int64( MEGABYTE( x ) * 1024 )
#define TERABYTE( x ) __int64( GIGABYTE( x ) * 1024 )

class MemoryManager {
 private:
  static HANDLE hHeap;
  static HANDLE hMutex;
  static DWORD  dwAlloc;

  bool CreateHeap( DWORD, DWORD );

#ifdef _DEBUG
  typedef struct MemBlock {
    TCHAR szFile[ 30 ];
    DWORD dwLine;
    DWORD dwSize;
    void *pPtr;
  } MemBlock;

  typedef std::list< MemBlock > MemBlocks;
  typedef MemBlocks::iterator   MemIterator;

  static MemBlocks MemoryTrack;
  static DWORD     dwMaxMem;
  static DWORD     dwBytes;

#endif

 public:
  MemoryManager( );
  MemoryManager( DWORD, DWORD );
  ~MemoryManager( );
  void *AllocMem( DWORD );
  void  FreeMem( void * );
  void  DumpUnfreed( void );

#ifdef _DEBUG
  void AddTrack( void *, DWORD, const TCHAR *, DWORD );
  void RemTrack( void * );
#endif
};

#ifndef __MAINMEMMANAGER__
extern MemoryManager MemManager;
#endif

#ifdef _DEBUG

inline void *__cdecl operator new( unsigned int iSize, const char *szFile, int iLine ) {
  void *pPtr = ::operator new( iSize );
  MemManager.AddTrack( pPtr, iSize, szFile, iLine );
  return pPtr;
}

#endif

inline void *__cdecl operator new( unsigned int iSize ) {
  return MemManager.AllocMem( iSize );
}
inline void __cdecl operator delete( void *pPtr ) {
#ifdef _DEBUG
  MemManager.FreeMem( pPtr );
#endif
}

#else
#include <crtdbg.h>
#ifdef _CRTDBG_MAP_ALLOC
#undef _CRTDBG_MAP_ALLOC
#endif

inline void *operator new( size_t nSize, const char *lpszFileName, int nLine ) {
  return ::operator new( nSize, _NORMAL_BLOCK, lpszFileName, nLine );
}
inline void __cdecl operator delete( void *_P, const char *lpszFileName, int nLine ) {
  ::operator delete( _P );
}
#endif

/********************************************************************************************************/