#pragma once

#include "exogiri.h"

void free_node(ErlNifEnv* __attribute__((unused))env, void* obj);
ERL_NIF_TERM node_list_to_term(ErlNifEnv* env, Document* document, xmlNodeSetPtr nodes);

ERL_NIF_TERM priv_node_local_name(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_namespace(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_namespaces(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM create_node_term(ErlNifEnv* env, Document* document, xmlNodePtr np);
ERL_NIF_TERM priv_node_unlink(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_add_child(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_content(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_set_content(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_attribute_value(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_set_attribute_value(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_children(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_parent(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_previous_sibling(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_next_sibling(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_last_element_child(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_first_element_child(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_add_next_sibling(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM priv_node_add_previous_sibling(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);