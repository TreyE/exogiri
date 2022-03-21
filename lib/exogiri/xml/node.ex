defmodule Exogiri.Xml.Node do
  # use Bundlex.Loader, nif: :exogiri_xml_node

  defstruct [:ref]

  @typedoc "The Node type."
  @opaque t :: %__MODULE__{ref: reference()}

  @typedoc """
  You've either managed to bork the parser (rare), OR my C code is bad (put your money here).
  """
  @type unknown_xpath_error :: {:error, :unknown_error}

  def local_name(%__MODULE__{} = a) do
    Exogiri.Xml.Internal.priv_node_local_name(a.ref)
  end

  def namespace(%__MODULE__{} = a) do
    case Exogiri.Xml.Internal.priv_node_namespace(a.ref) do
      :none -> nil
      a -> a
    end
  end

  @spec xpath(Exogiri.Xml.Node.t(), any, any) ::
    unknown_xpath_error() |
    {:error, [term]} |
    {:ok, [t]}
  def xpath(%__MODULE__{} = a, xpath, nses) do
    case Exogiri.Xml.Internal.priv_node_run_xpath_with_ns(a.ref, xpath, nses) do
      {:error, a} -> {:error, a}
      {:ok, list} -> {:ok, Enum.map(Enum.reverse(list), fn(n) -> %Exogiri.Xml.Node{ref: n} end)}
    end
  end

  def unlink(%__MODULE__{} = a) do
    Exogiri.Xml.Internal.priv_node_unlink(a.ref)
  end

  def add_child(%__MODULE__{} = parent, %__MODULE__{} = child) do
    Exogiri.Xml.Internal.priv_node_add_child(parent.ref, child.ref)
  end

  @spec content(Exogiri.Xml.Node.t()) :: binary()
  def content(%__MODULE__{} = a) do
    Exogiri.Xml.Internal.priv_node_content(a.ref)
  end

  @spec attribute_value(Exogiri.Xml.Node.t(), String.t) :: String.t | nil
  def attribute_value(%__MODULE__{} = a, attr_name) do
    case Exogiri.Xml.Internal.priv_node_attribute_value(a.ref, attr_name) do
      :none -> nil
      a -> a
    end
  end
end
