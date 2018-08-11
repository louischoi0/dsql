#include "pager.h"
#include "dsql.h"
#include <assert.h>
#include <cstring>
#include <iostream>

int getPageOffset(u32 nPage, u32* res){
  u32 offset = nPage * pageConfig::pageSize;
  offset += pageStart;
  *res = offset;
  return 0;
}

int commitNewPage(const dsqlDb& db, int nPage, struct table* table,int nsection,int sectionSize){
  char *buffer;
  s32 r = 1;
  dsqlCoreAlloc(&buffer, pageConfig::pageSize );
  memset(buffer, 0, pageConfig::pageHeader );
  memset(buffer + pageConfig::pageHeader , 65, pageConfig::pageSize - pageConfig::pageHeader);
  memcpy(buffer, (char*)&nPage, 4 );

  memcpy(buffer + 8, &nsection, 4 );
  memcpy(buffer + 12, &sectionSize, 4);

  u32 offset;
  getPageOffset(nPage, &offset);
  std::cout << "PageCommit Offset :: " << offset << std::endl;
  dsqlWriteFile(db, buffer, pageConfig::pageSize, offset);

  updatePageCount(db, nPage);
  return 0;
};

int updatePageCount(const dsqlDb& db , int pageCount){
  assert(pageCount >= 0);
  dsqlWriteFile(db, (char*)&pageCount, 4, pageInfo);
};

int getPageCount(const dsqlDb& db, int* nPage){
  dsqlReadFile(db, (char*)(nPage), 4, pageInfo);
  return 0;
};

int getSectionSize(const dsqlDb& db, u32* nsection, u32* sectionSize){
  dsqlReadFile(db,(char*)nsection,4,pageInfo + 4);
  dsqlReadFile(db,(char*)sectionSize,4,pageInfo + 8);
  return 0;
};

int setSectionSizeField(const dsqlDb& db, u32 nsection, u32 sectionSize){
  dsqlWriteFile(db,(char*)&nsection,4,pageInfo + 4);
  dsqlWriteFile(db,(char*)&sectionSize,4,pageInfo + 8);
  return 0;
};

int checkPageCountValidate(const dsqlDb& db, int nPage ){
  int count;
  getPageCount(db, &count);
  assert(nPage == count);

  return 0;
}
// Initialize mPage Structure.
// This routine read up dbFile to process memory.
int mupOnePage(const dsqlDb& db, mPage* mpage, int npage){
  u32 pageOffset;
  getPageOffset(npage, (u32*)&pageOffset);
  std::cout << "Page offset in muponepage :: " << pageOffset << std::endl;
  u32 page;
  char *pageHeaderBuffer;
  u32 nrecord;
  u32 nsection;
  u32 sectionSize;
  u16 overflow;

  dsqlCoreAlloc(&pageHeaderBuffer, pageConfig::pageHeader);
  dsqlReadFile(db, pageHeaderBuffer, pageConfig::pageHeader, pageOffset);

  memcpy( &page, pageHeaderBuffer, 4);
  assert( page == npage );

  memcpy(&nrecord,pageHeaderBuffer + 4, 4);
  assert(nrecord >= 0);

  std::cout << "Record Count in mupOnePage :: " << nrecord  << std::endl;
  memcpy(&nsection, pageHeaderBuffer + 8 , 4);
  assert(nsection >= 0);

  memcpy(&sectionSize, pageHeaderBuffer + 12, 4);
  assert(sectionSize >= 0);

  memcpy(&overflow, pageHeaderBuffer + 16, 2);

  mpage->setRecordCount(nrecord);
  mpage->setPageIndex(page);
  mpage->setSectionSize(sectionSize);
  mpage->setSectionCount(nsection);
  mpage->setOverFlowFlag(overflow);
  mpage->setPageOffset(pageOffset);

  mpage->allocateBuffer(pageConfig::pageSize);
  dsqlReadFile(db, mpage->bufferPointer(), pageConfig::pageSize - pageConfig::pageHeader , pageOffset + pageConfig::pageHeader);
  dsqlCoreFree(pageHeaderBuffer);
};

int readRecordBuffer(char* buffer, int npage, int ncursor){
  return 0;
};

int getInsertWirteBufferIndex(mPage* page , u32 npage, u32* offset, int size){
  char* pb = page->bufferPointer() + pageConfig::pageHeader;
  int i;
  for(i = 0; i < size ; i++, pb++){
    if( (s32)*pb == emptySignature ){
      *offset = i;
      break;
    }
  }

  return 0;
};

int comitRecord(const dsqlDb& db, char* buffer){
  return 0;
};

int getOverflowPageWriteBuffer( const dsqlDb &db, u32 npage, s32* offset){
  u32 pageOffset;

  getPageOffset(npage + 1, &pageOffset);
  dsqlReadFile(db, (char*)&offset, 4, pageOffset);

  return 0;
};

int initOverFlowPageFields(const dsqlDb &db, u32 npage){
  int r = 200;
  u32 offset;
  getPageOffset(npage, &offset);
  dsqlWriteFile(db,(char*)&r, 4, offset );
};

int setOverFlowFlag(const dsqlDb& db, int npage){
  s32 r = 1;
  u32 offset;
  getPageOffset(npage, &offset);
  dsqlWriteFile(db, (char*)&r, 20, offset);

  return 0;
};

int initAdminPage(const dsqlDb& db ){
  commitNewPage(db, 0, 0,400,20);
  commitNewPage(db, 1, 0,400,20);
  setOverFlowFlag(db, 1);

  return 0;
};
