#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <io.h>
using namespace std;

typedef unsigned long ulong;

class MemUnlocker {
  void* Address;
  ulong Length;
  ulong Protection;
public:

  MemUnlocker( const ulong& address, const ulong& length ) {
    Address = (void*)address;
    Length = length;
    Protection =
      VirtualProtect( Address, Length, PAGE_EXECUTE_READWRITE, &Protection );
  }

  MemUnlocker( void* address, const ulong& length ) {
    Address = address;
    Length = length;
    VirtualProtect( Address, Length, PAGE_EXECUTE_READWRITE, &Protection );
  }

  ~MemUnlocker() {
    VirtualProtect( Address, Length, Protection, &Protection );
  }
};

struct PeSection {
  ulong Address;
  ulong Length;
  string Name;
  PeSection() {
    Address = 0;
    Length = 0;
  }
};

bool FileExists( const char* fileName ) {
  return _access( fileName, 0 ) == 0;
}

bool DeleteFileSafe( const char* fileName ) {
  if( !FileExists( fileName ) )
    return true;

  for( size_t i = 0; i < 10; i++ ) {
    if( DeleteFile( fileName ) )
      return true;

    Sleep( 10 );
  }

  return false;
}

bool RenameFileSafe( const char* fileNameOld, const char* fileNameNew ) {
  if( !FileExists( fileNameOld ) )
    return false;

  for( size_t i = 0; i < 10; i++ ) {
    if( rename( fileNameOld, fileNameNew ) == 0 )
      return true;

    Sleep( 10 );
  }

  return false;
}