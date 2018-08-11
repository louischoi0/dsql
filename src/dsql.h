#ifndef DSQLH
#define DSQLH

#include "core.h"

typedef class dbInfo;
typedef struct dbConnector;
typedef struct dbConnector dsqlDb;
typedef struct schema;
typedef struct mPage;

int _dsqlWriteFile(const char* name, const char* buffer, u16 size, u16 offset);
int dsqlWriteFile(const dsqlDb& db, const char* buffer, u16 size, u16 offset);

int _dsqlReadFile(const char* name, char* dest, u16 size, u16 offset);
int dsqlReadFile(const dsqlDb& db, char* dest, u16 size, u16 offset);

int dsqlCoreAlloc(char** src, int size);
int dsqlCoreFree(char*);
int dsqlCoreMcopy(char* dest, char* src, int size);
int dsqlStrCmp(char* c0, char* c1);
int dsqlCreateFile(const char* name);
int loadSchemas(const dsqlDb& db, dbInfo* dbInfo);
int schemaFieldInitialize(const dbConnector& db);
int commitSchemaFieldInfos(const dbConnector& db,int id, uuPair* arr, u16 nfield , u16* fieldOffset);
int commitSchema(const dbConnector& connector, const char* buffer);
int updateBstart(const dbConnector& connector, u16 bstart);

int getSchemaCount(const dbConnector& db, u16* rBuffer);
int updateSchemaCount(const dbConnector& db, u16 newSchema);

int getSchemaStartBufferOffset(const dbConnector& connector, u16* bstart);
int openDataBase(const char* c);

int reflectMemoryPageToDb(const dsqlDb& db, mPage* mpage);
void schemaInitialize(struct schema* shm, uuPair* upairs, int nfield );
int updateRecordCount(const dsqlDb db, mPage* page);

  #ifndef NO_DEBUG
    void dsqlDebugPrint(const char* massage);
  #else
    #define dsqlDebugPrint(x)
  #endif

int commitSchema(const dbConnector& connector, const char* buffer);

typedef struct dbConnector{
  const char* dbName;
  const char* path;
  dbConnectorStatus status;

  dConnector(const char* name){
    dbName = name;
    status = dbConnectorStatus::PENDING;
  }
} dbConnector;

enum dataTypes{
  DINTEGER = 0,
  DVARCHAR = 1,
  DBLOB = 2,
  DCHAR = 3
};

class dbInfo {
public :
  dbConnector* c;

  int pageLen;
  int sAmount;
  u16 nschema;
  //schema* schemas;
  char** schemaBuffers;

  dbInfo() :
    pageLen(-1),
    sAmount(-1),
    nschema(0)
  {

  };

};

class mPage{
  u32 ntable;
  u32 nrecord;
  int bufferLen;
  char* buffer;
  u32 npage;
  u32 sectionSize;
  u16 overflow;
  u32 nsection;
  u32 pageOffset;

public:
  mPage() :
    ntable(-1),
    nrecord(0),
    buffer(0),
    npage(-1),
    sectionSize(-1),
    overflow(0),
    nsection(-1),
    pageOffset(-1)
  {

  };

  mPage(int nsection, int size) :
    ntable(-1),
    nrecord(0),
    npage(-1),
    sectionSize(size),
    overflow(0),
    nsection(-1),
    pageOffset(-1)
  {
      dsqlCoreAlloc(&this->buffer, nsection * size + 12);
  };

  int writeRecordBuffer(char* buffer, int bufferSize);
  int getRecordByRowid(u32 id, char* buffer, int bufferSize);

  char* bufferPointer() { return buffer; };

  u32 getSectionCount() { return this->nsection; };
  void setSectionCount(u32 scount ) { this->nsection = scount; };

  int getSectionSize() { return sectionSize; };
  void setSectionSize(u32 ssize) { this->sectionSize = ssize; };

  bool pageInited() { return !(ntable == -1 || nrecord == -1 || bufferLen == -1 || buffer == 0 || sectionSize == -1); };

  void setPageIndex(u32 i) { this->npage = i; };
  u32 getPageIndex() { return this->npage; };

  void setRecordCount( u32 r ) { this->nrecord = r; };
  u32 getRecordCount() { return this->nrecord; };

  void setOverFlowFlag(u16 v ) {  this->overflow = v; };
  void allocateBuffer(u32 size) { dsqlCoreAlloc(&this->buffer, size); };
  int initFieldBuffer();

  void setPageOffset(u32 offset) { this->pageOffset = offset; };
  u32 getPageOffset() { return this->pageOffset; };

};

#define PAGE_BLOCK_HEADER 12

class uCursor{
  mPage* pMpage;
  char* pBuffer;

  int nPage;
  int cNo;
  // pos represent x + ( y * nRow );
  int pos;
  int posX;
  int posY;

  int allocated;

public :
  uCursor(const dsqlDb& db, int npage, int section, int size) :
    nPage(npage),
    cNo(section)
  {
    this->memoryUpCursorBuffer(db,npage, section,size);
  };

  uCursor() :
    pMpage(0),
    pBuffer(0),
    allocated(-1),
    nPage(-1)
  {

  };

  int move_x(int off);
  int move_y(int off);

  int memoryUpCursorBuffer(const dsqlDb& db, int npage, int section, int size);

};

struct cursorPool{
  int ncursor;
  struct dsqlList<uCursor>* list;
  struct dsqlList<uCursor>* last;
};

struct schema{
  u32 id;
  dsqlList<uuPair> fieldDiscription;
  char* schemaBuffer;
  dsqlList<uuPair>* last;
  int nschema;
  char* schemaName;

  schema() :
    id(-1),
    schemaBuffer(0),
    last(&this->fieldDiscription),
    nschema(0),
    schemaName(0)
  {

  };


};

struct table{
  struct schema* schema;
  int pageTowrite;
};

enum sizeField{
  U16 = 2,
  U32 = 4
};


#endif
