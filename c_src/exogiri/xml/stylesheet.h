#pragma once

#include "exogiri.h"

void free_stylesheet(__attribute__((unused))ErlNifEnv* env, void* obj);
ERL_NIF_TERM priv_stylesheet_from_string(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_stylesheet_transform(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);