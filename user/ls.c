#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define NULL ((void*)0)

char*
fmtname(char *path, short type)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p)+1);
  if(type == 1){ // add / to T_DIR (/ will be replaced by null if longer than 14
    buf[strlen(p)] = '/';
  }
  buf[sizeof(buf)-1] = '\0';
  return buf;
}

// a node in linked list
struct file_node {
    struct stat st;
    struct file_node* next;
    char name[DIRSIZ+1];
};

struct file_node* head = NULL;

// DBG: print out size of list
void size_check(struct file_node * root){
  int count = 0;
  struct file_node * curr = root;
  while(curr != NULL) {
    curr = curr->next;
    count++;
  }
  printf("size of list file dir is %d\n", count);
}

// add instance to list decreasing order
void file_sort(struct file_node ** root, struct stat st, char * name)
{
  // skip hidden files
  if(*name == '.') {
    //printf("hidden file node\n");
    return;
  }
  if(*root == NULL) {
    //printf("empty node\n");
    *root = malloc(sizeof(struct file_node));
    (*root)->next = NULL;
    (*root)->st = st;
    memset((*root)->name, 0, sizeof((*root)->name));
    strcpy((*root)->name, name);
    //printf("%p - %p\n", head, root);
  }
  else{
    struct file_node * node = malloc(sizeof(struct file_node));
    node->next = NULL;
    node->st = st;
    memset(node->name, 0, sizeof(node->name));
    strcpy(node->name, name);
    if(st.size > (*root)->st.size){
      node->next = *root;
      *root = node;
      //printf("insert biggest\n");
      return;
    }
    struct file_node * curr = *root;
    while (curr->next != NULL && st.size <= curr->next->st.size) {
        curr = curr->next;
    }

    // Insert the node
    node->next = curr->next;
    curr->next = node;
  }
};

// print list
void print_file(struct file_node * root){
  //printf("######### start printing node ########\n");
  struct file_node * curr = root;
  while(curr != NULL){
    printf("%s %d %d %d\n", curr->name, curr->st.type, curr->st.ino, (int) curr->st.size);
    curr = curr->next;
  }
}

// clean list
void clean_node(struct file_node * root){
  struct file_node * temp = root;
  struct file_node * curr = root;
  while(curr != NULL) {
    temp = curr;
    curr = curr->next;
    free(temp);
  }
  root = NULL;
}

void
ls(char *path, int sort)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    char *f = fmtname(path, st.type);
    // skip hidden
    if(*f == '.') {
	break;
    }
    // if -s case
    if(sort){
      file_sort(&head, st, fmtname(path, st.type));
      // size_check(head);
    }
    else {
      printf("%s %d %d %d\n", fmtname(path, st.type), st.type, st.ino, (int) st.size);
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      // skip hidden
      if(*(de.name) == '.'){
	continue;
      }
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
     // if flag -s detected
     if(sort){
        file_sort(&head, st, fmtname(buf, st.type));
        //size_check(head);
	//print_file(head);
      }
     else{
       printf("%s %d %d %d\n", fmtname(buf, st.type), st.type, st.ino, (int) st.size);
     }
   }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".", 0);
    exit(0);
  }
  // detecting flag s
  else if(strcmp(argv[1], "-s") == 0){
    if(argc < 3){
     ls(".", 1);
     print_file(head);
     exit(0);
    }
    for(i=2; i<argc; i++)
      ls(argv[i], 1);
    print_file(head);
    clean_node(head);
    exit(0);
  }
  else {
    for(i=1; i<argc; i++)
      ls(argv[i], 0);
  }
  exit(0);
}
