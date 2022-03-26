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
end
