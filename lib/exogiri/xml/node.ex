defmodule Exogiri.Xml.Node do

  defstruct [:ref]

  @typedoc "The Node type."
  @opaque t :: %__MODULE__{ref: reference()}

  @typedoc """
  You've either managed to bork the XPath parser (rare), OR my C code is bad (put your money here).
  """
  @type unknown_xpath_error :: {:error, :unknown_error}

  @doc """
  Get the local name of a Node.
  """
  @spec local_name(Exogiri.Xml.Node.t()) :: binary()
  def local_name(%__MODULE__{} = a) do
    Exogiri.Xml.Internal.priv_node_local_name(a.ref)
  end

  @doc """
  Get the namespace of a node, as {abbreviation, href}.
  """
  def namespace(%__MODULE__{} = a) do
    case Exogiri.Xml.Internal.priv_node_namespace(a.ref) do
      :none -> nil
      a -> a
    end
  end

  @doc """
  Get the list of namespaces in this node's context.
  """
  def namespaces(%__MODULE__{} = a) do
    case Exogiri.Xml.Internal.priv_node_namespaces(a.ref) do
      :none -> nil
      a -> a
    end
  end

  @doc """
  Run an XPath expression, with namespaces, using the node as the root.
  """
  @spec xpath(Exogiri.Xml.Node.t(), String.t(), map()) ::
    unknown_xpath_error() |
    {:error, [term]} |
    {:ok, [t]}
  def xpath(%__MODULE__{} = a, xpath_expression, nses) when is_binary(xpath_expression) and is_map(nses) do
    case Exogiri.Xml.Internal.priv_node_run_xpath_with_ns(a.ref, xpath_expression, nses) do
      {:error, a} -> {:error, a}
      {:ok, list} -> {:ok, Enum.map(Enum.reverse(list), fn(n) -> %Exogiri.Xml.Node{ref: n} end)}
    end
  end

  @doc """
  Unlink a node from its parent.
  """
  @spec unlink(Exogiri.Xml.Node.t()) :: :ok
  def unlink(%__MODULE__{} = a) do
    Exogiri.Xml.Internal.priv_node_unlink(a.ref)
  end

  @doc """
  Add a child node.

  It will be moved from the previous parent if already had one.
  """
  @spec add_child(Exogiri.Xml.Node.t(), Exogiri.Xml.Node.t()) :: :ok
  def add_child(%__MODULE__{} = parent, %__MODULE__{} = child) do
    Exogiri.Xml.Internal.priv_node_add_child(parent.ref, child.ref)
  end

  @doc """
  Get the node content as a string.
  """
  @spec content(Exogiri.Xml.Node.t()) :: String.t | nil
  def content(%__MODULE__{} = a) do
    case Exogiri.Xml.Internal.priv_node_content(a.ref) do
      :none -> nil
      a -> a
    end
  end

  @doc """
  Set the node content as a string.
  """
  @spec set_content(Exogiri.Xml.Node.t(), String.t) :: :ok
  def set_content(%__MODULE__{} = a, content) do
    Exogiri.Xml.Internal.priv_node_set_content(a.ref, content)
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

  @doc """
  Return the children of a node.
  """
  @spec children(Exogiri.Xml.Node.t()) :: [t()]
  def children(%__MODULE__{} = a) do
    Enum.map(
      Exogiri.Xml.Internal.priv_node_children(a.ref),
      fn(n) ->
        %Exogiri.Xml.Node{ref: n}
      end
    )
  end

  @doc """
  Return the parent of a node.
  """
  @spec parent(Exogiri.Xml.Node.t()) :: nil | Exogiri.Xml.Node.t()
  def parent(%__MODULE__{} = a) do
    case Exogiri.Xml.Internal.priv_node_parent(a.ref) do
      :none -> nil
      n -> %Exogiri.Xml.Node{ref: n}
    end
  end

  @doc """
  Return the next sibling of a node.
  """
  @spec next_element_sibling(Exogiri.Xml.Node.t()) :: nil | Exogiri.Xml.Node.t()
  def next_element_sibling(%__MODULE__{} = a) do
    case Exogiri.Xml.Internal.priv_node_next_element_sibling(a.ref) do
      :none -> nil
      n -> %Exogiri.Xml.Node{ref: n}
    end
  end

  @doc """
  Return the previous sibling of a node.
  """
  @spec previous_element_sibling(Exogiri.Xml.Node.t()) :: nil | Exogiri.Xml.Node.t()
  def previous_element_sibling(%__MODULE__{} = a) do
    case Exogiri.Xml.Internal.priv_node_previous_element_sibling(a.ref) do
      :none -> nil
      n -> %Exogiri.Xml.Node{ref: n}
    end
  end

  @doc """
  Return the first element child of a node.
  """
  @spec first_element_child(Exogiri.Xml.Node.t()) :: nil | Exogiri.Xml.Node.t()
  def first_element_child(%__MODULE__{} = a) do
    case Exogiri.Xml.Internal.priv_node_first_element_child(a.ref) do
      :none -> nil
      n -> %Exogiri.Xml.Node{ref: n}
    end
  end

  @doc """
  Return the last element child of a node.
  """
  @spec last_element_child(Exogiri.Xml.Node.t()) :: nil | Exogiri.Xml.Node.t()
  def last_element_child(%__MODULE__{} = a) do
    case Exogiri.Xml.Internal.priv_node_last_element_child(a.ref) do
      :none -> nil
      n -> %Exogiri.Xml.Node{ref: n}
    end
  end

  @doc """
  Add next sibling to a node.
  """
  @spec add_next_sibling(Exogiri.Xml.Node.t(), Exogiri.Xml.Node.t()) :: :ok | :error
  def add_next_sibling(%__MODULE__{} = a, %__MODULE__{} = sibling) do
    Exogiri.Xml.Internal.priv_node_add_next_sibling(a.ref, sibling.ref)
  end

  @doc """
  Add previous sibling to a node.
  """
  @spec add_previous_sibling(Exogiri.Xml.Node.t(), Exogiri.Xml.Node.t()) :: :ok | :error
  def add_previous_sibling(%__MODULE__{} = a, %__MODULE__{} = sibling) do
    Exogiri.Xml.Internal.priv_node_add_previous_sibling(a.ref, sibling.ref)
  end
end
