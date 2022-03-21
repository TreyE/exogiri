defmodule Exogiri.Xml.Node do
  # use Bundlex.Loader, nif: :exogiri_xml_node

  defstruct [:ref]

  @typedoc "The Node type."
  @opaque t :: %__MODULE__{ref: reference()}

  @typedoc """
  You've either managed to bork the XPath parser (rare), OR my C code is bad (put your money here).
  """
  @type unknown_xpath_error :: {:error, :unknown_error}

  @spec local_name(Exogiri.Xml.Node.t()) :: binary()
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

  @spec unlink(Exogiri.Xml.Node.t()) :: :ok
  def unlink(%__MODULE__{} = a) do
    Exogiri.Xml.Internal.priv_node_unlink(a.ref)
  end

  @spec add_child(Exogiri.Xml.Node.t(), Exogiri.Xml.Node.t()) :: :ok
  def add_child(%__MODULE__{} = parent, %__MODULE__{} = child) do
    Exogiri.Xml.Internal.priv_node_add_child(parent.ref, child.ref)
  end

  @doc """
  Get the node content as a string.
  """
  @spec content(Exogiri.Xml.Node.t()) :: binary() | nil
  def content(%__MODULE__{} = a) do
    case Exogiri.Xml.Internal.priv_node_content(a.ref) do
      :none -> nil
      a -> a
    end
  end

  @doc """
  Retrieve the value of an attribute on the node by name.
  """
  @spec attribute_value(Exogiri.Xml.Node.t(), String.t) :: String.t | nil
  def attribute_value(%__MODULE__{} = a, attr_name) do
    case Exogiri.Xml.Internal.priv_node_attribute_value(a.ref, attr_name) do
      :none -> nil
      a -> a
    end
  end

  @doc """
  Set the value of an attribute on the node by name.
  """
  @spec set_attribute_value(Exogiri.Xml.Node.t(), String.t, String.t) :: :ok
  def set_attribute_value(%__MODULE__{} = a, attr_name, attr_value) do
    Exogiri.Xml.Internal.priv_node_set_attribute_value(a.ref, attr_name, attr_value)
  end
end
