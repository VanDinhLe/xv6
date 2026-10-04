#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define NULL (void*)0
enum flags {
  NONE,
  REVERSE,
  NUMERICAL,
  DUPLICATE,
};

// check if a char in a num
int isnum(char p ){
  //printf("char to check is %c\n", p);
  if(p >= '0' && p <= '9'){
    return 1;
  }
  return 0;
}

// customed strcmp
int
my_strcmp(const char *p, const char *q){
  char s1[16] = "";
  char s2[16] = "";
  while(*p && *p == *q)
    p++, q++;
  if(*p == *q)
    return 0;

  int i = 0;
  while(isnum(*p)){
    s1[i] = *p;
    p++;
    i++;
  }
  int j = 0;
  while(isnum(*q)){
    s2[j] = *q;
    q++;
    j++;
  }
  // if i or j is not number or just 1 digit
  // then we can compare first char(behave like strcmp)
  if(!i || !j){ 
    return (uchar)(*(p-i)) - (uchar)(*(q-j));
  }
  
  // convert to number and compare
  i = atoi(s1);
  j = atoi(s2);
  return i - j;
}

// for DBG
char * flags_name[] = {
  "none",
  "reverse",
  "numerical",
  "duplicate",
};

char buf[512];

// account for each line
struct text_line {
  char buf[512];
  struct text_line * next;
};

struct text_line * root = NULL;

// sort text line in linked list
struct text_line *
lex_sort(struct text_line * head, char buff[512], enum flags flag){
  if(head == NULL) {
    head = malloc(sizeof(struct text_line));
    strcpy(head->buf, buff);
    head->next = NULL;
    return head;
  }
  else {
    // duplicate then return
    if(flag == DUPLICATE && strcmp(head->buf, buff) == 0){
      return head;
    }
    // initialize node
    struct text_line * newline = malloc(sizeof(struct text_line));
    strcpy(newline->buf, buff);
    newline->next = NULL;

    // reverse flag
    int reverse_value = 1;
    if(flag == REVERSE){
      reverse_value = -1;
    }
    
    // using customed strcmp for numerical case
    int (*strcmp_ptr)(const char*, const char *) = &strcmp; 
    if(flag == NUMERICAL){
      strcmp_ptr = &my_strcmp;
    }
  
    // for reversing sort mutiply with -1
    if(strcmp_ptr(head->buf, buff) * reverse_value  > 0)
    {
      newline->next = head;
      return newline;
    }

    struct text_line * curr = head;


    while(curr->next != NULL && strcmp_ptr(curr->next->buf, buff) * reverse_value < 0 ){
      // redundant
      if(flag == DUPLICATE && strcmp(curr->next->buf, buff) == 0){
        free(newline);// then remove dup
	return head;
      }

      curr = curr->next;
    }// 1 -> 3 -> 6
    newline->next = curr->next;
    curr->next = newline;
    return head;
  }
}

// customed print linked list
static void 
print(struct text_line * head){
  while(head != NULL){
    printf("%s", head->buf);//already has \n
    head = head->next;
  }
}

// cat is used as based
void
cat(int fd, enum flags flag)
{
  int n;
  int count = 0;
  char line[512];
  //printf("fd num %d\n", fd);

  while((n = read(fd, buf, sizeof(buf))) > 0) {
   // printf("read in \n");
    char * buf_char = &buf[0];
    char * line_char = line;
    while((*buf_char) != '\0'){
      *line_char = *buf_char;
      if(*buf_char == '\n'){
	count++;
	line_char++;
	*line_char = '\0';
	// calling sort
    	root = lex_sort(root, line, flag);

	memset(line, '\0', count);
	count = 0;
	line_char = line;
	buf_char++;
	continue;
      }
      line_char++;
      buf_char++;
    }
  }
  // print list
  print(root);
  if(n < 0){
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;
  enum flags flag = NONE;
  if(argc <= 1){
    cat(0, flag);
    exit(0);
  }
  
  // set flags
  if(strcmp(argv[1], "-r") == 0){
    flag = REVERSE;
  } else if (strcmp(argv[1], "-n") == 0){
    flag = NUMERICAL;
  } else if (strcmp(argv[1], "-u") == 0){
    flag = DUPLICATE;
  }

  // offset for flagging or none cases
  if(flag == NONE){
    i = 1;
  } else {
    i = 2;
  }

  for(; i < argc; i++){
    if((fd = open(argv[i], O_RDONLY)) < 0){
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd, flag);
    close(fd);
  }
  exit(0);
}
