#include <string.h>
#include "exogiri.h"
#include "node.h"

void free_document(ErlNifEnv* __attribute__((unused))env, void* obj)
{
  Document* document = (Document *)obj;
  xmlFreeDoc(document->doc);
}

ERL_NIF_TERM priv_from_string(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  ErlNifBinary nb;
  ERL_NIF_TERM result;
  ERL_NIF_TERM result_tuple;
  Errors* parse_errors;
  unsigned int error_size;

  xmlDocPtr doc;

  const char *in_string;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }

  if (!enif_inspect_binary(env, argv[0], &nb)) {
    return enif_make_badarg(env);
  }
  in_string = nb.data;

  xmlResetLastError();
  parse_errors = (Errors*)enif_alloc(sizeof(Errors));
  parse_errors->env = env;
  parse_errors->errors = enif_make_list(env, 0);
  xmlSetStructuredErrorFunc((void *)parse_errors, Exogiri_error_array_pusher);
  doc = xmlReadMemory(in_string, nb.size, NULL, NULL, 1);

  enif_get_list_length(env, parse_errors->errors, &error_size);
  if (error_size > 0) {
    xmlFreeDoc(doc);
    result = parse_errors->errors;
    result_tuple = enif_make_tuple2(
      env,
      atom_error,
      result
    );
    enif_free(parse_errors);
    return result_tuple;
  }

  enif_free(parse_errors);

  if (doc == NULL) {
    xmlFreeDoc(doc);
    enif_make_existing_atom(
      env,
      "parse_failed",
      &result,
      ERL_NIF_LATIN1
    );
    result_tuple = enif_make_tuple2(
      env,
      atom_error,
      result
    );
    return result_tuple;
  }
  
  Document* docRes = (Document *)enif_alloc_resource(EXD_RES_TYPE, sizeof(Document));
  docRes->doc = doc;
  result = enif_make_resource(env, docRes);
  enif_release_resource(docRes);
  result_tuple = enif_make_tuple2(
      env,
      atom_ok,
      result
    );
  return result_tuple;
}

ERL_NIF_TERM priv_get_root(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Document *document;
  xmlNodePtr np;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXD_RES_TYPE,(void **)&document)) {
    return enif_make_badarg(env);
  }
  np = xmlDocGetRootElement(document->doc);
  return create_node_term(env, document, np);
}

ERL_NIF_TERM priv_to_xml(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  ERL_NIF_TERM result;
  ErlNifBinary nb;
  Document *document;
  xmlDocPtr doc;
  int docSize;
  xmlChar* dumpedDoc;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXD_RES_TYPE,(void **)&document)) {
    return enif_make_badarg(env);
  }

  doc = document->doc;

  xmlDocDumpMemoryEnc(
    doc,
    &dumpedDoc,
    &docSize,
    "UTF-8"
  );

  enif_alloc_binary(docSize,&nb);
  memcpy(nb.data, dumpedDoc, docSize);
  result = enif_make_binary(env,&nb);
  enif_release_binary(&nb);

  xmlFree(dumpedDoc);
  return result;
}