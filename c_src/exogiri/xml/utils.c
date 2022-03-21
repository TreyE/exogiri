#include <erl_nif.h>
#include <libxml/tree.h>
#include <string.h>

xmlChar *nif_binary_to_xmlChar(ErlNifBinary * bin) {
  xmlChar* result;
  int bin_size;
  bin_size = bin->size + 1;
  result = (xmlChar*)enif_alloc(bin_size);
  memset(result, 0, bin_size);
  memcpy(result, bin->data, bin->size);
  return result;
}

ERL_NIF_TERM xml_char_to_binary_term(ErlNifEnv* env, xmlChar *content) {
  ErlNifBinary nb;
  ERL_NIF_TERM result;
  size_t contentLen;

  contentLen = xmlStrlen(content);
  enif_alloc_binary(contentLen,&nb);
  memcpy(nb.data, content, contentLen);
  result = enif_make_binary(env,&nb);
  enif_release_binary(&nb);

  return result;
}