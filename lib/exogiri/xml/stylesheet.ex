defmodule Exogiri.Xml.Stylesheet do
  @moduledoc """
  Encapsulates an XML stylesheet.
  """

  defstruct [:ref]

  @typedoc "The Stylesheet type."
  @type t :: %__MODULE__{ref: reference()}

  @doc """
  Create a stylesheet from a string.
  """
  @spec from_string(String.t) :: {:error, :unknown_error | [String.t]} | {:ok, t}
  def from_string(string) when is_binary(string) do
    case Exogiri.Xml.Internal.priv_stylesheet_from_string(string) do
      {:error, :unknown_error} -> {:error, :parse_failed}
      {:error, errs} -> {:error, format_errors(errs)}
      {:ok, a} -> {:ok, %__MODULE__{ref: a}}
    end
  end

  @spec format_errors([any()]) :: [String.t]
  defp format_errors(errs) do
    errs
     |> Enum.reverse()
     |> Enum.map(fn x -> to_string(x) end)
  end
end
