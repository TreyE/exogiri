defmodule Exogiri.Xml.Schema do
  @moduledoc """
  Encapsulates an XML schema.
  """

  defstruct [:ref]

  @typedoc "The Schema type."
  @opaque t :: %__MODULE__{ref: reference()}

  @doc """
  Read a schema from a string.
  """
  @spec from_string(String.t) :: {:error, :parse_failed | [String.t]} | {:ok, t}
  def from_string(string) do
    case Exogiri.Xml.Internal.priv_schema_from_string(string) do
      {:error, :parse_failed} -> {:error, :parse_failed}
      {:error, errs} -> {:error, format_errors(errs)}
      {:ok, a} -> {:ok, %__MODULE__{ref: a}}
    end
  end

  @doc """
  Read a schema from a with a base path.
  """
  @spec from_string(String.t, String.t) :: {:error, :parse_failed | [String.t]} | {:ok, t}
  def from_string(string, path) do
    case Exogiri.Xml.Internal.priv_schema_from_string_with_path(string, path) do
      {:error, :parse_failed} -> {:error, :parse_failed}
      {:error, errs} -> {:error, format_errors(errs)}
      {:ok, a} -> {:ok, %__MODULE__{ref: a}}
    end
  end

  defp format_errors(errs) do
    errs
     |> Enum.reverse()
     |> Enum.map(fn x -> to_string(x) end)
  end
end
