#include <string.h>
#include "exogiri.h"
#include "node.h"

void free_document(__attribute__((unused))ErlNifEnv* env, void* obj)
{
  Document* document = (Document *)obj;
  enif_free(document->owner);
  xmlFreeDoc(document->doc);
}

ERL_NIF_TERM priv_from_string(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  ErlNifBinary nb;
  ERL_NIF_TERM result;
  ERL_NIF_TERM result_tuple;
  ErlNifPid *self;
  Errors* parse_errors;
  xmlChar *in_str;
  unsigned int error_size;
  ERL_NIF_TERM atom_result;

  xmlDocPtr doc;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }

  if (!enif_inspect_binary(env, argv[0], &nb)) {
    return enif_make_badarg(env);
  }

  xmlResetLastError();
  parse_errors = (Errors*)enif_alloc(sizeof(Errors));
  parse_errors->env = env;
  parse_errors->errors = enif_make_list(env, 0);
  xmlSetStructuredErrorFunc((void *)parse_errors, Exogiri_error_array_pusher);
  in_str = nif_binary_to_xmlChar(&nb);
  doc = xmlParseDoc(in_str);
  enif_free(in_str);
  enif_get_list_length(env, parse_errors->errors, &error_size);
  if (error_size > 0) {
    xmlFreeDoc(doc);
    result = parse_errors->errors;
    ASSIGN_ERROR(env, atom_result);
    result_tuple = enif_make_tuple2(
      env,
      atom_result,
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
    ASSIGN_ERROR(env, atom_result);
    result_tuple = enif_make_tuple2(
      env,
      atom_result,
      result
    );
    return result_tuple;
  }
  self = (ErlNifPid *)enif_alloc(sizeof(ErlNifPid));
  enif_self(env, self);
  Document* docRes = (Document *)enif_alloc_resource(EXD_RES_TYPE, sizeof(Document));
  docRes->owner = self;
  docRes->doc = doc;
  result = enif_make_resource(env, docRes);
  enif_release_resource(docRes);
  ASSIGN_OK(env, atom_result);
  result_tuple = enif_make_tuple2(
      env,
      atom_result,
      result
    );
  return result_tuple;
}

ERL_NIF_TERM priv_get_root(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  Document *document;
  xmlNodePtr np;
  ErlNifPid self;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXD_RES_TYPE,(void **)&document)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, document)

  np = xmlDocGetRootElement(document->doc);
  return create_node_term(env, document, np);
}

ERL_NIF_TERM priv_to_xml(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  ERL_NIF_TERM result;
  ErlNifBinary nb;
  Document *document;
  xmlDocPtr doc;
  ErlNifPid self;
  int docSize;
  xmlChar* dumpedDoc;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXD_RES_TYPE,(void **)&document)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, document)

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

ERL_NIF_TERM priv_doc_canonicalize(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  ERL_NIF_TERM result;
  Document *document;
  ErlNifPid self;
  int docSize;
  xmlChar* dumpedDoc;
  ERL_NIF_TERM atom_error;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXD_RES_TYPE,(void **)&document)) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, document)

  docSize = xmlC14NDocDumpMemory(
    document->doc,
    NULL,
    XML_C14N_1_0,
    NULL,
    0,
    &dumpedDoc
  );
  if (docSize < 0) {
    ASSIGN_ERROR(env, atom_error);
    return atom_error;
  }
  result = xml_char_to_binary_term(env, dumpedDoc);
  xmlFree(dumpedDoc);

  return result;
}