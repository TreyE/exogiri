#include "exogiri.h"

void free_stylesheet(__attribute__((unused))ErlNifEnv* env, void* obj) {
  Stylesheet* stylesheet = (Stylesheet*)obj;
  xsltFreeStylesheet(stylesheet->stylesheet);
  enif_free(stylesheet->owner);
}

static void
xslt_generic_error_handler(void *ctx, const char *msg, ...)
{
  Errors* errors = (Errors*)ctx;
  ERL_NIF_TERM new_error;
  ERL_NIF_TERM existing_errors;
  char *message;
  size_t err_size;
  va_list args;
  va_list args_for_len;

  va_start(args, msg);
  va_copy(args_for_len, args);
  err_size = vsnprintf(NULL, 0, msg, args_for_len);
  message = (char*)malloc(sizeof(char) * err_size + 1);
  vsnprintf(message, err_size, msg, args);
  va_end(args);
  existing_errors = errors->errors;
  new_error = enif_make_string(errors->env, message, ERL_NIF_LATIN1);
  errors->errors = enif_make_list_cell(
    errors->env,
    new_error,
    existing_errors
  );
  free(message);
}

static ERL_NIF_TERM read_stylesheet_from_doc(ErlNifEnv* env, xmlDocPtr doc) {
  ErlNifPid *self;
  xsltStylesheetPtr ss;
  Errors* parse_errors;
  ERL_NIF_TERM result;
  ERL_NIF_TERM pf_atom;
  ERL_NIF_TERM atom_result;
  unsigned int err_len;

  self = (ErlNifPid *)enif_alloc(sizeof(ErlNifPid));
  enif_self(env, self);

  parse_errors = (Errors*)enif_alloc(sizeof(Errors));
  parse_errors->env = env;
  parse_errors->errors = enif_make_list(env, 0);
  
  xsltSetGenericErrorFunc((void *)parse_errors, xslt_generic_error_handler);
  ss = xsltParseStylesheetDoc(doc);
  xsltSetGenericErrorFunc(NULL, NULL);
  enif_get_list_length(env,parse_errors->errors,&err_len);
  if (err_len > 0) {
    if (!ss) {
      xmlFreeDoc(doc);
    } else {
      xsltFreeStylesheet(ss);
    }
    ASSIGN_ERROR(env, atom_result);
    result = enif_make_tuple2(
        env,
        atom_result,
        parse_errors->errors
    );
    enif_free(parse_errors);
    return result;
  }
  if (!ss) {
    xmlFreeDoc(doc);
    enif_make_existing_atom(
      env,
      "unknown_error",
      &pf_atom,
      ERL_NIF_LATIN1
    );
    ASSIGN_ERROR(env, atom_result);
    result = enif_make_tuple2(
      env,
      atom_result,
      pf_atom
    );
    enif_free(parse_errors);
    return result;
  }
  
  enif_free(parse_errors);

  Stylesheet* ssRes = (Stylesheet *)enif_alloc_resource(EXSS_RES_TYPE, sizeof(Stylesheet));
  ssRes->owner = self;
  ssRes->stylesheet = ss;
  result = enif_make_resource(env, ssRes);
  enif_release_resource(ssRes);

  ASSIGN_OK(env, atom_result);
  return enif_make_tuple2(
    env,
    atom_result,
    result
  );
}

ERL_NIF_TERM priv_stylesheet_from_string(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  ErlNifBinary nb;
  ERL_NIF_TERM atom_result;
  xmlDocPtr doc;
  Errors* parse_errors;
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

  parse_errors = (Errors*)enif_alloc(sizeof(Errors));
  parse_errors->env = env;
  parse_errors->errors = enif_make_list(env, 0);

  xmlSetStructuredErrorFunc((void *)parse_errors, Exogiri_error_array_pusher);
  doc = xmlReadMemory(
    (const char *)nb.data,
    nb.size,
    NULL,
    NULL,
    0
  );
  xmlSetStructuredErrorFunc(NULL, NULL);

  enif_get_list_length(env,parse_errors->errors,&err_len);
  if (err_len > 0) {
    if (doc) {
      xmlFreeDoc(doc);
    }
    ASSIGN_ERROR(env, atom_result);
    result = enif_make_tuple2(
        env,
        atom_result,
        parse_errors->errors
    );
    enif_free(parse_errors);
    return result;
  }
  if (!doc) {
    enif_make_existing_atom(
      env,
      "unknown_error",
      &pf_atom,
      ERL_NIF_LATIN1
    );
    ASSIGN_ERROR(env, atom_result);
    result = enif_make_tuple2(
      env,
      atom_result,
      pf_atom
    );
    enif_free(parse_errors);
    return result;
  }

  enif_free(parse_errors);

  return read_stylesheet_from_doc(env, doc);
}

static void free_params_array(char **p_array) {
  char *cur_ptr;

  if (!p_array) {
    return;
  }

  int i = 0;
  cur_ptr = p_array[i];

  while(cur_ptr != NULL) {
    enif_free(cur_ptr);
    i++;
    cur_ptr = p_array[i];
  }

  enif_free(p_array);
}

static char **extract_transform_params(ErlNifEnv* env, ERL_NIF_TERM param_list, unsigned int list_size) {
  char **transform_params;
  ERL_NIF_TERM head;
  ERL_NIF_TERM tail;
  ERL_NIF_TERM tail2;
  ERL_NIF_TERM current;
  ErlNifBinary kb;
  ErlNifBinary vb;
  char *kc;
  char *vc;
  size_t allocated_size;

  if (list_size < 1) {
    return NULL;
  }

  allocated_size = (sizeof(char *) * list_size) + 1;

  transform_params = enif_alloc(allocated_size);
  transform_params[list_size] = NULL;

  current = param_list;

  for (unsigned int i = 0; i < list_size; i = i + 2) {
    enif_get_list_cell(env, current, &head, &tail);
    enif_inspect_binary(env, head, &kb);
    kc = nif_binary_to_char(&kb);
    enif_get_list_cell(env, tail, &head, &tail2);
    enif_inspect_binary(env, head, &vb);
    vc = nif_binary_to_char(&vb);
    transform_params[i] = kc;
    transform_params[i + 1] = vc;
    current = tail2;
  }

  return transform_params;
}

ERL_NIF_TERM priv_stylesheet_transform(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  ERL_NIF_TERM atom_result;
  ERL_NIF_TERM pf_atom;
  ERL_NIF_TERM result;
  xmlDocPtr result_doc;
  Document *document;
  Stylesheet *s_sheet;
  ErlNifPid self;
  ErlNifPid *doc_owner;
  Errors *parse_errors;
  char **transform_params;
  unsigned int err_len;
  unsigned int param_len;

  if(argc != 3)
  {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[0],EXSS_RES_TYPE,(void **)&s_sheet)) {
    return enif_make_badarg(env);
  }
  if (!enif_get_resource(env, argv[1],EXD_RES_TYPE,(void **)&document)) {
    return enif_make_badarg(env);
  }
  if (!enif_is_list(env, argv[2])) {
    return enif_make_badarg(env);
  }

  CHECK_STRUCT_OWNER(env, self, s_sheet)
  CHECK_STRUCT_OWNER(env, self, document)

  parse_errors = (Errors*)enif_alloc(sizeof(Errors));
  parse_errors->env = env;
  parse_errors->errors = enif_make_list(env, 0);

  enif_get_list_length(env, argv[2], &param_len);
  transform_params = extract_transform_params(env, argv[2],param_len);

  xsltSetGenericErrorFunc((void *)parse_errors, xslt_generic_error_handler);
  xmlSetGenericErrorFunc((void *)parse_errors, xslt_generic_error_handler);
  result_doc = xsltApplyStylesheet(s_sheet->stylesheet, document->doc, (const char **)transform_params);;
  xsltSetGenericErrorFunc(NULL, NULL);
  xmlSetGenericErrorFunc(NULL, NULL);

  free_params_array(transform_params);

  enif_get_list_length(env,parse_errors->errors,&err_len);
  if (err_len > 0) {
    if (result_doc) {
      xmlFreeDoc(result_doc);
    }
    ASSIGN_ERROR(env, atom_result);
    result = enif_make_tuple2(
        env,
        atom_result,
        parse_errors->errors
    );
    enif_free(parse_errors);
    return result;
  }
  if (!result_doc) {
    enif_make_existing_atom(
      env,
      "unknown_error",
      &pf_atom,
      ERL_NIF_LATIN1
    );
    ASSIGN_ERROR(env, atom_result);
    result = enif_make_tuple2(
      env,
      atom_result,
      pf_atom
    );
    enif_free(parse_errors);
    return result;
  }

  enif_free(parse_errors);

  doc_owner = (ErlNifPid *)enif_alloc(sizeof(ErlNifPid));
  enif_self(env, doc_owner);

  Document* docRes = (Document *)enif_alloc_resource(EXD_RES_TYPE, sizeof(Document));
  docRes->owner = doc_owner;
  docRes->doc = result_doc;
  docRes->unlinked_nodes = NULL;
  docRes->unlinked_nses = NULL;
  result = enif_make_resource(env, docRes);
  enif_release_resource(docRes);

  ASSIGN_OK(env, atom_result);
  return enif_make_tuple2(
      env,
      atom_result,
      result
  );
}