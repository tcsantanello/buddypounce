#define __MAINMEMMANAGER__

#include "Buddy Pounce.h"
#include "stdafx.h"
#include <windows.h>

#ifdef MEMMAN

#ifdef new
#undef new
#endif

#ifdef DEBUG_NEW
#undef DEBUG_NEW
#endif

MemoryManager MemManager( MEGABYTE( 5 ), MEGABYTE( 10 ) );

HANDLE MemoryManager::hHeap   = NULL;
HANDLE MemoryManager::hMutex  = NULL;
DWORD  MemoryManager::dwAlloc = 0;

#ifdef _DEBUG
MemoryManager::MemBlocks MemoryManager::MemoryTrack;
DWORD                    MemoryManager::dwBytes  = 0;
DWORD                    MemoryManager::dwMaxMem = 0;

void MemoryManager::AddTrack( void *pPtr, DWORD dwSize, const TCHAR *szFile, DWORD dwLine ) {
  MemBlock NewNode;

  if ( szFile ) {
    DWORD        dwEnd = 0;
    DWORD        dwLen = 0;
    const TCHAR *pFile = strrchr( szFile, '\\' );

    if ( pFile == NULL ) {
      pFile = szFile;
    }

    dwLen = ( DWORD ) strlen( pFile );
    dwEnd = min( 30, dwLen );

    memcpy( NewNode.szFile, pFile, dwEnd );
    NewNode.szFile[ dwEnd ] = NULL;
  }

  NewNode.dwLine = dwLine;

  NewNode.pPtr   = pPtr;
  NewNode.dwSize = dwSize;

  MemoryTrack.push_back( NewNode );
}

void MemoryManager::RemTrack( void *pPtr ) {
  if ( ( MemoryTrack.size( ) ) ) {
    MemIterator Iterator;
    foreach ( Iterator, MemoryTrack ) {
      MemBlock MemNode = *Iterator;
      if ( MemNode.pPtr == pPtr ) {
        MemoryTrack.erase( Iterator-- );
        break;
      }
    }
    if ( !MemoryTrack.size( ) ) {
      MemoryTrack.clear( );
    }
  }
}

void MemoryManager::DumpUnfreed( ) {
  MemIterator Iterator;

  DWORD totalSize = 0;
  char  buf[ 1024 ];

  sprintf( buf,
           "-----------------------------------------------------------\n"
           "\t\t\t Max Memory Used: %10d Bytes\n"
           "-----------------------------------------------------------\n",
           dwMaxMem );
  OutputDebugString( buf );

  if ( !MemoryTrack.size( ) )
    return;

  sprintf( buf, "-----------------------------------------------------------\n" );
  OutputDebugString( buf );

  foreach ( Iterator, MemoryTrack ) {
    MemBlock MemNode = *Iterator;
    sprintf( buf,
             "%-32s:\t\tLINE %d,\t\tADDRESS 0x%x\t%d unfreed\n",
             MemNode.szFile,
             MemNode.dwLine,
             MemNode.pPtr,
             MemNode.dwSize );

    OutputDebugString( buf );
    totalSize += MemNode.dwSize;
  }

  sprintf( buf, "-----------------------------------------------------------\n" );
  OutputDebugString( buf );

  sprintf( buf, "Total Unfreed: %d bytes\n", totalSize );
  OutputDebugString( buf );
}

#endif

MemoryManager::~MemoryManager( ) {
  if ( ( --dwAlloc ) == 0 ) {
    HeapUnlock( hHeap );
    HeapDestroy( hHeap );
    hHeap = NULL;
  }
}

MemoryManager::MemoryManager( DWORD dwMin, DWORD dwMax ) {
  CreateHeap( dwMin, dwMax );
}

MemoryManager::MemoryManager( ) {
  CreateHeap( MEGABYTE( 5 ), MEGABYTE( 10 ) );
}

bool MemoryManager::CreateHeap( DWORD dwMin, DWORD dwMax ) {
  if ( hMutex == NULL )
    hMutex = CreateMutex( NULL, FALSE, "HEAPLOCK" );
  KILLONFAIL( hMutex, "Error: Couldn't Create Heap Lock!", return false );

  WaitForSingleObject( hMutex, INFINITE );

  if ( hHeap == NULL ) {
    hHeap = HeapCreate( HEAP_NO_SERIALIZE, dwMin, dwMax );
    KILLONFAIL( hHeap, "Error: Couldn't Create Heap!", DeleteObject( hMutex ); return false );
    HeapLock( hHeap );
  }

  ReleaseMutex( hMutex );
  return true;
}

void *MemoryManager::AllocMem( DWORD dwSize ) {

  void *pPtr = NULL;
  __try {
    pPtr =
      HeapAlloc( hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY, dwSize );
    if ( pPtr ) {
      dwAlloc++;
#ifdef _DEBUG
      dwBytes += ( DWORD ) HeapSize( hHeap, 0, pPtr );
      if ( dwBytes > dwMaxMem ) {
        dwMaxMem = dwBytes;
      }
#endif
    }
  } __finally {
    ReleaseMutex( hMutex );
  }
  return pPtr;
}

void MemoryManager::FreeMem( void *pPtr ) {
  if ( hMutex && hHeap && IsValid( pPtr ) ) {
    WaitForSingleObject( hMutex, INFINITE );
#ifdef _DEBUG
    dwBytes -= ( DWORD ) HeapSize( hHeap, 0, pPtr );
#endif
    HeapFree( hHeap, 0, pPtr );
    KILLONFAIL(
      ( HeapCompact( hHeap, HEAP_NO_SERIALIZE ) ), "Compaction Reports Out-Of-Memory.", NULL );
    ReleaseMutex( hMutex );
  }
}

#endif