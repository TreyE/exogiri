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

  @doc """
  Transform a document using this stylesheet and an optional set of parameters.
  """
  @spec transform(Exogiri.Xml.Stylesheet.t(), Exogiri.Xml.Document.t(), map) ::
          {:error, :transform_failed | [binary]} | {:ok, Exogiri.Xml.Document.t()}
  def transform(%__MODULE__{ref: ss_ref}, %Exogiri.Xml.Document{ref: doc_ref}, params \\ %{}) when is_map(params) do
    clean_params = escape_params(params)
    case Exogiri.Xml.Internal.priv_stylesheet_transform(ss_ref, doc_ref, clean_params) do
      {:error, :unknown_error} -> {:error, :transform_failed}
      {:error, errs} -> {:error, format_errors(errs)}
      {:ok, a} -> {:ok, %Exogiri.Xml.Document{ref: a}}
    end
  end

  @spec escape_params(map()) :: [String.t]
  defp escape_params(params) when is_map(params) do
    Enum.reduce(
      params,
      [],
      fn({k,v}, acc) ->
        clean_k = to_string(k)
        clean_v = case String.match?(to_string(v), ~r/'/) do
          true ->
            concat_values = String.replace(
              to_string(v),
              "'",
              "', \"'\", '",
              global: true
            )
            "concat('" <> concat_values <> "')"
          _ -> to_string(v)
        end
        [clean_k,clean_v|acc]
      end
    )
  end

  @spec format_errors([any()]) :: [String.t]
  defp format_errors(errs) do
    errs
     |> Enum.reverse()
     |> Enum.map(fn x -> to_string(x) end)
  end
end
