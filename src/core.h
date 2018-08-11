#ifndef COREH
#define COREH

typedef unsigned short u16;
typedef unsigned int u32;

template <class T0, class T1>
class pair{

public:
  T0 first;
  T1 second;

  pair(const T0& first, const T1& second ){
    this->first = first;
    this->second = second;
  }

  // C must be allocated.
  void serializeBuffer(char* c){
    memcpy(c, &first, sizeof(T0));
    memcpy(c + sizeof(T0), &second , sizeof(T1));
  };

  u16 bufferSize(){
    return sizeof(T0) + sizeof(T1);
  };

  void set(const T0& first, const T1& second){
    this->first = first;
    this->second = second;
  }

  void sum(){
    return first + second;
  }

};

enum dbConnectorStatus{
  PENDING = 0,
  SUCCESSFUL = 1,
  FAIL = -1
};


template <class T>
struct dsqlList{
  struct dsqlList<T>* first;
  struct dsqlList<T>* next;
  struct dsqlList<T>* prev;

  T* v;

  dsqlList(dsqlList<T>* toAppend, T* value){
    toAppend->next = this;

    this->prev = toAppend;
    this->first = toAppend->first;
    this->v = value;
    this->next = 0;
  };

  dsqlList(T* value){
    this->v = value;
    this->first = this;
    this->prev = 0;
    this->next = 0;
  }

  dsqlList(){
    this->first = 0;
    this->next = 0;
    this->prev = 0;
    v = 0;
  };

};


typedef pair<u16, u16> uuPair;
typedef int s32;

const char* const dsqlSignature = "DSQL";
const u16 signatureSize = 4;

const u16 pageInfo = 10;

const u16 headerSize = 100;
const u16 schemaInfo = headerSize;
const u16 schemaDescriptionSize = 100;
const u16 pageStart = headerSize + schemaDescriptionSize;
const u16 schemaIndexInfo = 80;

const s32 emptySignature = 65;

template <class T>
int releaseLinkedList(struct dsqlList<T>* list, bool removeValue){
  struct dsqlList<T>* iter = list;
  struct dsqlList<T>* temp;

  if(list->first != iter){
    for(; iter ; iter = iter->prev){
      if(iter == list->first){
        iter = list->first;
        break;
      }
    }
  }

  for(;iter;){
    if(removeValue){
      delete iter->v;
    };

    temp = iter->next;
    delete iter;
    iter = temp;
  }

  return 0;
}

template<class T>
int appendOrInitList(struct dsqlList<T>* list, T* value){
  if(!list->v){
    *list = dsqlList<T>(value);
  }

  else{
    if(!list->next){
      new dsqlList<T>(list, value);
    }

    else{
      struct dsqlList<T>* temp = list->next;
      new dsqlList<T>(list, value);

      list->next->next = temp;
      temp->prev = list->next;
    }
  }

  return 0;
};

#endif
