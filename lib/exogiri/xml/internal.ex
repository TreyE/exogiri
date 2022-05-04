defmodule Exogiri.Xml.Internal do
  @moduledoc false
  use Bundlex.Loader, nif: :exogiri_xml_internal

  defnif priv_from_string(string)
  defnif priv_to_xml(doc_ref)
  defnif priv_doc_get_root(doc_ref)
  defnif priv_doc_canonicalize(doc_ref)
  defnif priv_node_local_name(node_ref)
  defnif priv_node_namespace(node_ref)
  defnif priv_node_namespaces(node_ref)
  defnif priv_node_run_xpath_with_ns(node_ref,xpath,nses)
  defnif priv_node_unlink(node_ref)
  defnif priv_node_add_child(parent_node_ref, child_node_ref)
  defnif priv_node_content(node_ref)
  defnif priv_node_set_content(node_ref, content)
  defnif priv_node_attribute_value(node_ref, attr_name)
  defnif priv_node_set_attribute_value(node_ref, attr_name, attr_value)
  defnif priv_node_children(node_ref)
  defnif priv_node_parent(node_ref)
  defnif priv_node_previous_element_sibling(node_ref)
  defnif priv_node_next_element_sibling(node_ref)
  defnif priv_node_last_element_child(node_ref)
  defnif priv_node_first_element_child(node_ref)
  defnif priv_node_add_next_sibling(node_ref, sibling_ref)
  defnif priv_node_add_previous_sibling(node_ref, sibling_ref)
  defnif priv_node_create_no_ns(doc_ref, node_name)
  defnif priv_node_create_with_ns(doc_ref, node_name, ns_abbrev, ns_href)

  defnif priv_schema_from_string(string)
  defnif priv_schema_from_string_with_path(string, path)
  defnif priv_schema_validate_doc(schema_ref,doc_ref)

  defnif priv_doc_new_root_no_ns(root_name)
  defnif priv_doc_new_root_with_ns(root_name, root_abbrev, root_href)

  defnif priv_stylesheet_from_string(string)
  defnif priv_stylesheet_transform(ss_ref, doc_ref, params_list)
end
