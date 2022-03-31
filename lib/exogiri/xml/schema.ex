defmodule Exogiri.Xml.Schema do
  @moduledoc """
  Encapsulates an XML schema.
  """

  defstruct [:ref]

  @typedoc "The Schema type."
  @opaque t :: %__MODULE__{ref: reference()}

  @doc """
  Read a schema from a string.

  As you don't have a path, this doesn't handle more complex cases like
  included or imported schemas, if either is local.
  """
  @spec from_string(String.t) :: {:error, :unknown_error | [String.t]} | {:ok, t}
  def from_string(string) do
    case Exogiri.Xml.Internal.priv_schema_from_string(string) do
      {:error, :unknown_error} -> {:error, :unknown_error}
      {:error, errs} -> {:error, format_errors(errs)}
      {:ok, a} -> {:ok, %__MODULE__{ref: a}}
    end
  end

  @doc """
  Read a schema from a string with a path.

  This version handles local includes and imports correctly.  The path argument
  is the location of the actual schema you are parsing as a string, NOT its
  directory.  The schema isn't read from the path, so it doesn't have to be
  real - the path is used to establish the base location for any local schema
  references.
  """
  @spec from_string(String.t, String.t) :: {:error, :unknown_error | [String.t]} | {:ok, t}
  def from_string(string, path) do
    case Exogiri.Xml.Internal.priv_schema_from_string_with_path(string, path) do
      {:error, :unknown_error} -> {:error, :unknown_error}
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
