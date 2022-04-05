#pragma once

#include <erl_nif.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/c14n.h>
#include <libxml/xmlschemas.h>

#include "unlinked_node.h"

ErlNifResourceType* EXD_RES_TYPE;
ErlNifResourceType* EXN_RES_TYPE;
ErlNifResourceType* EXS_RES_TYPE;

typedef struct {
  ErlNifPid* owner;
  xmlDocPtr doc;
  struct UnlinkedNode *unlinked_nodes;
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

#define ASSIGN_OK(env, atom_target) enif_make_existing_atom(env, "ok", &atom_target, ERL_NIF_LATIN1)
#define ASSIGN_ERROR(env, atom_target) enif_make_existing_atom(env, "error", &atom_target, ERL_NIF_LATIN1)
#define ASSIGN_NONE(env, atom_target) enif_make_existing_atom(env, "none", &atom_target, ERL_NIF_LATIN1)