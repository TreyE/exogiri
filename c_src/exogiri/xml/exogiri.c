#include "exogiri.h"
#include "node.h"
#include "document.h"
#include "xpath.h"
#include "schema.h"
#include "stylesheet.h"

static int open_resource(ErlNifEnv* env)
{
    const char* d_mod_str = "Elixir.Exogiri.Xml.Internal.Nif.Document";
    const char* n_mod_str = "Elixir.Exogiri.Xml.Internal.Nif.Node";
    const char* s_mod_str = "Elixir.Exogiri.Xml.Internal.Nif.Schema";
    const char* ss_mod_str = "Elixir.Exogiri.Xml.Internal.Nif.StyleSheet";
    const char* d_name = "exogiri_xml_document";
    const char* n_name = "exogiri_xml_node";
    const char* s_name = "exogiri_xml_schema";
    const char* ss_name = "exogiri_xml_stylesheet";

    int flags = ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER;

    EXD_RES_TYPE = enif_open_resource_type(env, d_mod_str, d_name, free_document, flags, NULL);
    EXN_RES_TYPE = enif_open_resource_type(env, n_mod_str, n_name, free_node, flags, NULL);
    EXS_RES_TYPE = enif_open_resource_type(env, s_mod_str, s_name, free_schema, flags, NULL);
    EXSS_RES_TYPE = enif_open_resource_type(env, ss_mod_str, ss_name, free_stylesheet, flags, NULL);
    if(EXD_RES_TYPE == NULL) return -1;
    if(EXN_RES_TYPE == NULL) return -1;
    if(EXS_RES_TYPE == NULL) return -1;
    if(EXSS_RES_TYPE == NULL) return -1;
    return 0;
}

static int
load(ErlNifEnv* env, __attribute__((unused))void** priv, __attribute__((unused))ERL_NIF_TERM load_info)
{
    if(open_resource(env) == -1) return -1;

    return 0;
}

// Erlang requires that we re-open resources on re-initialisation.
static int
reload(ErlNifEnv* env, __attribute__((unused))void** priv, __attribute__((unused))ERL_NIF_TERM load_info)
{
    if(open_resource(env) == -1) return -1;
    return 0;
}

static int
upgrade(ErlNifEnv* env, __attribute__((unused))void** priv, __attribute__((unused))void** old_priv, __attribute__((unused))ERL_NIF_TERM load_info)
{
    if(open_resource(env) == -1) return -1;
    return 0;
}

static ErlNifFunc nif_funcs[] =
{
  {"priv_from_string", 1, priv_from_string, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_to_xml", 1, priv_to_xml, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_doc_get_root", 1, priv_get_root, 0},
  {"priv_doc_canonicalize", 1,priv_doc_canonicalize, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_node_local_name", 1, priv_node_local_name, 0},
  {"priv_node_namespace", 1, priv_node_namespace, 0},
  {"priv_node_namespaces", 1, priv_node_namespaces, 0},
  {"priv_node_run_xpath_with_ns", 3, priv_node_run_xpath_with_ns, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_node_unlink", 1, priv_node_unlink, 0},
  {"priv_node_add_child", 2, priv_node_add_child, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_node_content", 1, priv_node_content, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_node_set_content", 2, priv_node_set_content, 0},
  {"priv_node_attribute_value", 2, priv_node_attribute_value, 0},
  {"priv_node_set_attribute_value", 3, priv_node_set_attribute_value, 0},
  {"priv_node_children", 1, priv_node_children, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_node_parent", 1, priv_node_parent, 0},
  {"priv_node_next_element_sibling", 1, priv_node_next_sibling, 0},
  {"priv_node_previous_element_sibling", 1, priv_node_previous_sibling, 0},
  {"priv_node_first_element_child", 1, priv_node_first_element_child, 0},
  {"priv_node_last_element_child", 1, priv_node_last_element_child, 0},
  {"priv_node_add_next_sibling", 2, priv_node_add_next_sibling, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_node_add_previous_sibling", 2, priv_node_add_previous_sibling, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_node_create_no_ns", 2, priv_node_create_no_ns, 0},
  {"priv_node_create_with_ns", 4, priv_node_create_with_ns, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_schema_from_string", 1, priv_schema_from_string, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_schema_from_string_with_path", 2, priv_schema_from_string_with_path, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_schema_validate_doc", 2, priv_schema_validate_doc, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_stylesheet_from_string", 1, priv_stylesheet_from_string, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_doc_new_root_no_ns", 1, priv_doc_new_root_no_ns, 0},
  {"priv_doc_new_root_with_ns", 3, priv_doc_new_root_with_ns, 0}
};

ERL_NIF_INIT(Elixir.Exogiri.Xml.Internal.Nif,nif_funcs,&load,&reload,&upgrade,NULL)