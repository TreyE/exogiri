#pragma once

#include <libxml/tree.h>

typedef struct UnlinkedNode {
  xmlNodePtr node;
  struct UnlinkedNode *next;
} UnlinkedNode;

typedef struct UnlinkedNs {
  xmlNsPtr ns;
  struct UnlinkedNs *next;
} UnlinkedNs;

void free_unlinked_nodes(UnlinkedNode *unlinkedNodes);
UnlinkedNode *add_unlinked_node(UnlinkedNode *unlinked_nodes, xmlNodePtr node);
UnlinkedNode *remove_unlinked_node(UnlinkedNode *unlinked_nodes, xmlNodePtr node);

UnlinkedNs *pin_unlinked_ns(UnlinkedNs *unlinked_nses, xmlNsPtr ns);
void free_unlinked_nses(UnlinkedNs *unlinked_nses);