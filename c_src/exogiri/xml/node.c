#include <string.h>

#include "exogiri.h"
#include "namespace.h"

void free_node(__attribute__((unused))ErlNifEnv* env, void* obj) {
  Node* node = (Node*)obj;
  enif_free(node->owner);
  if (node->doc) {
    enif_release_resource(node->doc);
  }
  if (node->doc == NULL) {
    xmlFreeNode(node->node);
  }
}

ERL_NIF_TERM priv_node_local_name(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifBinary nb;
  ERL_NIF_TERM result;
  ErlNifPid self;
  
  size_t nameLen;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, node)

  nameLen = xmlStrlen(node->node->name);
  enif_alloc_binary(nameLen,&nb);
  memcpy(nb.data, node->node->name, nameLen);
  result = enif_make_binary(env,&nb);
  enif_release_binary(&nb);
  return result;
}

ERL_NIF_TERM namespace_as_tuple(ErlNifEnv* env, xmlNsPtr ns) {
  ErlNifBinary ns_prefix_nb;
  ErlNifBinary ns_href_nb;
  ERL_NIF_TERM ns_prefix_term;
  ERL_NIF_TERM ns_href_term;
  int ns_prefix_size;
  int ns_href_size;

  if (ns->prefix) {
    ns_prefix_size = xmlStrlen(ns->prefix);
    enif_alloc_binary(ns_prefix_size, &ns_prefix_nb);
    memcpy(ns_prefix_nb.data, ns->prefix, ns_prefix_size);
  } else {
    ns_prefix_size = 0;
    enif_alloc_binary(ns_prefix_size, &ns_prefix_nb);
  }
  ns_prefix_term = enif_make_binary(env, &ns_prefix_nb);
  enif_release_binary(&ns_prefix_nb);
  ns_href_size = xmlStrlen(ns->href);
  enif_alloc_binary(ns_href_size, &ns_href_nb);
  memcpy(ns_href_nb.data, ns->href, ns_href_size);
  ns_href_term = enif_make_binary(env, &ns_href_nb);
  enif_release_binary(&ns_href_nb);
  return enif_make_tuple2(
    env,
    ns_prefix_term,
    ns_href_term
  );
}

ERL_NIF_TERM priv_node_namespace(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  xmlNsPtr ns;
  ERL_NIF_TERM atom_none;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }
  ns = node->node->ns;
  if (ns) {
    return namespace_as_tuple(env, ns);
  }
  ASSIGN_NONE(env, atom_none);
  return atom_none;
}

ERL_NIF_TERM priv_node_namespaces(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;
  xmlNsPtr *namespaces;
  ERL_NIF_TERM ns_terms;
  int j;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, node)

  namespaces = xmlGetNsList(node->doc->doc, node->node);
  if (!namespaces) {
    return enif_make_list(env, 0);
  }

  ns_terms = enif_make_list(env, 0);
  for (j = 0 ; namespaces[j] != NULL ; ++j) {
    ns_terms = enif_make_list_cell(
      env,
      namespace_as_tuple(env, namespaces[j]),
      ns_terms
    );
  }

  xmlFree(namespaces);
  return ns_terms;
}

ERL_NIF_TERM create_node_term(ErlNifEnv* env, Document* document, xmlNodePtr np) {
  ERL_NIF_TERM result;
  ErlNifPid *self;

  self = (ErlNifPid *)enif_alloc(sizeof(ErlNifPid));
  enif_self(env, self);
  Node* nodeRes = (Node *)enif_alloc_resource(EXN_RES_TYPE, sizeof(Node));
  nodeRes->owner = self;
  nodeRes->node = np;
  nodeRes->doc = document;
  if (document) {
    enif_keep_resource(document);
  }
  result = enif_make_resource(env, nodeRes);
  enif_release_resource(nodeRes);
  return result;
}

ERL_NIF_TERM priv_node_unlink(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;
  ERL_NIF_TERM atom_ok;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }
  CHECK_STRUCT_OWNER(env, self, node)

  if (node->doc) {
    enif_release_resource(node->doc);
    xmlUnlinkNode(node->node);
    node->doc = NULL;
  }

  ASSIGN_OK(env, atom_ok);
  return atom_ok;
}

ERL_NIF_TERM priv_node_add_child(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *p_node;
  Node *c_node;
  ErlNifPid self;
  ERL_NIF_TERM atom_ok;

  if(argc != 2)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&p_node)) {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[1],EXN_RES_TYPE,(void **)&c_node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, p_node)
  CHECK_STRUCT_OWNER(env, self, c_node)

  if (c_node->doc) {
    enif_release_resource(c_node->doc);
    xmlUnlinkNode(c_node->node);
    c_node->doc = NULL;
  }
  xmlAddChild(p_node->node, c_node->node);
  recon_ns_after_move(p_node->doc->doc, c_node->node, 0);
  c_node->doc = p_node->doc;
  enif_keep_resource(p_node->doc);

  ASSIGN_OK(env, atom_ok);
  return atom_ok;
}

ERL_NIF_TERM priv_node_content(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ERL_NIF_TERM result;
  xmlChar *content;
  ErlNifPid self;
  ERL_NIF_TERM atom_none;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, node)

  content = xmlNodeGetContent(node->node);
  if (!content) {
    ASSIGN_NONE(env, atom_none);
    return atom_none;
  }
  result = xml_char_to_binary_term(env, content);
  xmlFree(content);
  return result;
}

ERL_NIF_TERM priv_node_attribute_value(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;
  ErlNifBinary attr_name_b;
  ERL_NIF_TERM result;
  xmlChar *attr_name;
  xmlChar *attr_val;
  ERL_NIF_TERM atom_none;

  if(argc != 2)
  {
    return enif_make_badarg(env);
  }
  if (!enif_inspect_binary(env, argv[1], &attr_name_b)) {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, node)

  attr_name = nif_binary_to_xmlChar(&attr_name_b);

  attr_val = xmlGetProp(node->node, attr_name);
  if (!attr_val) {
    enif_free(attr_name);
    ASSIGN_NONE(env, atom_none);
    return atom_none;
  }
  result = xml_char_to_binary_term(env, attr_val);
  xmlFree(attr_val);
  enif_free(attr_name);

  return result;
}

ERL_NIF_TERM priv_node_set_attribute_value(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;
  ErlNifBinary attr_name_b;
  ErlNifBinary attr_value_b;
  xmlChar *attr_name;
  xmlChar *attr_val;
  xmlAttrPtr attr;
  ERL_NIF_TERM atom_result;

  if(argc != 3)
  {
    return enif_make_badarg(env);
  }
  if (!enif_inspect_binary(env, argv[2], &attr_value_b)) {
    return enif_make_badarg(env);
  }
  if (!enif_inspect_binary(env, argv[1], &attr_name_b)) {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, node)

  attr_name = nif_binary_to_xmlChar(&attr_name_b);
  attr_val = nif_binary_to_xmlChar(&attr_value_b);

  attr = xmlSetProp(node->node, attr_name, attr_val);
  if (!attr) {
    enif_free(attr_name);
    enif_free(attr_val);
    ASSIGN_ERROR(env, atom_result);
    return atom_result;
  }

  enif_free(attr_val);
  enif_free(attr_name);

  ASSIGN_OK(env, atom_result);

  return atom_result;
}

// TODO: Sanitize content for entities
ERL_NIF_TERM priv_node_set_content(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;
  ErlNifBinary content_b;
  xmlChar *content_val;
  ERL_NIF_TERM atom_ok;

  if(argc != 2)
  {
    return enif_make_badarg(env);
  }
  if (!enif_inspect_binary(env, argv[1], &content_b)) {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, node)

  content_val = nif_binary_to_xmlChar(&content_b);
  xmlNodeSetContent(node->node, content_val);

  enif_free(content_val);

  ASSIGN_OK(env, atom_ok);
  return atom_ok;
}

ERL_NIF_TERM node_list_to_term(ErlNifEnv* env, Document* document, xmlNodeSetPtr nodes) {
  ERL_NIF_TERM result_list_reversed;
  xmlNodePtr* nsp;
  ERL_NIF_TERM result_node;
  ERL_NIF_TERM result_list;
  int listLength;
  int i;
  listLength = nodes->nodeNr;
  result_list_reversed = enif_make_list(env, 0);
  nsp = nodes->nodeTab;

  for (i = 0; i<listLength; i++) {
    result_node = create_node_term(env, document, *nsp);
    result_list_reversed = enif_make_list_cell(
      env,
      result_node,
      result_list_reversed
    );
    nsp++;
  }

  enif_make_reverse_list(env, result_list_reversed, &result_list);

  return result_list;
}

ERL_NIF_TERM priv_node_children(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;
  xmlNodePtr child;
  xmlNodeSetPtr set;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, node)

  child = node->node->children;
  if (!child) {
    return enif_make_list(env, 0);
  }

  set = xmlXPathNodeSetCreate(child);
  child = child->next;
  while (NULL != child) {
    xmlXPathNodeSetAddUnique(set, child);
    child = child->next;
  }

  return node_list_to_term(env, node->doc, set);
}

ERL_NIF_TERM priv_node_parent(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;
  ERL_NIF_TERM atom_none;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, node)

  if (!(node->node->parent)) {
    ASSIGN_NONE(env, atom_none);
    return atom_none;
  }

  return create_node_term(env, node->doc, node->node->parent);
}

ERL_NIF_TERM priv_node_previous_sibling(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;
  xmlNodePtr sibling;
  ERL_NIF_TERM atom_none;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, node)

  sibling = xmlPreviousElementSibling(node->node);

  if (!sibling) {
    ASSIGN_NONE(env, atom_none);
    return atom_none;
  }

  return create_node_term(env, node->doc, sibling);
}

ERL_NIF_TERM priv_node_next_sibling(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;
  xmlNodePtr sibling;
  ERL_NIF_TERM atom_none;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, node)

  sibling = xmlNextElementSibling(node->node);

  if (!sibling) {
    ASSIGN_NONE(env, atom_none);
    return atom_none;
  }

  return create_node_term(env, node->doc, sibling);
}

ERL_NIF_TERM priv_node_last_element_child(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;
  xmlNodePtr child;
  ERL_NIF_TERM atom_none;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, node)

  child = xmlLastElementChild(node->node);

  if (!child) {
    ASSIGN_NONE(env, atom_none);
    return atom_none;
  }

  return create_node_term(env, node->doc, child);
}

ERL_NIF_TERM priv_node_first_element_child(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;
  xmlNodePtr child;
  ERL_NIF_TERM atom_none;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, node)

  child = xmlFirstElementChild(node->node);

  if (!child) {
    ASSIGN_NONE(env, atom_none);
    return atom_none;
  }

  return create_node_term(env, node->doc, child);
}

ERL_NIF_TERM priv_node_add_next_sibling(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *p_node;
  Node *s_node;
  xmlNodePtr node_copy;
  xmlNodePtr new_node;
  ErlNifPid self;
  ERL_NIF_TERM atom_result;

  if(argc != 2)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&p_node)) {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[1],EXN_RES_TYPE,(void **)&s_node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, p_node)
  CHECK_STRUCT_OWNER(env, self, s_node)

  if (s_node->doc) {
    enif_release_resource(s_node->doc);
    xmlUnlinkNode(s_node->node);
    s_node->doc = NULL;
  }
  node_copy = xmlCopyNode(s_node->node, 2);
  new_node = xmlAddNextSibling(p_node->node, node_copy);
  if (!new_node) {
    xmlFreeNode(node_copy);
    ASSIGN_ERROR(env, atom_result);
    return atom_result;
  }
  recon_ns_after_move(p_node->doc->doc, new_node, 0);
  xmlFreeNode(s_node->node);
  s_node->node = new_node;
  s_node->doc = p_node->doc;
  enif_keep_resource(p_node->doc);

  ASSIGN_OK(env, atom_result);
  return atom_result;
}

ERL_NIF_TERM priv_node_add_previous_sibling(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *p_node;
  Node *s_node;
  xmlNodePtr node_copy;
  xmlNodePtr new_node;
  ErlNifPid self;
  ERL_NIF_TERM atom_result;

  if(argc != 2)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&p_node)) {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[1],EXN_RES_TYPE,(void **)&s_node)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, p_node)
  CHECK_STRUCT_OWNER(env, self, s_node)

  if (s_node->doc) {
    enif_release_resource(s_node->doc);
    xmlUnlinkNode(s_node->node);
    s_node->doc = NULL;
  }
  node_copy = xmlCopyNode(s_node->node, 2);
  new_node = xmlAddPrevSibling(p_node->node, node_copy);
  if (!new_node) {
    xmlFreeNode(node_copy);
    ASSIGN_ERROR(env, atom_result);
    return atom_result;
  }
  recon_ns_after_move(p_node->doc->doc, new_node, 0);
  xmlFreeNode(s_node->node);
  s_node->node = new_node;
  s_node->doc = p_node->doc;
  enif_keep_resource(p_node->doc);

    ASSIGN_OK(env, atom_result);
    return atom_result;
}

ERL_NIF_TERM priv_node_create_no_ns(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Document *document;
  xmlNodePtr node;
  ErlNifPid self;
  ERL_NIF_TERM node_term;
  ErlNifBinary nb;
  xmlChar *nodeName;

  if(argc != 2)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXD_RES_TYPE,(void **)&document)) {
    return enif_make_badarg(env);
  }
  if (!enif_inspect_binary(env, argv[1], &nb)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, document)
  
  nodeName = nif_binary_to_xmlChar(&nb);
  node = xmlNewDocNode(document->doc, NULL, nodeName, NULL);
  node_term = create_node_term(env, document, node);
  enif_free(nodeName);
  return node_term;
}

ERL_NIF_TERM priv_node_create_with_ns(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Document *document;
  xmlNodePtr node;
  ErlNifPid self;
  ERL_NIF_TERM node_term;
  ErlNifBinary nb;
  ErlNifBinary nb_ns_abbrev;
  ErlNifBinary nb_ns_href;
  xmlChar *nodeName;
  xmlChar *nsAbbrev;
  xmlChar *nsHref;
  xmlNsPtr ns;

  if(argc != 4)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXD_RES_TYPE,(void **)&document)) {
    return enif_make_badarg(env);
  }
  if (!enif_inspect_binary(env, argv[1], &nb)) {
    return enif_make_badarg(env);
  }
  if (!enif_inspect_binary(env, argv[2], &nb_ns_abbrev)) {
    return enif_make_badarg(env);
  }
  if (!enif_inspect_binary(env, argv[3], &nb_ns_href)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, document)
  
  nodeName = nif_binary_to_xmlChar(&nb);
  nsHref = nif_binary_to_xmlChar(&nb_ns_href);
  nsAbbrev = nif_binary_to_xmlChar(&nb_ns_abbrev);

  if (xmlStrlen(nsAbbrev) < 1) {
    enif_free(nsAbbrev);
    nsAbbrev = NULL;
  }

  node = xmlNewDocNode(document->doc, NULL, nodeName, NULL);
  ns = xmlNewNs(node, nsHref, nsAbbrev);
  xmlSetNs(node, ns);
  node_term = create_node_term(env, document, node);
  enif_free(nodeName);
  enif_free(nsHref);
  if (nsAbbrev) {
    enif_free(nsAbbrev);
  }
  return node_term;
}