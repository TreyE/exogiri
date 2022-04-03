defmodule Exogiri.Xml.Document do
  @moduledoc """
  Encapsulates an XML document.
  """

  defstruct [:ref]

  @typedoc "The Document type."
  @type t :: %__MODULE__{ref: reference()}

  @doc """
  New document with no namespaces and a named root element.
  """
  def new_without_ns(root_name) when is_binary(root_name) do
    {d_ref, n_ref} = Exogiri.Xml.Internal.priv_doc_new_root_no_ns(root_name)
    {%__MODULE__{ref: d_ref}, %Exogiri.Xml.Node{ref: n_ref}}
  end

  @doc """
  New document with a namespace and a named root element.
  """
  def new_with_ns(root_name, nil, root_href) when
    is_binary(root_name) and
    is_binary(root_href) do
    {d_ref, n_ref} = Exogiri.Xml.Internal.priv_doc_new_root_with_ns(root_name, "", root_href)
    {%__MODULE__{ref: d_ref}, %Exogiri.Xml.Node{ref: n_ref}}
  end
  def new_with_ns(root_name, root_abbrev, root_href) when
    is_binary(root_name) and
    is_binary(root_abbrev) and
    is_binary(root_href) do
    {d_ref, n_ref} = Exogiri.Xml.Internal.priv_doc_new_root_with_ns(root_name, root_abbrev, root_href)
    {%__MODULE__{ref: d_ref}, %Exogiri.Xml.Node{ref: n_ref}}
  end

  @doc """
  Build a document from a string.
  """
  @spec from_string(String.t) :: {:error, :parse_failed | [String.t]} | {:ok, t}
  def from_string(string) when is_binary(string) do
    case Exogiri.Xml.Internal.priv_from_string(string) do
      {:error, :parse_failed} -> {:error, :parse_failed}
      {:error, errs} -> {:error, format_errors(errs)}
      {:ok, a} -> {:ok, %__MODULE__{ref: a}}
    end
  end

  @doc """
  Use if you are of the opinion your Document is better serialized.

  I know it doesn't support most of same options the cool kids are used to yet.
  """
  @spec to_xml(Exogiri.Xml.Document.t()) :: binary()
  def to_xml(%__MODULE__{ref: ref}) do
    Exogiri.Xml.Internal.priv_to_xml(ref)
  end

  @doc """
  Return the document root.
  """
  @spec root(Exogiri.Xml.Document.t()) :: Exogiri.Xml.Node.t()
  def root(%__MODULE__{ref: ref}) do
    %Exogiri.Xml.Node{
      ref: Exogiri.Xml.Internal.priv_doc_get_root(ref)
    }
  end

  def canonicalize(%__MODULE__{ref: ref}) do
    Exogiri.Xml.Internal.priv_doc_canonicalize(ref)
  end

  defp format_errors(errs) do
    errs
     |> Enum.reverse()
     |> Enum.map(fn x -> to_string(x) end)
  end
end
