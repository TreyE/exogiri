#pragma once

#include <erl_nif.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

extern ERL_NIF_TERM atom_ok;
extern ERL_NIF_TERM atom_error;
extern ERL_NIF_TERM atom_none;

ErlNifResourceType* EXD_RES_TYPE;
ErlNifResourceType* EXN_RES_TYPE;

typedef struct {
  xmlDocPtr doc;
} Document;

typedef struct {
  Document ** doc;
  xmlNodePtr node;
} Node;

typedef struct {
  ErlNifEnv* env;
  ERL_NIF_TERM errors;
} Errors;