#include "rbtree.h" // 구조체,

#include <stdlib.h>

rbtree *new_rbtree(void) {
  // NIL 노드 공간 확보후 초기화 과정
  node_t *Nil= malloc(sizeof(node_t));
  Nil->color = RBTREE_BLACK;
  Nil->parent = Nil;
  Nil->left = Nil;
  Nil->right = Nil;

  //calloc(contiguous allocation)이란? 메모리를 동적으로 할당하면서 0으로 초기화 해주는 함수
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree)); // 여기서 1은 할당할 요소의 개수-> rbtree 하나 만들겠다
  p->nil = Nil; // nil 사이즈의 동적 메모리를 할당하며 할당한 주소의 값을 0으로 초기화??
  p->root = Nil; // 루트가 없을 땐 방금 만들어낸 NIL 가르키게
  return p;
}

/////////////////////////////////* DELETE *//////////////////////////////////////
void delete_rbtree(rbtree *t) 
{
  if (t == NULL || t->root == NULL) return;
  node_t *cur = t->root, *parents;
  // 1. 트리 안에 있는 모든 노드들을 반환후
  while (cur != t->nil) // 널 포인터와 논리적의미가 다름
  {
    if(cur->left != t->nil) // 자식이 있는 경우(왼쪽 자식부터)
    {
      cur = cur->left;
    }
    else if (cur->right != t->nil) // 오른쪽 자식이 있으면
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
///////////////////////////// Left - Rotate 의사 코드 작성//////////////////////////
void rotate_L(rbtree *T, node_t *x) // n = 부모
{

  if (x == NULL || x == T->nil || x->right == T->nil) return;

  node_t *y = x->right; // 자식 노드를 지정
  x->right = y->left;  // x의 오른쪽 자식을 y의 왼쪽 자식으로 설정

  // y의 왼쪽 자식 감별
  if(y->left != T->nil) y->left->parent = x;

  y->parent = x->parent;// y 부모를 x의 부모로 설정

  // x 부모 판별에 따른 y 위치 이동
  if(x->parent == T->nil) T->root = y;
  else if(x == x->parent->left) x->parent->left = y; // x 가 왼쪽이라면
  else x->parent->right = y;

  // y를 부모로 올리는 과정
  y->left = x;
  x->parent = y;
  return;
}
////////////////////////////// Right - Rotate 의사 코드 작성////////////////////////
void rotate_R(rbtree *T, node_t *x)
{

  if (x == NULL || x == T->nil || x->left == T->nil) return;

  node_t *y = x->left;
  x->left = y->right;  

  if(y->right != T->nil) y->right->parent = x;

  y->parent = x->parent;

  if(x->parent == T->nil) T->root = y;
  else if(x == x->parent->right) x->parent->right = y;
  else x->parent->left = y;

  y->right = x;
  x->parent = y;
  return;
}
/////////////////////////////////* INSERT-FIXUP */////////////////////////////////
void insert_fixup(rbtree *t, node_t *cur) //부모노드
{
  node_t *uncle = t->nil;
  while (cur->parent->color == RBTREE_RED) // 부모 색이 빨간색이 아니면 -> 4번 조건 통과
  {
    if(cur->parent == cur->parent->parent->left)// 부모가 왼쪽 자식인가?
    {
      uncle = cur->parent->parent->right; // 삼촌 정의
      // CASE 1: 삼촌 = 빨
      if (uncle->color == RBTREE_RED){ // 삼촌색 = RED
        cur->parent->color = RBTREE_BLACK; // 아부지-> black
        uncle->color = RBTREE_BLACK; // 삼촌색 -> black
        cur->parent->parent->color = RBTREE_RED; // 할아부지 -> red
        cur = cur->parent->parent; // cur = 할아부지로 업데이트
      }
      else
      { //삼촌 = 깜
        // CASE 2: 삼촌 = 깜 , 꺾
        if (cur == cur->parent->right) // 꺾 : 아부지가 할부지 왼쪽에 있는데 나는 아부지 오른쪽
        {
          cur = cur->parent;
          rotate_L(t, cur); // 아부지 기준 회전 하는데 Why 왼쪽?
        }
        // CASE 3: 삼촌 = 깜, 노꺾
        // 노꺾 : 아부지가 할부지 왼쪽 이면서 나도 아부지의 왼쪽
          cur->parent->color = RBTREE_BLACK;
          cur->parent->parent->color = RBTREE_RED;
          // cur = cur->parent->parent; -> 왜 할부지로 cur 업데이트 안함?: 자식에서 더 해결해야하기 때문
          rotate_R(t, cur->parent->parent); 
      }
    }
    else // 부모가 오른쪽 자식인가?
    {
      uncle = cur->parent->parent->left; // 삼촌 정의
      // CASE 1: 삼촌 = 빨
      if (uncle->color == RBTREE_RED){ // 삼촌색 = RED
        cur->parent->color = RBTREE_BLACK; // 아부지-> black
        uncle->color = RBTREE_BLACK; // 삼촌색 -> black
        cur->parent->parent->color = RBTREE_RED; // 할아부지 -> red
        cur = cur->parent->parent; // cur = 할아부지로 업데이트
      }
      else{//삼촌 = 깜
        // CASE 2: 삼촌 = 깜 , 꺾
        if (cur == cur->parent->left) // 꺾 : 아부지가 할부지 왼쪽에 있는데 나는 아부지 오른쪽
        {
          cur = cur->parent;
          rotate_R(t, cur); // 아부지 기준 회전 하는데 Why 왼쪽?
        }
        // CASE 3: 삼촌 = 깜, 노꺾
        // 노꺾 : 아부지가 할부지 왼쪽 이면서 나도 아부지의 왼쪽
        
        cur->parent->color = RBTREE_BLACK;
        cur->parent->parent->color = RBTREE_RED;
        // cur = cur->parent->parent; -> 왜 할부지로 cur 업데이트 안함?: 자식에서 더 해결해야하기 때문
        rotate_L(t, cur->parent->parent); 
      }
    }
  }
  t->root->color = RBTREE_BLACK; // 2번 조건
  return;
}
//////////////////////////////////* INSERT *//////////////////////////////////////
node_t *rbtree_insert(rbtree *t, const key_t key) 
{
  // 1. BST 삽입과 같은 방식을 가지고 온다
  node_t *node = calloc(1, sizeof(node_t));
  node_t *cur= t->root, *p=t->nil;

  while(cur != t->nil)
  {
    p = cur;
    // 키 값 위치 찾기
    if (cur->key > key) cur = cur->left; 
    else cur = cur->right;
  }
  node->parent = p;

  if(p == t->nil) t->root = node;
  else if(p->key > key) p->left= node;
  else p->right= node; 

  node->color = RBTREE_RED;
  node->left = t->nil;
  node->right = t->nil;
  node->key = key;

  insert_fixup(t, node);

  return t->root; // 반환값: root
}

////////////////////////////////////* FIND *//////////////////////////////////////
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *cur = t->root;

  while (cur->key != key)
  {
    if (cur == t->nil) return NULL;
    
    // 오른쪽 왼쪽 탐색
    if (cur->key > key) cur = cur->left;
    else cur = cur->right;
  }
  return cur; // 찾는 노드가 반환
}
////////////////////////////////////* MIN *///////////////////////////////////////
node_t *rbtree_min(const rbtree *t) {
  node_t *cur = t->root, *p = t->nil;
  while(cur != t->nil) 
  {
    p = cur;
    cur = cur->left;
  }
  return p; // 찾고자하는 최대값
}
////////////////////////////////////* MAX *///////////////////////////////////////
node_t *rbtree_max(const rbtree *t) {
  node_t *cur = t->root ,*p = t->nil;
  while(cur != t->nil) 
  {
    p = cur;
    cur = cur->right;
  }
  return p; // 찾고자하는 최대값
}
////////////////////////////////////* ERASE-TRANSPLANT *//////////////////////////
void rbtree_transplant(rbtree *t, node_t *u , node_t *v) // 역할:  u의 위치로 v (이식)
{
  if (u->parent == t->nil) t->root=v; 
  else if(u == u->parent->left) u->parent->left = v;
  else u->parent->right = v;
  v->parent = u->parent;
}
////////////////////////////////* ERASE-SUCCESSOR *///////////////////////////////
node_t *rbtree_successor(rbtree *t, node_t *node)
{
  node_t *cur = node;
  if(cur->right != t->nil)
  {
    cur = cur->right;

    while(cur->left != t->nil) cur = cur->left;
    
    return cur;
  }
  // 오른쪽 자식이 없으면 부모 방향으로 올라감
  node_t *parent = cur->parent;
  while (parent != t->nil && cur == parent->right) { //내가 부모의 오른쪽 자식인 동안 계속 올라간다
    cur = parent;
    parent = parent->parent;
  }
  return parent; // 없으면 t->nil (successor 없음)
}
////////////////////////////////////* ERASE- FIXEDUP *////////////////////////////
void erase_fixup(rbtree *t, node_t *p)
{
  node_t *brother = t->nil;
  while(p != t->root && p->color == RBTREE_BLACK)
  {
    if (p == p->parent->left) // 지금 수정하려는 곳이 루트 기준 윈쪽이야?
    {
      brother = p->parent->right;
      //CASE 1: 형제 빨간색일때
      if (brother->color == RBTREE_RED)
      {
        brother->color = RBTREE_BLACK;
        p->parent->color = RBTREE_RED;
        rotate_L(t, p->parent); // 조부모 기준 회전
        brother = p->parent->right;
      }
      // CASE 2: 형제의 두 자녀가 BLACK 일 떄
      if (brother->left->color == RBTREE_BLACK && brother->right->color == RBTREE_BLACK)
      {
        brother->color = RBTREE_BLACK;
        p = p->parent;
      }
      else // 형제 자녀가 둘다 빨갛거나 한쪽만 빨간색일 때
      {
        // CASE 3: 형제의 가까운 자식이 블랙
        if (brother->right->color == RBTREE_BLACK)
        {
          brother->left->color = RBTREE_BLACK;
          brother->color = RBTREE_RED;
          rotate_R(t, brother); // 형제 기준 회전
          brother = p->parent->left;
        }
        brother->color = p->parent->color;
        p->parent->color = RBTREE_BLACK;
        brother->right->color = RBTREE_BLACK;
        rotate_L(t, p->parent); // 조부모 기준 회전
        p = t->root;
      }
    }
    else // 노드가 루트 기준으로 반대에 위치해있을 때
    {
      brother = p->parent->left;
      //CASE 1: 형제 빨간색일때
      if (brother->color == RBTREE_RED)
      {
        brother->color = RBTREE_BLACK;
        p->parent->color = RBTREE_RED;
        rotate_R(t, p->parent); // 조부모 기준 회전
        brother = p->parent->left;
      }
      // CASE 2: 형제의 두 자녀가 BLACK 일 떄
      if (brother->right->color == RBTREE_BLACK && brother->left->color == RBTREE_BLACK)
      {
        brother->color = RBTREE_BLACK;
        p = p->parent;
      }
      else // 형제 자녀가 둘다 빨갛거나 한쪽만 빨간색일 때
      {
        // CASE 3: 형제의 가까운 자식이 블랙
        if (brother->left->color == RBTREE_BLACK)
        {
          brother->right->color = RBTREE_BLACK;
          brother->color = RBTREE_RED;
          rotate_L(t, brother); // 형제 기준 회전
          brother = p->parent->right;
        }
        brother->color = p->parent->color;
        p->parent->color = RBTREE_BLACK;
        brother->left->color = RBTREE_BLACK;
        rotate_R(t, p->parent); // 조부모 기준 회전
        p = t->root;
      }
    }
  }
}

////////////////////////////////////* ERASE */////////////////////////////////////
int rbtree_erase(rbtree *t, node_t *p) 
{
  node_t *remove_node = p, *remove_node_child= t->nil; // 삭제되는노드(자식수의 따라 본인 or 후계자), 삭제되는 노드의 자식
  color_t remove_color;
  remove_color = remove_node->color;

  // 삭제하려는 노드의 자식 수 확인
  if (p->left == t->nil) 
  {
    remove_node_child = p->right; // 왼쪽 자식 없으니 오른쪽 자식
    rbtree_transplant(t, p, p->right); // 자식을 삭제하려는 노드와 바꿔주고
  }
  else if(p->right == t->nil)
  {
    remove_node_child = p->left; // 오른쪽 자식 없으니 왼쪽 자식
    rbtree_transplant(t, p, p->left); // 자식을 삭제하려는 노드와 바꿔주고
  }
  else //자식이 둘다 있는거라면
  {
    remove_node = rbtree_successor(t, p); // 후계자가 삭제되는 노드가 된다
    remove_color = remove_node->color; //삭제하는 색 또한 교체해주고
    remove_node_child = remove_node->right; // 삭제하려는 노드의 자식또한 오른쪽 자식이 된다.

    if (remove_node != p->right) // 만약 삭제하려는 노드가 오른쪽 자식이 아니라면 
    {
      rbtree_transplant(t, remove_node, remove_node_child); // 삭제하려는 노드를 오른쪽 자식으로 바꾼다.
      remove_node->right = p->right; // 기존에 삭제 하렸던 노드의 오른쪽을 물려받음
      remove_node->right->parent = remove_node; //그 물려받은 노드의 부모도 설정
    }
    else // 후계자의 오른쪽 자식이 없을경우
    {
      remove_node_child->parent = remove_node;
    }
    rbtree_transplant(t, p, remove_node); // 기존의 삭제하려했는 노드를 후계자로 바꾼다?
    remove_node->left = p->left;
    remove_node->left->parent = remove_node;
    remove_node->color = p->color;
  if (remove_color == RBTREE_BLACK) // case: 삭제된 색이 검은색일때
  {
    erase_fixup(t, remove_node_child);
  }
  }
  free(p);

  return 0;
}
/////////////////////////////* ARRAY *///////////////////////////////////////////
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) 
{ 
  // 중위 순회 하면서 - 재귀, 스택형
  node_t *cur = t->root;
  int idx = 0;
  array_recursion(t, arr, cur, &idx);
  return 0;
}

void array_recursion(rbtree *t, key_t *arr, node_t *r, int *idx)
{
  if (r == t->nil) return 0;

  array_recursion(t,arr, r->left, idx);
  arr[(*idx)++] = r->key;
  array_recursion(t,arr, r->right, idx);
  return 0;
}