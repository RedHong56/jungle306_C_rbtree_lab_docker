#include "rbtree.h" // 구조체,

#include <stdlib.h>

rbtree *new_rbtree(void) {
  //calloc(contiguous allocation)이란? 메모리를 동적으로 할당하면서 0으로 초기화 해주는 함수
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree)); // 여기서 1은 할당할 요소의 개수-> rbtree 하나 만들겠다
  // TODO: initialize struct if needed
  p->nil = calloc(1, sizeof(node_t)); // nil 사이즈의 동적 메모리를 할당하며 할당한 주소의 값을 0으로 초기화??
  p->nil->color = RBTREE_BLACK; // 노드를 할당했으니 노드에 대한 초기화 -> 색(검은색)
  p->root = p->nil; // 루트가 없을 땐 방금 만들어낸 NIL 가르키게
  return p;
}
// new_rbtree 1트///
// 1. root 노드 초기화
  // p->root = NULL; 이렇게 하는거 아님
  // 2. nil 노드 초기화
  // p->nil->color = 1; 이렇게 하면 나중에 코드 읽을때 불편
//////////////////////* DELETE *///////////////////////////////////////////
void delete_rbtree(rbtree *t) 
{
  // TODO: reclaim the tree nodes's memory
  if (t == NULL || t->root == NULL) return;
  node_t *cur = t->root, *parents;
  // 1. 트리 안에 있는 모든 노드들을 반환후
  while (cur != t->nil) // 널 포인터와 논리적의미가 다름
  {
    if(cur->left != NULL) // 자식이 있는 경우(왼쪽 자식부터)
    {
      cur = cur->left;
    }
    else if (cur->right != NULL) // 오른쪽 자식이 있으면
    {
        cur = cur->right;
    }
    else // 왼쪽 자식도 없고 오른쪽 자식도 없다면
    { // 삭제 조건 : nil, nil 일 때 free 하고 현재 보고있는 노드에서 부모노드로 이동
      parents = cur->parent;
      // free 전에 자식 포인터 정리 해줘야함
      if (parents->left == cur) parents->left = t->nil;
      else if ( parents->right == cur) parents->right = t->nil;
      free(cur);
      cur = parents; // 메모리 누수 주의? -> 삭제해도 쓰레기 값이 있기 때문에
    }
  }
// 루트의 nil 노드도 반환해줘야함
free(t->nil);
// 2. 트리 반환
free(t);
}
////////////// Left - Rotate 의사 코드 작성//////////////////
void rotate_L(rbtree *T, node_t *x) // n = 부모
{
  node_t *y = x->right; // 자식 노드를 지정
  x->right = y->left;  // x의 오른쪽 자식을 y의 왼쪽 자식으로 설정

  // y의 왼쪽 자식 감별
  if(y->left != T->nil) y->left->parent = x->parent;

  y->parent = x->parent;// y 부모를 x의 부모로 설정

  // x 부모 판별에 따른 y 위치 이동
  if(x->parent == T->nil) T->root = y;
  else if (x == x->parent->left) x->parent->left = y;
  else x->parent->right = y;

  // 최종 위치 이동술
  y->left = x;
  x->parent = y;
  return;
}
////////////// Right - Rotate 의사 코드 작성//////////////////
void rotate_R(rbtree *T, node_t *x)
{
  node_t *y = x->left;
  x->left = y->right;  

  if(y->right != T->nil) y->right->parent = x->parent;

  y->parent = x->parent;

  if(x->parent == T->nil) T->root = y;
  else if (x == x->parent->right) x->parent->right = y;
  else x->parent->left = y;

  y->right = x;
  x->parent = y;
  return;
}
/////////////////////////////* INSERT-FIXUP */////////////////////////////////////////
void insert_fixup(rbtree *t, node_t *cur) //부모노드
{
  node_t *uncle;
  while (uncle->parent->color == RBTREE_RED) // 부모 색이 빨간색이 아니면 -> 4번 조건 통과
  {
    if(cur->parent == cur->parent->parent->left)// 부모가 왼쪽 자식인가?
    {
      uncle = cur->parent->parent->right; // 삼촌 정의
      // CASE 1: 삼촌 = 빨
      if (uncle->color == RBTREE_RED){ // 삼촌색 = RED
        cur->parent->color = RBTREE_BLACK; // 아부지-> black
        uncle->color = RBTREE_BLACK; // 삼촌색 -> black
        cur->parent->parent = RBTREE_RED; // 할아부지 -> red
        cur = cur->parent->parent; // cur = 할아부지로 업데이트
      }
      else{//삼촌 = 깜
        // CASE 2: 삼촌 = 깜 , 꺾
        if (cur == cur->parent->right) // 꺾 : 아부지가 할부지 왼쪽에 있는데 나는 아부지 오른쪽
        {
          cur = cur->parent;
          rotate_L(t, cur); // 아부지 기준 회전 하는데 Why 왼쪽?
        }
        // CASE 3: 삼촌 = 깜, 노꺾
        else // 노꺾 : 아부지가 할부지 왼쪽 이면서 나도 아부지의 왼쪽
        {
          cur = RBTREE_BLACK;
          cur->parent->parent = RBTREE_RED;
          // cur = cur->parent->parent; -> 왜 할부지로 cur 업데이트 안함?: 자식에서 더 해결해야하기 때문
          rotate_R(t, cur->parent->parent); 
        }
      }
    }
    else // 부모가 오른쪽 자식인가?
    {
      uncle = cur->parent->parent->left; // 삼촌 정의
      // CASE 1: 삼촌 = 빨
      if (uncle->color == RBTREE_RED){ // 삼촌색 = RED
        cur->parent->color = RBTREE_BLACK; // 아부지-> black
        uncle->color = RBTREE_BLACK; // 삼촌색 -> black
        cur->parent->parent = RBTREE_RED; // 할아부지 -> red
        cur = cur->parent->parent; // cur = 할아부지로 업데이트
      }
      else{//삼촌 = 깜
        // CASE 2: 삼촌 = 깜 , 꺾
        if (cur == cur->parent->left) // 꺾 : 아부지가 할부지 왼쪽에 있는데 나는 아부지 오른쪽
        {
          cur = cur->parent;
          rotate_L(t, cur); // 아부지 기준 회전 하는데 Why 왼쪽?
        }
        // CASE 3: 삼촌 = 깜, 노꺾
        else // 노꺾 : 아부지가 할부지 왼쪽 이면서 나도 아부지의 왼쪽
        {
          cur = RBTREE_BLACK;
          cur->parent->parent = RBTREE_RED;
          // cur = cur->parent->parent; -> 왜 할부지로 cur 업데이트 안함?: 자식에서 더 해결해야하기 때문
          rotate_R(t, cur->parent->parent); 
        }
      }
    }
  }
  t->root->color = RBTREE_BLACK; // 2번 조건
}
/////////////////////////////* INSERT */////////////////////////////////////////
node_t *rbtree_insert(rbtree *t, const key_t key) 
{
  // TODO: implement insert
  // 1. BST 삽입과 같은 방식을 가지고 온다
  node_t *cur= t->root, *parent=t->nil;
  while (cur != t->nil)
  {
    parent = cur;
    // 키 값 위치 찾기
    if (cur->key > key) cur = cur->left; 
    else cur = cur->right;
  }
  cur->color = RBTREE_RED;
  cur->left = t->nil;
  cur->right = t->nil;
  // cur->key = key;
  // cur->parent = parent;
  if (cur->parent->color == RBTREE_RED && cur->color== RBTREE_RED) // 5번 조건 위배
  {
    insert_fixup(t, cur);
  }
  // 2. RB트리의 속성들을 비교하면서 재조정한다
  // 여기서 회전과 재조정하는 기능을 함수에 같이 구현해야한다?
  return t->root; // 반환값: root
}
/////////////////////////////* FIND */////////////////////////////////////////
node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  return t->root;
}
/////////////////////////////* MIN */////////////////////////////////////////
node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}
/////////////////////////////* MAX */////////////////////////////////////////
node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}
/////////////////////////////* ERASE */////////////////////////////////////////
int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}
/////////////////////////////* ARRAY */////////////////////////////////////////
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

