#pragma warning( disable : 4717 )

#ifdef _CRTDBG_MAP_ALLOC
#undef _CRTDBG_MAP_ALLOC
#endif
#include <crtdbg.h>
#ifdef _DEBUG
#define THIS_FILE __FILE__
inline void *operator new( size_t nSize, const char *lpszFileName, int nLine ) {
  return ::operator new( nSize, _NORMAL_BLOCK, lpszFileName, nLine );
}
inline void __cdecl operator delete( void *_P, const char *lpszFileName, int nLine ) {
  ::operator delete( _P, lpszFileName, nLine );
}
#define DEBUG_NEW new ( THIS_FILE, __LINE__ )
#define malloc( s ) _malloc_dbg( s, _NORMAL_BLOCK, THIS_FILE, __LINE__ )
#define calloc( c, s ) _calloc_dbg( c, s, _NORMAL_BLOCK, THIS_FILE, __LINE__ )
#define realloc( p, s ) _realloc_dbg( p, s, _NORMAL_BLOCK, THIS_FILE, __LINE__ )
#define _expand( p, s ) _expand_dbg( p, s, _NORMAL_BLOCK, THIS_FILE, __LINE__ )
#define free( p ) _free_dbg( p, _NORMAL_BLOCK )
#define _msize( p ) _msize_dbg( p, _NORMAL_BLOCK )
#endif

#pragma warning( default : 4717 )
