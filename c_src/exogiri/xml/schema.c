#include "exogiri.h"

void free_schema(ErlNifEnv* __attribute__((unused))env, void* obj) {
  Schema* schema = (Schema*)obj;
  xmlSchemaFree(schema->schema);
  if (schema->doc) {
    xmlFreeDoc(schema->doc);
  }
}

ERL_NIF_TERM priv_schema_from_string(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifPid *self;
  ErlNifBinary nb;
  Errors* parse_errors;
  xmlSchemaParserCtxtPtr ctx;
  xmlSchemaPtr schema;
  ERL_NIF_TERM pf_atom;
  ERL_NIF_TERM result;
  unsigned int err_len;

  if(argc != 1)
  {
    return enif_make_badarg(env);
  }

  if (!enif_inspect_binary(env, argv[0], &nb)) {
    return enif_make_badarg(env);
  }

  self = (ErlNifPid *)enif_alloc(sizeof(ErlNifPid));
  enif_self(env, self);

  parse_errors = (Errors*)enif_alloc(sizeof(Errors));
  parse_errors->env = env;
  parse_errors->errors = enif_make_list(env, 0);

  ctx = xmlSchemaNewMemParserCtxt((const char *)nb.data, nb.size);
  xmlSetStructuredErrorFunc((void *)parse_errors, Exogiri_error_array_pusher);
  schema = xmlSchemaParse(ctx);
  xmlSetStructuredErrorFunc(NULL, NULL);
  enif_get_list_length(env,parse_errors->errors,&err_len);
  if (err_len > 0) {
    if (schema) {
      xmlSchemaFree(schema);
    }
    xmlSchemaFreeParserCtxt(ctx);
    result = enif_make_tuple2(
        env,
        atom_error,
        parse_errors->errors
    );
    enif_free(parse_errors);
    return result;
  }
  if (!schema) {
    xmlSchemaFreeParserCtxt(ctx);
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
    enif_free(parse_errors);
    return result;
  }

  enif_free(parse_errors);
  xmlSchemaFreeParserCtxt(ctx);

  Schema* schemaRes = (Schema *)enif_alloc_resource(EXS_RES_TYPE, sizeof(Schema));
  schemaRes->owner = self;
  schemaRes->schema = schema;
  schemaRes->doc = NULL;
  result = enif_make_resource(env, schemaRes);
  enif_release_resource(schemaRes);
  return enif_make_tuple2(
    env,
    atom_ok,
    result
  );
}

ERL_NIF_TERM priv_schema_from_string_with_path(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifPid *self;
  ErlNifBinary nb;
  ErlNifBinary path_b;
  Errors* parse_errors;
  xmlSchemaParserCtxtPtr ctx;
  xmlSchemaPtr schema;
  xmlDocPtr doc;
  xmlChar* path;
  ERL_NIF_TERM pf_atom;
  ERL_NIF_TERM result;
  unsigned int err_len;

  if(argc != 2)
  {
    return enif_make_badarg(env);
  }

  if (!enif_inspect_binary(env, argv[0], &nb)) {
    return enif_make_badarg(env);
  }
  if (!enif_inspect_binary(env, argv[1], &path_b)) {
    return enif_make_badarg(env);
  }

  self = (ErlNifPid *)enif_alloc(sizeof(ErlNifPid));
  enif_self(env, self);

  parse_errors = (Errors*)enif_alloc(sizeof(Errors));
  parse_errors->env = env;
  parse_errors->errors = enif_make_list(env, 0);

  path = nif_binary_to_xmlChar(&path_b);

  xmlSetStructuredErrorFunc((void *)parse_errors, Exogiri_error_array_pusher);
  doc = xmlReadMemory(
    (const char *)nb.data,
    nb.size,
    (const char *)path,
    NULL,
    0
  );
  xmlSetStructuredErrorFunc(NULL, NULL);
  enif_get_list_length(env,parse_errors->errors,&err_len);
  if (err_len > 0) {
    enif_free(path);
    if (doc) {
      xmlFreeDoc(doc);
    }
    result = enif_make_tuple2(
        env,
        atom_error,
        parse_errors->errors
    );
    enif_free(parse_errors);
    return result;
  }
  if (!doc) {
    enif_free(path);
    xmlFreeDoc(doc);
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
    enif_free(parse_errors);
    return result;
  }

  ctx = xmlSchemaNewDocParserCtxt(doc);
  if (!ctx) {
    enif_free(path);
    xmlFreeDoc(doc);
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
    enif_free(parse_errors);
    return result;
  }

  xmlSetStructuredErrorFunc((void *)parse_errors, Exogiri_error_array_pusher);
  schema = xmlSchemaParse(ctx);
  xmlSetStructuredErrorFunc(NULL, NULL);
  enif_get_list_length(env,parse_errors->errors,&err_len);

  if (err_len > 0) {
    enif_free(path);
    xmlFreeDoc(doc);
    if (schema) {
      xmlSchemaFree(schema);
    }
    xmlSchemaFreeParserCtxt(ctx);
    result = enif_make_tuple2(
        env,
        atom_error,
        parse_errors->errors
    );
    enif_free(parse_errors);
    return result;
  }
  if (!schema) {
    enif_free(path);
    xmlFreeDoc(doc);
    xmlSchemaFreeParserCtxt(ctx);
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
    enif_free(parse_errors);
    return result;
  }
  enif_free(path);
  enif_free(parse_errors);
  xmlSchemaFreeParserCtxt(ctx);

  Schema* schemaRes = (Schema *)enif_alloc_resource(EXS_RES_TYPE, sizeof(Schema));
  schemaRes->owner = self;
  schemaRes->schema = schema;
  schemaRes->doc = doc;
  result = enif_make_resource(env, schemaRes);
  enif_release_resource(schemaRes);
  return enif_make_tuple2(
    env,
    atom_ok,
    result
  );
}