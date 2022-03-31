#pragma once

#include "exogiri.h"

void free_schema(ErlNifEnv* __attribute__((unused))env, void* obj);

ERL_NIF_TERM priv_schema_from_string(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_schema_from_string_with_path(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_schema_validate_doc(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);