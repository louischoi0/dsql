#include "core.h"

if(strncmp(c, "CREATE", 6) != 0){
int parseCreateQuery(const char* c){
    return 1;
  }

  if(strncmp(c[7], "TABLE" ,5) != 0){
    return 1;
  }
}
