#ifndef __9CC_H_
#define __9CC_H_

enum {
  TK_NUM = 256,
  TK_EOF,
  TK_EQ,
  TK_NE,
  TK_LE,
  TK_GE,
};

typedef struct Node {
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val;
} Node;

char *user_input;
void tokenize();
Node *expr();
void gen(Node *node);
void error(char *fmt, ...);
void error_at(char *loc, char *msg);

#endif
