#include "exogiri.h"
#include "node.h"
#include "document.h"
#include "xpath.h"

static int
open_resource(ErlNifEnv* env)
{
    const char* mod = "exogiri_xml_document";
    const char* name = "Elixir.Exogiri.Xml.Internal.Nif";
    const char* n_mod = "exogiri_xml_node";
    const char* n_name = "Elixir.Exogiri.Xml.Internal.Nif";

    int flags = ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER;

    EXD_RES_TYPE = enif_open_resource_type(env, mod, name, free_document, flags, NULL);
    EXN_RES_TYPE = enif_open_resource_type(env, n_mod, n_name, free_node, flags, NULL);
    if(EXD_RES_TYPE == NULL) return -1;
    if(EXN_RES_TYPE == NULL) return -1;
    return 0;
}

static int
load(ErlNifEnv* env, void** __attribute__((unused))priv, ERL_NIF_TERM __attribute__((unused))load_info)
{
    if(open_resource(env) == -1) return -1;

    return 0;
}

// Erlang requires that we re-open resources on re-initialisation.
static int
reload(ErlNifEnv* env, void** __attribute__((unused))priv, ERL_NIF_TERM __attribute__((unused))load_info)
{
    if(open_resource(env) == -1) return -1;
    return 0;
}

static int
upgrade(ErlNifEnv* env, void** __attribute__((unused))priv, void** __attribute__((unused))old_priv, ERL_NIF_TERM __attribute__((unused)) load_info)
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
  {"priv_node_content", 1, priv_node_content, 0},
  {"priv_node_set_content", 2, priv_node_set_content, 0},
  {"priv_node_attribute_value", 2, priv_node_attribute_value, 0},
  {"priv_node_set_attribute_value", 3, priv_node_set_attribute_value, 0},
  {"priv_node_children", 1, priv_node_children, ERL_NIF_DIRTY_JOB_CPU_BOUND},
  {"priv_node_parent", 1, priv_node_parent, 0}
};

ERL_NIF_INIT(Elixir.Exogiri.Xml.Internal.Nif,nif_funcs,&load,&reload,&upgrade,NULL)