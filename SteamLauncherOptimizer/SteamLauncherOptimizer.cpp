#include "Utils.h"
#include <DbgHelp.h>
#include <Psapi.h>
#pragma comment( lib, "dbghelp.lib" )
#pragma comment( lib, "Psapi.lib" )


string ItoA( int i, int rad ) {
  char buffer[1024];
  _itoa_s( i, buffer, rad );
  return buffer;
}

string UltoA( ulong i, int rad ) {
  char buffer[1024];
  _ultoa_s( i, buffer, rad );
  return buffer;
}


int __stdcall CreateHardLinkA_2_CreateSymbolicLinkA( const char* lpFileName, const char* lpExistingFileName, LPSECURITY_ATTRIBUTES lpSecurityAttributes ) {
  if( CreateSymbolicLinkA( lpFileName, lpExistingFileName, 0x0 ) )
    return 1;

  if( CreateSymbolicLinkA( lpFileName, lpExistingFileName, 0x2 ) )
    return 1;
  
  return CreateHardLinkA( lpFileName, lpExistingFileName, lpSecurityAttributes );
}


bool GetModuleRangeInformation( HMODULE module, ulong& base, ulong& size ) {
  MODULEINFO module_info; memset( &module_info, 0, sizeof( module_info ) );
  if( GetModuleInformation( GetCurrentProcess(), module, &module_info, sizeof( module_info ) ) ) {
    base = (ulong)module_info.lpBaseOfDll;
    size = (ulong)module_info.SizeOfImage;
    return true;
  }

  return false;
}


void CollectPeSections( HMODULE module, vector<PeSection>& sections ) {
  IMAGE_NT_HEADERS* ntHeader = ImageNtHeader( module );
  IMAGE_SECTION_HEADER* sectionHeader = (IMAGE_SECTION_HEADER*)(ntHeader + 1);
  ulong imageBase, imageLength;
  if( !GetModuleRangeInformation( module, imageBase, imageLength ) )
    return;

  ulong segments = ntHeader->FileHeader.NumberOfSections;
  for( ulong i = 0; i < segments; i++ ) {
    PeSection section;
    section.Address = imageBase + sectionHeader->VirtualAddress;
    section.Length = sectionHeader->Misc.VirtualSize;
    section.Name = (char*)sectionHeader->Name;
    sections.push_back( section );
    sectionHeader++;
  }
}


void ReplaceText( const string& oldtxt, const string& newtxt, PeSection& section ) {
  char* textData = (char*)section.Address;
  char* textDataEnd = (char*)(section.Address + section.Length - oldtxt.size());
  do
  {
    if( memcmp( oldtxt.c_str(), textData, oldtxt.size() + 1 ) == 0 ) {
      MemUnlocker unlocker( textData, oldtxt.size() + 1 );
      memcpy( textData, newtxt.c_str(), newtxt.size() + 1 );
    }
  } while( ++textData < textDataEnd );
}


void ReplaceInt( const ulong& oldInt, const ulong& newInt, PeSection& section ) {
  int* intTable = (int*)section.Address;
  int intTableLength = section.Length / 4;
  for( ulong i = 0; i < intTableLength; i++ ) {
    if( intTable[i] == oldInt ) {
      MemUnlocker unlocker( &intTable[i], 4 );
      intTable[i] = newInt;
    }
  }
}


void PatchCalls() {
  vector<PeSection> sections;
  CollectPeSections( GetModuleHandle( nullptr ), sections );

  for( ulong i = 0; i < sections.size(); i++ ) {
    PeSection& section = sections[i];
    if( section.Name.compare( ".rdata" ) == 0 ) {
      ReplaceText( ".vdf", ".???", section );
      ReplaceText( ".mod", ".???", section );

      HMODULE kernel32 = GetModuleHandle( "kernel32.dll" );
      ulong procCreateHardLinkA = (ulong)GetProcAddress( kernel32, "CreateHardLinkA" );
      ulong procCreateSymLinkA = (ulong)CreateHardLinkA_2_CreateSymbolicLinkA;
      ReplaceInt( procCreateHardLinkA, procCreateSymLinkA, section );
    }
  }
}

const long MaxLogSize = 1024 * 1024 * 5;

bool CheckLogSize() {
  const char* logFileName = "logs.txt";
  const char* logPrevFileName = "logs_prev.txt";
  if( !FileExists( logFileName ) )
    return true;

  FILE* file = fopen( "logs.txt", "rb+" );
  if( !file )
    return false;

  fseek( file, 0, SEEK_END );
  long fileSize = ftell( file );
  fclose( file );
  if( fileSize >= MaxLogSize ) {
    DeleteFileSafe( logPrevFileName );
    return RenameFileSafe( logFileName, logPrevFileName );
  }

  return true;
}

extern string GetSpecialPath( int index );

int __stdcall DllMain( HMODULE hModule, DWORD fdwReason, LPVOID lpvReserved ) {
  if( fdwReason == DLL_PROCESS_ATTACH ) {
    PatchCalls();
    CheckLogSize();
  }
  if( fdwReason == DLL_PROCESS_DETACH ) {

  }
  return 1;
}