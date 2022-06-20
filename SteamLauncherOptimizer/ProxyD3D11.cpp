#include <Windows.h>
#include <Shlobj.h>
#include <iostream>
using namespace std;

HMODULE LoadLibraryFromSystem( string name ) {
  char path[MAX_PATH + 1];
  if( SHGetFolderPath( NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, path ) == S_OK )
    return LoadLibrary( (string( path ).append( "\\" ).append( name )).c_str());

  return LoadLibrary( string( "C:\\Windows\\System32\\" ).append( name ).c_str());
}

#define Function extern "C" __declspec(naked) void __stdcall

#define DllProxyLoadLibrary(d)                          \
                                                        \
/* Loader of original module */                         \
EXTERN_C HMODULE GetModuleHandle_##d() {                \
  static HMODULE hModule = LoadLibraryFromSystem( #d ); \
  return hModule;                                       \
}

#define DllProxyDeclaration(d, f)                                             \
                                                                              \
/* Pointer to oroginal function */                                            \
static int Pointer_##d##f = (int)GetProcAddress( GetModuleHandle_##d(), #f ); \
                                                                              \
/* Jump to original function */                                               \
Function __##d##f() {                                                         \
  __asm { jmp Pointer_##d##f }                                                \
}

DllProxyLoadLibrary( d3d11 );
DllProxyDeclaration( d3d11, D3D11CoreCreateDevice );
DllProxyDeclaration( d3d11, D3D11CoreCreateLayeredDevice );
DllProxyDeclaration( d3d11, D3D11CoreGetLayeredDeviceSize );
DllProxyDeclaration( d3d11, D3D11CoreRegisterLayers );
DllProxyDeclaration( d3d11, D3D11CreateDevice );
DllProxyDeclaration( d3d11, D3D11CreateDeviceAndSwapChain );
DllProxyDeclaration( d3d11, D3DKMTCloseAdapter );
DllProxyDeclaration( d3d11, D3DKMTCreateAllocation );
DllProxyDeclaration( d3d11, D3DKMTCreateContext );
DllProxyDeclaration( d3d11, D3DKMTCreateDevice );
DllProxyDeclaration( d3d11, D3DKMTCreateSynchronizationObject );
DllProxyDeclaration( d3d11, D3DKMTDestroyAllocation );
DllProxyDeclaration( d3d11, D3DKMTDestroyContext );
DllProxyDeclaration( d3d11, D3DKMTDestroyDevice );
DllProxyDeclaration( d3d11, D3DKMTDestroySynchronizationObject );
DllProxyDeclaration( d3d11, D3DKMTEscape );
DllProxyDeclaration( d3d11, D3DKMTGetContextSchedulingPriority );
DllProxyDeclaration( d3d11, D3DKMTGetDeviceState );
DllProxyDeclaration( d3d11, D3DKMTGetDisplayModeList );
DllProxyDeclaration( d3d11, D3DKMTGetMultisampleMethodList );
DllProxyDeclaration( d3d11, D3DKMTGetRuntimeData );
DllProxyDeclaration( d3d11, D3DKMTGetSharedPrimaryHandle );
DllProxyDeclaration( d3d11, D3DKMTLock );
DllProxyDeclaration( d3d11, D3DKMTOpenAdapterFromHdc );
DllProxyDeclaration( d3d11, D3DKMTOpenResource );
DllProxyDeclaration( d3d11, D3DKMTPresent );
DllProxyDeclaration( d3d11, D3DKMTQueryAdapterInfo );
DllProxyDeclaration( d3d11, D3DKMTQueryAllocationResidency );
DllProxyDeclaration( d3d11, D3DKMTQueryResourceInfo );
DllProxyDeclaration( d3d11, D3DKMTRender );
DllProxyDeclaration( d3d11, D3DKMTSetAllocationPriority );
DllProxyDeclaration( d3d11, D3DKMTSetContextSchedulingPriority );
DllProxyDeclaration( d3d11, D3DKMTSetDisplayMode );
DllProxyDeclaration( d3d11, D3DKMTSetDisplayPrivateDriverFormat );
DllProxyDeclaration( d3d11, D3DKMTSetGammaRamp );
DllProxyDeclaration( d3d11, D3DKMTSetVidPnSourceOwner );
DllProxyDeclaration( d3d11, D3DKMTSignalSynchronizationObject );
DllProxyDeclaration( d3d11, D3DKMTUnlock );
DllProxyDeclaration( d3d11, D3DKMTWaitForSynchronizationObject );
DllProxyDeclaration( d3d11, D3DKMTWaitForVerticalBlankEvent );
DllProxyDeclaration( d3d11, D3DPerformance_BeginEvent );
DllProxyDeclaration( d3d11, D3DPerformance_EndEvent );
DllProxyDeclaration( d3d11, D3DPerformance_GetStatus );
DllProxyDeclaration( d3d11, D3DPerformance_SetMarker );
DllProxyDeclaration( d3d11, EnableFeatureLevelUpgrade );
DllProxyDeclaration( d3d11, OpenAdapter10 );
DllProxyDeclaration( d3d11, OpenAdapter10_2 );