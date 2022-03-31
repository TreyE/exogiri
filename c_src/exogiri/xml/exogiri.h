#pragma once

#include <erl_nif.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathinternals.h>
#include <libxml/c14n.h>
#include <libxml/xmlschemas.h>

extern ERL_NIF_TERM atom_ok;
extern ERL_NIF_TERM atom_error;
extern ERL_NIF_TERM atom_none;

ErlNifResourceType* EXD_RES_TYPE;
ErlNifResourceType* EXN_RES_TYPE;
ErlNifResourceType* EXS_RES_TYPE;

typedef struct {
  ErlNifPid* owner;
  xmlDocPtr doc;
} Document;

typedef struct {
  ErlNifPid* owner;
  Document *prev_doc;
  Document *doc;
  xmlNodePtr node;
} Node;

typedef struct {
  ErlNifEnv* env;
  ERL_NIF_TERM errors;
} Errors;

typedef struct {
  ErlNifPid* owner;
  xmlSchemaPtr schema;
  xmlDocPtr doc;
} Schema;

void Exogiri_error_array_pusher(void *ctx, xmlErrorPtr error);
xmlChar *nif_binary_to_xmlChar(ErlNifBinary * bin);
ERL_NIF_TERM xml_char_to_binary_term(ErlNifEnv* env, xmlChar *content);

#define CHECK_STRUCT_OWNER(env_var, pid_var, struct_var) enif_self(env_var, &pid_var); \
  if (enif_compare_pids(&pid_var,struct_var->owner) != 0) { \
    return enif_make_badarg(env); \
  }