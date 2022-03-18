defmodule Exogiri.Xml.Document do
  defstruct [:ref]

  @opaque t :: %__MODULE__{}

  @spec from_string(String.t) :: {:error, :parse_failed | [String.t]} | {:ok, t}
  def from_string(string) do
    case Exogiri.Xml.Internal.priv_from_string(string) do
      {:error, :parse_failed} -> {:error, :parse_failed}
      {:error, errs} -> {:error, format_errors(errs)}
      {:ok, a} -> {:ok, %__MODULE__{ref: a}}
    end
  end

  @spec to_xml(Exogiri.Xml.Document.t()) :: binary()
  def to_xml(%__MODULE__{ref: ref}) do
    Exogiri.Xml.Internal.priv_to_xml(ref)
  end

  def root(%__MODULE__{ref: ref}) do
    %Exogiri.Xml.Node{
      ref: Exogiri.Xml.Internal.priv_doc_get_root(ref)
    }
  end

  defp format_errors(errs) do
    errs
     |> Enum.reverse()
     |> Enum.map(fn x -> to_string(x) end)
  end
end
