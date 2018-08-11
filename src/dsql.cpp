#include <iostream>
#include <assert.h>
#include <iostream>
#include "dsql.h"
#include "macro.h"
#include <cstring>
#include "pager.h"

int checkDabaBaseInitialized(const char* ){

}


int write(char* buffer, int off, int size){
  return 0;
}

int macro_cmp(char* buffer, int size, int src, int dOff){
  int i;
  int d = src;
  for( i = 0; i < size; i++, d += dOff){
    continue;
  }

}


int checkDataBaseOk(const char* c){
  char* signature;
  dsqlCoreAlloc(&signature, 4);

  int rc = _dsqlReadFile(c, signature,4,0);
  if(rc){
    dsqlDebugPrint("DataBase Not Exists.");
    return 1;
  }

  if ( strncmp(signature, dsqlSignature,4) != 0){
    dsqlDebugPrint("Signature Not Matched");
    return 1;
  }

  return 0;
}

int databaseFirstOpen(const dbConnector& db){
  int rc;
  char dummy[200];
  u32 nPage = 0;

  dsqlDebugPrint("databaseFirstOpen");

  rc = dsqlCreateFile(db.dbName);
  dsqlDebugPrint(db.dbName);

  // Write DataBase Format Signature and Dummy Buffer.
  rc = dsqlWriteFile(db, dsqlSignature, 4, 0);
  rc = dsqlWriteFile(db, (char*)&dummy, 200, 4);

  // Write Page Count (0).
  rc = dsqlWriteFile(db, (char*)&nPage , 4, pageInfo);

  if(rc){
    dsqlDebugPrint("Wirte File Failed At databaseFirstOpen");
  }

  schemaFieldInitialize(db);
  dsqlDebugPrint("DataBase Initialized");
  return 0;

}

typedef dbConnector db;

int loadSchemas(const db& db, dbInfo* dbInfo){
  u16 schemas = 0;
  dsqlReadFile(db, (char*)(&schemas), 2, schemaInfo);
  schemas = get2Byte((char*)&schemas);
  dsqlDebugPrint("loadSchemas****************");

  dbInfo->nschema = schemas;

  std::cout << std::endl << "Schemas : " << schemas << std::endl;
};


int openDataBase(const char* c){
  dbConnector dbase;
  dbase.dbName = c;
  dbase.status = dbConnectorStatus::PENDING;
  u16 rc = 0;

  if(checkDataBaseOk(c)){
    rc = databaseFirstOpen(dbase);
  }

  return rc;
};

void schemaAppendField(struct schema* shm , uuPair* pair){
  auto nshm = new struct dsqlList<uuPair>(shm->last, pair );
  shm->nschema++;
  shm->last = nshm;
};

void schemaInitialize(struct schema* shm, uuPair* upairs, int nfield ){
  int i;
  uuPair* uiter = upairs;
  struct dsqlList<uuPair>* liter = &shm->fieldDiscription;

  shm->nschema++;
  shm->fieldDiscription = dsqlList<uuPair>(uiter++);

  for(i = 0; i < nfield - 1 ; i++){
      schemaAppendField(shm, uiter++);
  };

};

int commitSchemaIndex(const dbConnector& connector, int id, int bstart){
  dsqlReadFile(connector, (char*)&bstart, sizeof(u32), schemaIndexInfo + ( id * sizeof(u32) ));
  return 0;
};

int commitSchema(const dbConnector& connector, const char* buffer, int id){

#define rBufferLen 100
  u16 rc;

  u16 schemas;
  u16 bufferLen = strlen(buffer);
  u16 fieldOffset;
  u16 bstart;
  u16 reserve = 4;
  u16 dscFileds;
  uuPair* uup;

  rc = dsqlReadFile(connector, (char*)&schemas, sizeof(u16), schemaInfo);

  schemas = get2Byte((char*)&schemas);
  schemas++;

  getSchemaStartBufferOffset(connector,&bstart);
  commitSchemaIndex(connector, id, bstart);

  std::cout << "Buffer Offset ::";
  std::cout << bstart << std::endl;

/****** Wip******/
  dsqlCoreAlloc((char**)&uup, sizeof(uuPair) * 3);

  uup[0].set(0,4);
  uup[1].set(0,4);
  uup[2].set(1,10);

/****** Wip******/

  commitSchemaFieldInfos(connector, 0,uup, 3, &fieldOffset);
  //Write Buffer Size
  dsqlWriteFile(connector, (char*)&bufferLen,sizeof(u16), schemaInfo + bstart );
  // Write Schema Buffer to Bstart offset
  dsqlWriteFile(connector, buffer, bufferLen, schemaInfo + bstart + 2);
  // Append Reserve space
  updateBstart(connector, bstart + fieldOffset + 4 + 2);

  return 0;
};

int updateBstart(const dbConnector& connector, u16 bstart){
  dsqlWriteFile(connector, (char*)&bstart, sizeof(u16), schemaInfo + 2);
  return 0;
};

int getSchemaStartBufferOffset(const dbConnector& connector, u16* bstart){
  dsqlReadFile(connector, (char*)bstart, 2, schemaInfo + 2);
  return 0;
};

int getNewChemaBuffer(const dsqlDb& db,u16& bufferOffset){
  dsqlReadFile(db, (char*)&bufferOffset, sizeof(u16) , schemaInfo);
}

int getAllFieldDescriptionSize(uuPair* pairs , u16 nfield){
  u16 allSize = 0;
  int i;
  uuPair* uuit = pairs;

  for(i = 0; i < nfield; i++, uuit++){
    allSize += uuit->second;
  };

  return allSize + 4;
}

int getFieldSizeDescription(char* buffer , uuPair* uu, int count){
  int i;
  int p;
  uuPair* uuiter = uu;

  for(i = 0 , p = 0; i < count; i++ , p += 2 * sizeField::U16 ){
    memcpy(buffer, &uuiter->first, sizeField::U16 );
    memcpy(buffer + sizeField::U16, &uuiter->second, sizeField::U16);
    uuiter++;
  }
}


int commitSchemaFieldInfos(const dbConnector& db,int id, uuPair* arr, u16 nfield , u16* fieldOffset){
  char* wBuffer;
  u16 pointer = 0;
  int i = 0;
  uuPair* uuit;
  u16 nSchema;

  // For Schema String buffer
  *fieldOffset = getAllFieldDescriptionSize(arr, nfield);

  // For Schema Field Discription
  *fieldOffset += sizeof(u16) * nfield * 2;

  u16 rc = dsqlCoreAlloc(&wBuffer, *fieldOffset);
  memcpy(wBuffer + pointer, &nfield, sizeof(u16));
  pointer += 2;
  memcpy(wBuffer + pointer, fieldOffset, sizeof(u16));
  pointer += 2;

  getFieldSizeDescription(wBuffer + pointer, arr, nfield);
  pointer += sizeof(u16) * nfield * 2;

  dsqlDebugPrint("commit Schema Field Infomations ");

  std::cout <<  " fields :: " <<  nfield << " , ";
  std::cout << *fieldOffset << std::endl;

  for(uuit = arr; i < nfield ; uuit++, i++){
    pointer += uuit->second;
  };

  std::cout << "Pointer and Write Size :: " << pointer << " , " <<  " , " << *fieldOffset << std::endl;
  getSchemaCount(db, &nSchema);
  std::cout << "New Schemas : " << nSchema << std::endl;
  updateSchemaCount(db, nSchema + 1);

  assert(pointer == *fieldOffset);
  return 0;
}

int initSchemaStruct(const struct schema sch, int schemaId){


};

int updateSchemaCount(const dbConnector& db, u16 newSchema){
  dsqlWriteFile(db, (char*)&newSchema, sizeof(u16), schemaInfo);
  return 0;
}

int getSchemaCount(const dbConnector& db, u16* nSchema){
  char temBuffer[2];
  dsqlReadFile(db, (char*)temBuffer, sizeof(u16), schemaInfo);
  *nSchema = get2Byte(temBuffer);
  return 0;
};

// In Case DataBase File First Opend.
// This function set Schema field counter.
int schemaFieldInitialize(const dbConnector& db){
  u16 schemas = 0;
  u16 bufferOffset = sizeof(u16) * 2;

  char* buffer;
  dsqlCoreAlloc(&buffer, 4);
  memcpy(buffer, &schemas, 2);
  memcpy(buffer+2, &bufferOffset, 2);

  dsqlWriteFile(db, buffer, 4, schemaInfo );
  dsqlCoreFree(buffer);
  return 0;
}

int commitSchemaRecord(mPage* mpage, const struct schema &schema ){
  u32 idx;
  getInsertWirteBufferIndex( mpage, 1, &idx, mpage->getSectionSize() );
  assert(idx >= 0);
  return 0;
};

int mPage::writeRecordBuffer(char* buffer, int bufferSize){

  int i;
  int nrecord = this->nrecord;
  char* pbuffer = buffer;
  char* wbuffer = this->buffer + nrecord;

  for(i = 0; i < bufferSize; i++){
    *wbuffer = *pbuffer++;
    wbuffer += getSectionSize();
  };

  this->nrecord++;
  return 0;

};

int mPage::getRecordByRowid(u32 id, char* buffer, int bufferSize){
  int i;
  char* pbuffer = this->buffer;
  char* wbuffer = buffer;

  for(i = 0; i < bufferSize; i++){
    *wbuffer = *pbuffer;
    wbuffer ++;
    pbuffer += getSectionSize();
  };

  return 0;

};

int updateRecordCount(const dsqlDb db, mPage* page){
  u32 record = page->getRecordCount();
  dsqlWriteFile(db, (char*)&record, 4, page->getPageOffset() + 4);
  return 0;
}

int reflectMemoryPageToDb(const dsqlDb& db, mPage* mpage){
  u32 offset = mpage->getPageOffset();
  u32 record;
  updateRecordCount(db, mpage);
  dsqlWriteFile(db, mpage->bufferPointer() , pageConfig::pageSize - pageConfig::pageHeader , offset + pageConfig::pageHeader);

  return 0;
};
