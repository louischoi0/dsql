#include "dsql.h"
#include "pager.h"
#include <assert.h>

int uCursor::memoryUpCursorBuffer(const dsqlDb& db, int npage, int section, int size){
  u32 pageOffset;
  int cursorReadOffset = (section * size) + pageConfig::pageHeader;

  getPageOffset(npage,&pageOffset);

  assert( pageOffset >= 0 );
  assert( this->allocated >= -1 );

  if(this->allocated == -1 || this->allocated < size ){
    if(this->allocated != -1 ){
      dsqlCoreFree(this->pBuffer);
    }
    dsqlCoreAlloc(&this->pBuffer, size);
    this->allocated = size;

    dsqlReadFile(db, this->pBuffer, size, pageOffset + cursorReadOffset);
  }

  return 0;
}

// The ncursor represent section count.
// Cpool struct must be already allocated.
int initializeCursorPool(const dsqlDb& db, cursorPool* cpool, int npage, int ncursor, int size){
  int i;
  cpool->list = new struct dsqlList<uCursor>();

  for( i = 0; i < ncursor ; i++){
    appendOrInitList(cpool->list, new uCursor(db,npage,i,size));
  };

};

int uCursor::move_x(int off){
  int cpos = this->posX + off;
  this->posX = cpos;
  this->pos += off;

  return 0;
};

int uCursor::move_y(int off){

  return 0;
}
