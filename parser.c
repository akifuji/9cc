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
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

Node *new_node_ident(char name) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
  node->name = name;
  return node;
}



typedef struct {
  int ty;      
  int val;    
  char *input;
} Token;

Token tokens[100];
int pos = 0;

int consume(int ty) {
  if (tokens[pos].ty != ty)
    return 0;
  pos++;
  return 1;
}

Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();

Node *program() {
  int i = 0;
  while (tokens[pos].ty != TK_EOF)
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node = expr();
  if (!consume(';'))
    error_at(tokens[pos].input, "';'ではないトークンです");
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume('='))
    node = new_node('=', node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume(TK_EQ))
      node = new_node(TK_EQ, node, relational());
    else if (consume(TK_NE))
      node = new_node(TK_NE, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume('<'))
      node = new_node('<', node, add());
    else if (consume(TK_LE))
      node = new_node(TK_LE, node, add());
    else if (consume('>'))
      node = new_node('<', add(), node);
    else if (consume(TK_GE))
      node = new_node(TK_LE, add(), node);
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

Node *unary() {
  if (consume('+'))
    return term();
  if (consume('-'))
    return new_node('-', new_node_num(0), term());
  return term();
}

Node *term() {
  if (tokens[pos].ty == TK_IDENT)
    return new_node_ident(&(tokens[pos].input));

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



void tokenize() {
  char *p = user_input;

  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    } 

    if ('a' <= *p && *p <= 'z') {
      tokens[i].ty = TK_IDENT;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if (*p == '=') {
      if (*(p + 1) == '=') {
        tokens[i].ty = TK_EQ;
        tokens[i].input = p++;
        i++;
        p++;
        continue;
      }
    }
 
    if (*p == '!') {
      if (*(p + 1) == '=') {
        tokens[i].ty = TK_NE;
        tokens[i].input = p++;
        i++;
        p++;
        continue;
      } else {
        error_at(p, "'='が続くべきです");
      }
    }    

    if (*p == ';' || *p == '=' || *p == '(' || *p == ')' || *p == '*' || *p == '/' || *p == '+' || *p == '-') {
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
