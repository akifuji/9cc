#ifndef __9CC_H_
#define __9CC_H_

enum {
  TK_NUM = 256,
  TK_EOF,
  TK_RETURN,
  TK_EQ,
  TK_NE,
  TK_LE,
  TK_GE,
  TK_IDENT,
};

enum {
  ND_NUM = 256,
  ND_IDENT,
  ND_RETURN,
};

typedef struct Node {
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val;
  char name;
} Node;

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

typedef struct {
  Vector *keys;
  Vector *vals;
} Map;

char *user_input;
Node *code[100];
void tokenize();
Node *program();
void gen(Node *node);
void error(char *fmt, ...);
void error_at(char *loc, char *msg);
Vector *new_vector();
void vec_push(Vector *vec, void *elem);
void runtest();
Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);

#endif
