#include "libami.h"

#ifndef AMIGAOS

void NewList(struct List *list)
{
  list->lh_TailPred = (struct Node *)&list->lh_Head;
  list->lh_Tail = NULL;
  list->lh_Head = (struct Node *)&list->lh_Tail;
}

void AddHead(struct List *list, struct Node *node)
{
  struct Node *oldhead = list->lh_Head;
  list->lh_Head = node;
  node->ln_Succ = oldhead;
  node->ln_Pred = (struct Node *)&list->lh_Head;
  oldhead->ln_Pred = node;
}

void AddTail(struct List *list, struct Node *node)
{
  struct Node *oldtail = list->lh_TailPred;
  list->lh_TailPred = node;
  node->ln_Succ = (struct Node *)&list->lh_Tail;
  node->ln_Pred = oldtail;
  oldtail->ln_Succ = node;
}

struct Node *RemHead(struct List *list)
{
  struct Node *node = list->lh_Head;
  if(node->ln_Succ) {
    list->lh_Head = node->ln_Succ;
    node->ln_Succ->ln_Pred = (struct Node *)&list->lh_Head;
    return node;
  } else return NULL;
}

struct Node *RemTail(struct List *list)
{
  struct Node *node = list->lh_TailPred;
  if(node->ln_Pred) {
    list->lh_TailPred = node->ln_Pred;
    node->ln_Pred->ln_Succ = (struct Node *)&list->lh_Tail;
    return node;
  } else return NULL;
}

void Insert(struct List *list, struct Node *node, struct Node *after)
{
  if(!after) after = (struct Node *)&list->lh_Head;
  node->ln_Succ = after->ln_Succ;
  node->ln_Pred = after;
  after->ln_Succ->ln_Pred = node;  
  after->ln_Succ = node;
}

void Remove(struct Node *node)
{
  node->ln_Pred->ln_Succ = node->ln_Succ;
  node->ln_Succ->ln_Pred = node->ln_Pred;
}

void Enqueue(struct List *list, struct Node *node)
{
  struct Node *before = list->lh_Head;
  while(before->ln_Succ && before->ln_Pri >= node->ln_Pri)
    before = before->ln_Succ;
  node->ln_Succ = before;
  node->ln_Pred = before->ln_Pred;
  before->ln_Pred->ln_Succ = node;  
  before->ln_Pred = node;
}

struct Node *FindName(struct List *list, STRPTR name)
{
  struct Node *node;
  for(node = list->lh_Head; node->ln_Succ; node = node->ln_Succ)
    if(!strcmp((char *)name, node->ln_Name))
      return node;
  return NULL;
}

#endif
