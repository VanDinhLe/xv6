// Simple grep.  Only supports ^ . * $ operators.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// bitwise enum flags
enum flags {
  NONE		= 0,
  NOCASE	= 1 << 0 ,
  LINE_NUM	= 1 << 1 ,
  INVERT	= 1 << 2 ,
};

// turn on the flags
enum flags
set_flag(const char flag[]){
  enum flags res = NONE;
  int i = 1;// skip dash -
  while(flag[i] != '\0'){
    if(flag[i] == 'i'){
      res |= NOCASE;
    }
    else if(flag[i] == 'n'){
      res |= LINE_NUM;
    }
    else if(flag[i] == 'v'){
      res |= INVERT;
    }
    else{
     printf("invalid flag\n");
     return NONE;
    }
    i++;
  }
  return res;
}

// DBG: print flags
void
check_flag(enum flags flag){
  if(flag & NOCASE){
    printf("set -i\n");
  }
  if(flag & LINE_NUM){
    printf("set -n\n");
  }
  if(flag & INVERT){
    printf("set -v\n");
  }
  if(flag == NONE){
    printf("no flag\n");
  }
}

// lower all letter in string
char
* tolowerstr(char * str){
  char * temp = str;
  while(*temp != '\0' && *temp != '\n'){
    if(*temp > 64 && *temp < 91) {
      *temp += 32;
    }
    temp++;
  }
  return str;
}

// lower a letter
char
tolowerchar(char str){
  if(str != '\0' && str != '\n'){
    if(str > 64 && str < 91) {
      str += 32;
    }
  }
  return str;
}

char buf[1024];
int match(char*, char*, enum flags);

void
grep(char *pattern, int fd, enum flags flag)
{
  int n, m;
  char *p, *q;
  int linenum = 0, invert_bit = 0;
  
  // inverse search 
  if(flag & INVERT){
    invert_bit = 1; // toggle bit
  }

  // lowercase
  if(flag & NOCASE){
    pattern = tolowerstr(pattern); // lower the pattern
  }

  m = 0;
  while((n = read(fd, buf+m, sizeof(buf)-m-1)) > 0){
    m += n;
    buf[m] = '\0';
    p = buf;
    // loop through each line
    while((q = strchr(p, '\n')) != 0){
      if(flag & LINE_NUM){
        linenum++;
      } 
      *q = 0;
      // using XOR to toggle inverse search
      if(match(pattern, p, flag) ^ invert_bit){
        *q = '\n';
	if(flag & LINE_NUM) printf("%d:\t", linenum); // line number
        write(1, p, q+1 - p);
      }
      p = q+1;
    }
    // if there is unprocessed letters left, move it at the beginning
    // if line is longer than buffer then it might miss the pattern if in first half
    if(m > 0){
      m -= p - buf;
      memmove(buf, p, m);
    }
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;
  char *pattern;

  enum flags flag = NONE;

  if(argc <= 1){
    fprintf(2, "usage: grep pattern [file ...]\n");
    exit(1);
  }
  
  // check for flags
  if(argv[1][0] == '-'){
    flag = set_flag(argv[1]);
  }

  //check_flag(flag);
  int flag_offset = 0;
  if(flag != NONE){
    flag_offset = 1; // add offset if found flags
  }

  pattern = argv[1 + flag_offset];

  if(argc <= 2 + flag_offset){
    grep(pattern, 0, flag);
    exit(0);
  }

  for(i = 2 + flag_offset; i < argc; i++){
    if((fd = open(argv[i], O_RDONLY)) < 0){
      printf("grep: cannot open %s\n", argv[i]);
      exit(1);
    }
    grep(pattern, fd, flag);
    close(fd);
  }
  exit(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

int matchhere(char*, char*, enum flags);
int matchstar(int, char*, char*, enum flags);

int
match(char *re, char *text, enum flags flag)
{
  if(re[0] == '^')
    return matchhere(re+1, text, flag);
  do{  // must look at empty string
    if(matchhere(re, text, flag))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text, enum flags flag)
{
  if(flag & NOCASE) {
    re[0] = tolowerchar(re[0]);
    *text = tolowerchar(*text);
  }
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text, flag);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1, flag);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text, enum flags flag)
{
  if(flag & NOCASE) {
    c = tolowerchar(c);
    *text = tolowerchar(*text);
  }
  do{  // a * matches zero or more instances
    if(matchhere(re, text, flag))
      return 1;

    if(flag & NOCASE) {
      *text = tolowerchar(*text);
    }
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}

