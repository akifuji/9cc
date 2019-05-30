#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

Node *new_node(int ty, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = TK_NUM;
  node->val = val;
  return node;
}

typedef struct {
  int ty;      
  int val;    
  char *input;
} Token;

Node *expr();
Node *add();
Node *mul();
Node *unary();
Node *term();

Token tokens[100];
int pos = 0;

int consume(int ty) {
  if (tokens[pos].ty != ty)
    return 0;
  pos++;
  return 1;
}

Node *term() {
  if (consume('(')) {
    Node *node = expr();
    if (!consume(')'))
      error_at(tokens[pos].input,
        "開き括弧に対応する閉じ括弧がありません");
    return node;
  }

  if (tokens[pos].ty == TK_NUM)
    return new_node_num(tokens[pos++].val);

  error_at(tokens[pos].input,
      "数値でも開き括弧でもないトークンです");
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume('<'))
      node = new_node('<', node, add());
    else if (consume(TK_LE))
      node = new_node(TK_LE, node, add());
    else if (consume('>'))
      node = new_node('>', node, add());
    else if (consume(TK_GE))
      node = new_node(TK_GE, node, add());
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume('+'))
      node = new_node('+', node, mul());
    else if (consume('-'))
      node = new_node('-', node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume('*'))
      node = new_node('*', node, unary());
    else if (consume('/'))
      node = new_node('/', node, unary());
    else
      return node;
  }
}

Node *expr() {
  Node *node = relational();
  return  node;

  for (;;) {
    if (consume('+'))
      node = new_node('+', node, mul());
    else if (consume('-'))
      node = new_node('-', node, mul());
    else
      return node;
  }
}

Node *unary() {
  if (consume('+'))
    return term();
  if (consume('-'))
    return new_node('-', new_node_num(0), term());
  return term();
}


void tokenize() {
  char *p = user_input;

  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    } 

    if (*p == '(' || *p == ')' || *p == '*' || *p == '/' || *p == '+' || *p == '-') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }
    
    if (*p == '<') {
      if (*(p + 1) == '=') {
        tokens[i].ty = TK_LE;
        tokens[i].input = p++;
      }
      else {
        tokens[i].ty = *p;
        tokens[i].input = p;
      }
      i++;
      p++;
      continue;
    }
   
    if (*p == '>') {
      if (*(p + 1) == '=') {
        tokens[i].ty = TK_GE;
        tokens[i].input = p++;
      }
      else {
        tokens[i].ty = *p;
        tokens[i].input = p;
      }
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}
