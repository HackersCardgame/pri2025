
#include <stdio.h>
#include <string.h>

#define _DEBUG
#ifdef _DEBUG
#define lsize 255 /* const int lsize=255; -- C++ */
struct locs {
  int   line[lsize];
  char *file[lsize];
  char *func[lsize];
  int  cur; /* cur=0; C++ */
} locs;

#define track do {\
      locs.line[locs.cur]=__LINE__ ;\
      locs.file[locs.cur]=(char*)__FILE__ ;\
      locs.func[locs.cur]=(char*) __builtin_FUNCTION() /* __PRETTY_FUNCTION__ -- C++ */ ;\
      locs.cur=(locs.cur+1) % lsize;\
  } while(0);

void track_start(){
  memset(&locs,0, sizeof locs);
}

void track_print(){
  int i, k;
  for (i=0; i<lsize; i++){
    k = (locs.cur+i) % lsize;
    if (locs.file[k]){
      fprintf(stderr,"%d: %s:%d %s\n",
        k, locs.file[k],
        locs.line[k], locs.func[k]);
    }
  }
}
#else
#define track       do {} while(0)
#define track_start() (void)0
#define track_print() (void)0
#endif


// Sample usage.
void bar(){ track ; }
void foo(){ track ; bar(); }

int main(){
  int k;
  track_start();
  for (k=0;k<2;k++)
    foo();
  track;
  track_print();
  return 0;
} 
