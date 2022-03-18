defmodule Exogiri.Xml.Internal do
  @moduledoc false
  use Bundlex.Loader, nif: :exogiri_xml_internal

  defnif priv_from_string(string)
  defnif priv_to_xml(doc_ref)
  defnif priv_doc_get_root(doc_ref)
  defnif priv_node_local_name(node_ref)
  defnif priv_node_namespace(node_ref)
  defnif priv_node_run_xpath_with_ns(node_ref,xpath,nses)
end
