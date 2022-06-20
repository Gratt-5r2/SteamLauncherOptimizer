#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
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