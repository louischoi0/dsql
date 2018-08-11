#include "dsql.h"
#include <iostream>
#include <windows.h>

// Now i write code that open and close for one transaction.
// It will be changed to the code that db struct hold file HANDLE to reduce overhead.
// And Exception Handle Code Not implemented.

int dsqlCreateFile(const char* name){
  HANDLE hfile = CreateFile(name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

  if( hfile == INVALID_HANDLE_VALUE ){
    dsqlDebugPrint("Create File Failed");
    return 1;
  }

  CloseHandle(hfile);
  return 0;
}

int _dsqlWriteFile(const char* name, const char* buffer, u16 size, u16 offset){
  	HANDLE hfile = CreateFile(name, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  	SetFilePointer(hfile, offset, NULL, FILE_BEGIN);
    std::cout << GetLastError << std::endl;

  	BOOL bErrorFlag = FALSE;
  	DWORD dwBytesWritten = 0;

  	bErrorFlag = WriteFile(hfile, (LPCVOID)buffer, size, &dwBytesWritten, NULL);

    dsqlDebugPrint("dsqlWriteFile");
  	CloseHandle(hfile);
  	return 0;
}

int dsqlWriteFile(const dsqlDb& db, const char* buffer, u16 size, u16 offset){
  _dsqlWriteFile(db.dbName, buffer, size, offset);
}

int _dsqlReadFile(const char* name, char* dest, u16 size, u16 offset){
  int rc = 0;

	HANDLE hfile = CreateFileA(name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetFilePointer(hfile, offset, NULL, FILE_BEGIN);

	BOOL bErrorFlag = FALSE;
	DWORD dwBytesRead = 0;

	rc = ReadFile(hfile, dest, size, &dwBytesRead, 0);

  CloseHandle(hfile);

  if(!rc){
    return 1;
  }

  else{
    return 0;
  }
}

int dsqlReadFile(const dbConnector& db, char* dest, u16 size, u16 offset){
  _dsqlReadFile(db.dbName, dest, size, offset);
}

int dsqlCoreAlloc(char** src, int size){
  *src = (char*)malloc(size);
  return 0;
}

int dsqlCoreFree(char* c){
  delete []c;
}

int dsqlCoreMcopy(char* dest, char* src, int size){
  memcpy(dest, src, size);
  return 0;
}

void dsqlDebugPrint(const char* message){
  std::cout << message;
};
