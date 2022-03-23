#include <string.h>

#include "exogiri.h"

void free_node(ErlNifEnv* __attribute__((unused))env, void* obj) {
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
  enif_keep_resource(document);
  result = enif_make_resource(env, nodeRes);
  enif_release_resource(nodeRes);
  return result;
}

ERL_NIF_TERM priv_node_unlink(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;

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

  return atom_ok;
}

ERL_NIF_TERM priv_node_add_child(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *p_node;
  Node *c_node;
  ErlNifPid self;

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
  xmlReconciliateNs(p_node->doc->doc, c_node->node);
  c_node->doc = p_node->doc;
  enif_keep_resource(p_node->doc);

  return atom_ok;
}

ERL_NIF_TERM priv_node_content(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ERL_NIF_TERM result;
  xmlChar *content;
  ErlNifPid self;

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
    return atom_error;
  }

  enif_free(attr_val);
  enif_free(attr_name);

  return atom_ok;
}

ERL_NIF_TERM priv_node_set_content(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifPid self;
  ErlNifBinary content_b;
  xmlChar *content_val;

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


/*
// Clone a node and document and reserve reference to document
Node* clone_node(Node* inNode) {
  xmlDocPtr doc_copy;
  xmlNodePtr np_copy;
  Node* node_copy;
  xmlXPathContextPtr ctx;
  xmlChar *path;
  xmlXPathObjectPtr xpath;

  path = xmlGetNodePath(inNode->node);
  node_copy = enif_alloc(sizeof(Node));
  doc_copy = xmlCopyDoc(inNode->doc->doc,1);

  ctx = xmlXPathNewContext(doc_copy);

  xpath = xmlXPathEvalExpression(path, ctx);
  np_copy = *(xpath->nodesetval->nodeTab);

  xmlXPathFreeNodeSet(xpath->nodesetval);
  xmlXPathFreeNodeSetList(xpath);
  xmlXPathFreeContext(ctx);

  node_copy->doc = doc_copy;
  node_copy->node = np_copy;

  return node_copy;
}*/