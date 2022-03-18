#include <string.h>

#include "exogiri.h"

xmlChar *nif_binary_to_xmlChar(ErlNifBinary * bin) {
  xmlChar* result;
  int bin_size;
  bin_size = bin->size + 1;
  result = (xmlChar*)enif_alloc(bin_size);
  memset(result, 0, bin_size);
  memcpy(result, bin->data, bin->size);
  return result;
}

void register_xpath_query_ns(xmlXPathContextPtr ctx, unsigned char* prefix, unsigned char* uri) {
  xmlXPathRegisterNs(ctx,
                     prefix,
                     uri
                    );
}

void register_xml_nses_from_map(xmlXPathContextPtr ctx, ErlNifEnv* env, const ERL_NIF_TERM name_map) {
  ERL_NIF_TERM key, value;
  ErlNifBinary key_b,value_b;
  ErlNifMapIterator iter;
  xmlChar *key_xc, *value_xc;
  enif_map_iterator_create(env, name_map, &iter, ERL_NIF_MAP_ITERATOR_FIRST);
  while (enif_map_iterator_get_pair(env, &iter, &key, &value)) {
      if (enif_inspect_binary(env, key, &key_b) && enif_inspect_binary(env, value, &value_b)) {
        key_xc = nif_binary_to_xmlChar(&key_b);
        value_xc = nif_binary_to_xmlChar(&value_b);
        register_xpath_query_ns(ctx, key_xc, value_xc);
        enif_free((void *)key_xc);
        enif_free((void *)value_xc);
      }
      enif_map_iterator_next(env, &iter);
  }
  enif_map_iterator_destroy(env, &iter);
}

void
xpath_generic_exception_handler(void *ctx, const char *msg, ...)
{
  char* message;

  va_list args;
  va_start(args, msg);
  vasprintf(&message, msg, args);
  va_end(args);
}

ERL_NIF_TERM priv_node_run_xpath_with_ns(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  ERL_NIF_TERM ns_hash;
  ErlNifBinary xpath_b;
  ERL_NIF_TERM pf_atom;
  xmlXPathContextPtr ctx;
  Node *node;
  xmlChar* query;
  xmlXPathObjectPtr xpath;
  Errors* errors;
  ERL_NIF_TERM result;
  unsigned int err_len;

  if (argc != 3) {
    return enif_make_badarg(env);
  }
  if (!enif_is_map(env, argv[2])) {
    return enif_make_badarg(env);
  }
  if (!enif_inspect_binary(env, argv[1], &xpath_b)) {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0], EXN_RES_TYPE, (void **)&node)) {
    return enif_make_badarg(env);
  }

  ns_hash = argv[2];
  xmlDocPtr ptr = node->doc;
  ctx = xmlXPathNewContext(ptr);
  register_xml_nses_from_map(ctx, env, ns_hash);
  query = nif_binary_to_xmlChar(&xpath_b);

  errors = enif_alloc(sizeof(Errors));
  errors->env = env;
  errors->errors = enif_make_list(env, 0);

  xmlResetLastError();
  xmlSetStructuredErrorFunc(errors, Exogiri_error_array_pusher);
  xmlSetGenericErrorFunc(NULL, xpath_generic_exception_handler);
  xpath = xmlXPathNodeEval(node->node, query, ctx);
  xmlSetStructuredErrorFunc(NULL, NULL);
  xmlSetGenericErrorFunc(NULL, NULL);
  enif_get_list_length(env,errors->errors,&err_len);
  if (err_len > 0) {
    result = enif_make_tuple2(
        env,
        atom_error,
        errors->errors
    );
    enif_free(errors);
    xmlXPathFreeContext(ctx);
    enif_free(query);
    if (xpath) {
      xmlXPathFreeObject(xpath);
    }
    return result;
  }
  if (xpath == NULL) {
    enif_free(errors);
    xmlErrorPtr error = xmlGetLastError();
    if (error) {
      result = enif_make_tuple2(
        env,
        atom_error,
        enif_make_list1(
          env,
          enif_make_string(env, error->message, ERL_NIF_LATIN1)
        )
      );
    } else {
      enif_make_existing_atom(
        env,
        "unknown_error",
        &pf_atom,
        ERL_NIF_LATIN1
      );
      result = enif_make_tuple2(
        env,
        atom_error,
        pf_atom
      );
    }
    enif_free(errors);
    xmlXPathFreeContext(ctx);
    enif_free(query);
    return result;
  }
  // TODO: build actual nodes

  xmlXPathFreeObject(xpath);
  xmlXPathFreeContext(ctx);
  enif_free(query);
  enif_free(errors);
  return atom_ok;
}