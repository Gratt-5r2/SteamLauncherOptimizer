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
  // Safe symbolic link creation. The existence of the file is checked for Linux.
  if( CreateSymbolicLinkA( lpFileName, lpExistingFileName, 0x0 ) && FileExists( lpFileName ) )
    return 1;
  
  if( CreateSymbolicLinkA( lpFileName, lpExistingFileName, 0x2 ) && FileExists( lpFileName ) )
    return 1;
  
  return CreateHardLinkA( lpFileName, lpExistingFileName, lpSecurityAttributes );
}


int __stdcall SetWindowTextWithSign( HWND hWnd, LPCWSTR lpString ) {
  return SetWindowTextW( hWnd, wstring( lpString ).append( L" (optimized)" ).c_str() );
}


int __stdcall CreateStarterProcess( LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                    LPSECURITY_ATTRIBUTES lpThreadAttributes, int bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
                                    LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation ) {

  if( std::string( lpCommandLine ).find( "-game:GothicGame.ini" ) != std::string::npos ) {
    if( _access( "GothicStarter.exe", 0 ) == 0 ) {
      int ok = MessageBoxA( 0, "Would you like to run a classic 'GothicStarter' with Steam Workshop support?", "Starting the game", MB_YESNO | MB_ICONQUESTION );
      if( ok == IDYES )
        lpCommandLine = const_cast<char*>("GothicStarter.exe");
    }
  }

  return CreateProcessA( lpApplicationName, lpCommandLine, lpProcessAttributes,
                         lpThreadAttributes, bInheritHandles, dwCreationFlags, 
                         lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation );
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


FARPROC __stdcall GetProcAddressSafe( LPCSTR libName, LPCSTR procName ) {
  HMODULE lib = LoadLibrary( libName );
  return GetProcAddress( lib, procName );
}


void PatchCalls() {
  vector<PeSection> sections;
  CollectPeSections( GetModuleHandle( nullptr ), sections );

  for( ulong i = 0; i < sections.size(); i++ ) {
    PeSection& section = sections[i];
    if( section.Name.compare( ".rdata" ) == 0 ) {
      ReplaceText( ".vdf", ".???", section );
      ReplaceText( ".mod", ".???", section );

      ReplaceInt( (ulong)GetProcAddressSafe( "kernel32.dll", "CreateHardLinkA" ), (ulong)CreateHardLinkA_2_CreateSymbolicLinkA, section );
      ReplaceInt( (ulong)GetProcAddressSafe( "user32.dll", "SetWindowTextW" ), (ulong)SetWindowTextWithSign, section );
      ReplaceInt( (ulong)GetProcAddressSafe( "kernel32.dll", "CreateProcessA" ), (ulong)CreateStarterProcess, section );
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