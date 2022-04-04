#pragma once

#include <libxml/tree.h>

typedef struct UnlinkedNode {
  xmlNodePtr node;
  struct UnlinkedNode *next;
} UnlinkedNode;

void free_unlinked_nodes(UnlinkedNode *unlinkedNodes);
UnlinkedNode *add_unlinked_node(UnlinkedNode *unlinked_nodes, xmlNodePtr node);
UnlinkedNode *remove_unlinked_node(UnlinkedNode *unlinked_nodes, xmlNodePtr node);