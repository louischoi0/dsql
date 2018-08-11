#ifndef PAGERH
#define PAGERH

#include "dsql.h"


/**
  Page Header Format

  offset size Field
    0     4   Page No
    4     4   Record Count
    8     4   Section Count
    12    4   Section Size
    16    2   Overflow Page Flag
**/

enum pageConfig{
  pageSize = 1024 * 8,
  pageHeader = 40

};

struct pager{
  int nPage;
  int mAlloc;
  struct table* t;
  int writeBuffer;

};

int commitNewPage(const dsqlDb& db, int nPage, struct table* table, int nsection, int sectionSize);
int getPageOffset(u32 nPage, u32* res);
int getPageCount(const dsqlDb& db, int* nPage);
int updatePageCount(const dsqlDb& db , int pageCount);

int getOverflowPageWriteBuffer( const dsqlDb &db, u32 npage, s32* offset);
int getInsertWirteBufferIndex(mPage* page , u32 npage, u32* offset, int size);

int mupOnePage(const dsqlDb& db, mPage* mpage, int npage);

#endif
