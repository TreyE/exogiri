#pragma once

#include "exogiri.h"

void free_node(ErlNifEnv* __attribute__((unused))env, void* obj);

ERL_NIF_TERM priv_node_local_name(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_namespace(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);