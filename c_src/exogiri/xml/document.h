#pragma once

#include "exogiri.h"

void free_document(ErlNifEnv* __attribute__((unused))env, void* obj);
void Exogiri_error_array_pusher(void *ctx, xmlErrorPtr error);

ERL_NIF_TERM priv_get_root(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_to_xml(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_from_string(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);