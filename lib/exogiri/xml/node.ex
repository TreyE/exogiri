defmodule Exogiri.Xml.Node do
  # use Bundlex.Loader, nif: :exogiri_xml_node

  defstruct [:ref]

  @opaque t :: %__MODULE__{}

  def local_name(%__MODULE__{} = a) do
    Exogiri.Xml.Internal.priv_node_local_name(a.ref)
  end

  def namespace(%__MODULE__{} = a) do
    case Exogiri.Xml.Internal.priv_node_namespace(a.ref) do
      :none -> nil
      a -> a
    end
  end

  def xpath(%__MODULE__{} = a, xpath, nses) do
    Exogiri.Xml.Internal.priv_node_run_xpath_with_ns(a.ref, xpath, nses)
  end
end
