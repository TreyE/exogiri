#include <string.h>

#include "exogiri.h"

void free_node(ErlNifEnv* __attribute__((unused))env, void* obj) {
  Node* node = (Node*)obj;
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
  
  size_t nameLen;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }
  nameLen = xmlStrlen(node->node->name);
  enif_alloc_binary(nameLen,&nb);
  memcpy(nb.data, node->node->name, nameLen);
  result = enif_make_binary(env,&nb);
  enif_release_binary(&nb);
  return result;
}

ERL_NIF_TERM priv_node_namespace(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;
  ErlNifBinary ns_prefix_nb;
  ErlNifBinary ns_href_nb;
  ERL_NIF_TERM ns_prefix_term;
  ERL_NIF_TERM ns_href_term;
  xmlNsPtr ns;
  int ns_prefix_size;
  int ns_href_size;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }
  ns = node->node->ns;
  if (ns) {
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
  return atom_none;
}

ERL_NIF_TERM create_node_term(ErlNifEnv* env, Document* document, xmlNodePtr np) {
  ERL_NIF_TERM result;

  Node* nodeRes = (Node *)enif_alloc_resource(EXN_RES_TYPE, sizeof(Node));
  nodeRes->node = np;
  nodeRes->doc = document;
  enif_keep_resource(document);
  result = enif_make_resource(env, nodeRes);
  enif_release_resource(nodeRes);
  return result;
}

ERL_NIF_TERM priv_node_unlink(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Node *node;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXN_RES_TYPE,(void **)&node)) {
    return enif_make_badarg(env);
  }

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

  if (c_node->doc) {
    enif_release_resource(c_node->doc);
    xmlUnlinkNode(c_node->node);
    c_node->doc = NULL;
  }
  xmlAddChild(p_node->node, c_node->node);
  c_node->doc = p_node->doc;
  enif_keep_resource(p_node->doc);

  return atom_ok;
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