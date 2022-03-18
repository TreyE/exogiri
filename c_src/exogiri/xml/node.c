#include <string.h>

#include "exogiri.h"

void free_node(ErlNifEnv* __attribute__((unused))env, void* obj) {
  Node* node = (Node*)obj;
  xmlFreeDoc(node->doc);
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