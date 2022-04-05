#include <erl_nif.h>
#include "unlinked_node.h"

void free_unlinked_nodes(UnlinkedNode *unlinked_nodes) {
  UnlinkedNode *next;
  UnlinkedNode *curr;
  if (!unlinked_nodes) {
    return;
  }
  curr = unlinked_nodes;
  while (NULL != curr) {
    next = curr->next;
    xmlFreeNode(curr->node);
    enif_free(curr);
    curr = next;
  }
}

UnlinkedNode *allocate_new_node() {
  UnlinkedNode *result;

  result = (UnlinkedNode *)enif_alloc(sizeof(UnlinkedNode));
  result->next = NULL;
  result->node = NULL;

  return result;
}

UnlinkedNode *remove_unlinked_node(UnlinkedNode *unlinked_nodes, xmlNodePtr node) {
  UnlinkedNode *curr;
  UnlinkedNode *prev;
  UnlinkedNode *next;
  curr = unlinked_nodes;
  if (!curr) {
    return unlinked_nodes;
  }
  if (curr->node == node) {
    next = curr->next;
    enif_free(curr);
    return next;
  }
  prev = NULL;
  while (NULL != curr) {
    next = curr->next;
    if (curr->node == node) {
      prev->next = next;
      enif_free(curr);
      return unlinked_nodes;
    }
    prev = curr;
    curr = next;
  }
  return unlinked_nodes;
}

UnlinkedNode* add_unlinked_node(UnlinkedNode *unlinked_nodes, xmlNodePtr node) {
  UnlinkedNode *curr;
  UnlinkedNode *prev;
  UnlinkedNode *next;
  if (!unlinked_nodes) {
    curr = allocate_new_node();
    curr->node = node;
    return curr;
  }
  prev = NULL;
  curr = unlinked_nodes;
  while (NULL != curr) {
    next = curr->next;
    if (curr->node == node) {
      return unlinked_nodes;
    }
    prev = curr;
    curr = next;
  }
  curr = allocate_new_node();
  curr->node = node;
  prev->next = curr;
  return unlinked_nodes;
}

UnlinkedNs *allocate_new_unlinked_ns() {
  UnlinkedNs *result;

  result = (UnlinkedNs *)enif_alloc(sizeof(UnlinkedNs));
  result->next = NULL;
  result->ns = NULL;

  return result;
}

UnlinkedNs* pin_unlinked_ns(UnlinkedNs *unlinked_nses, xmlNsPtr ns) {
  UnlinkedNs *curr;
  UnlinkedNs *prev;
  UnlinkedNs *next;
  if (!unlinked_nses) {
    curr = allocate_new_unlinked_ns();
    curr->ns = ns;
    return curr;
  }
  prev = NULL;
  curr = unlinked_nses;
  while (NULL != curr) {
    next = curr->next;
    if (curr->ns == ns) {
      return unlinked_nses;
    }
    prev = curr;
    curr = next;
  }
  curr = allocate_new_unlinked_ns();
  curr->ns = ns;
  prev->next = curr;
  return unlinked_nses;
}

void free_unlinked_nses(UnlinkedNs *unlinked_nses) {
  UnlinkedNs *next;
  UnlinkedNs *curr;
  if (!unlinked_nses) {
    return;
  }
  curr = unlinked_nses;
  while (NULL != curr) {
    next = curr->next;
    xmlFreeNs(curr->ns);
    enif_free(curr);
    curr = next;
  }
}