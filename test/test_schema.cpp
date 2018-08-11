#include "../src/dsql.h"
#include "../src/macro.h"
#include "../src/pager.h"
#include <cstring>
#include <iostream>

void check_mpage(mPage* page){
  std::cout << "Page Index : " << page->getPageIndex() << std::endl;
  std::cout << "nrecord : " << page->getRecordCount() << std::endl;
  std::cout << "Section Count : " << page->getSectionCount() << std::endl;
  std::cout << "Section Size : " << page->getSectionSize() << std::endl;
};

int testSchemaInfos(const dsqlDb& d){
  u16 schema = 0;
  u16 bufferOffset = 0;
  char* buffer;

  dsqlCoreAlloc(&buffer, sizeof(u16) * 2);
  dsqlReadFile(d, buffer, sizeof(u16) * 2, schemaInfo);

  schema = get2Byte(buffer);
  buffer += 2;
  bufferOffset = get2Byte(buffer);
  buffer -= 2;

  dsqlCoreFree(buffer);
  std::cout << std::endl;
  std::cout << "schema Infos ************" <<std::endl;
  std::cout << "nschema :: ";
  std::cout << schema << std::endl;
  std::cout << "bufferOffset :: ";
  std::cout << bufferOffset << std::endl;
}

void shemaInfo(const schema& s){
  int i;
  auto iter = &s.fieldDiscription;
  std::cout << "This schemas struct have " << s.nschema << " Columns" << std::endl;
  for( i = 0; i < s.nschema; i++){
    std::cout << i << " Field : ";
    std::cout << iter->v->first << " , " << iter->v->second << std::endl;
    iter = iter->next;
  };

}

void test_open_database(const char* db){
  dsqlDebugPrint("Debug Routine Start");
  std::cout << std::endl;
  openDataBase(db);
};

void test_mpage_buffer_read_write(mPage* page , int section, int size, char* buffer, int bufferSize,int id){

  char* rbuffer = (char*)malloc(bufferSize);

  page->writeRecordBuffer(buffer, bufferSize);
  page->getRecordByRowid(id, rbuffer, bufferSize);

  std::cout << "readBuffer :: " << rbuffer << std::endl;
  check_mpage(page);
};


void test_commit_page(const dbConnector& db, int npage, struct table* tb, int nsc, int secs){
  int result;
  dsqlDebugPrint("Commit New page to database file.");
  commitNewPage(db, npage, tb, nsc, secs);
  getPageCount(db, &result);

  std::cout << "Page Count :: " << result << std::endl;

};

void test_memory_up_page(const dsqlDb& db, mPage** mpage, int npage ){
  *mpage = new mPage();
  mupOnePage(db, *mpage, npage );
  check_mpage(*mpage);
};

void test_get_record(mPage* mpage){
  u32 nrecord = mpage->getRecordCount();
  int i;

  char* buffer;
  dsqlCoreAlloc(&buffer, 15);

  for( i = 0 ; i < nrecord; i ++ ){
    mpage->getRecordByRowid(i,buffer,15);
    std::cout << buffer << std::endl;
  }

};


int main(){

  dbConnector db;
  db.dbName = "./hi.db\0";
  dbInfo* di = new dbInfo();
  struct table tb;
  s32 ex = 1234;
  s32 exx = 1234551;
  s32 res;

  mPage* mpage0;
  mPage* mpage1;

  test_open_database("./hi.db\0");
  commitNewPage(db, 0, 0, 200,40);
  test_memory_up_page(db,&mpage0,0);

  char* a = (char*)malloc(20);
  memcpy(a, (void*)"dsql bufferss 0", 15);

  test_mpage_buffer_read_write(mpage0, 200, 40, a, 15,0);
  memcpy(a, (void*)"dsql bufferss 1", 15);
  test_mpage_buffer_read_write(mpage0, 200, 40, a, 15,1);

  reflectMemoryPageToDb(db, mpage0);
  test_memory_up_page(db, &mpage0, 0);

  test_get_record(mpage0);

  return 0;
}
