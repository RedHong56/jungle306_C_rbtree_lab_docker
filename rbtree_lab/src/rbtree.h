#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;
////////// 키 값 : int /////////////////////
typedef int key_t;

/////////// 트리의 노드 구조체 선언/////////////
typedef struct node_t {
  color_t color; // 색
  key_t key; // 카 겂
  struct node_t *parent, *left, *right; // 부모노드, 자식노드
} node_t;

//////////////////// Tree 구조체 선언/////////////////
typedef struct { 
  node_t *root; //루트 노드
  node_t *nil;  // for sentinel
} rbtree;

///////////////////
rbtree *new_rbtree(void); 
void delete_rbtree(rbtree *);

node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *);

int rbtree_to_array(const rbtree *, key_t *, const size_t);

#endif  // _RBTREE_H_
