defmodule Exogiri do
  @moduledoc """
  Top level for Exogiri functionality.

  Most of your helpers for easy usage of this library can be found here.
  """

  @doc """
  Read a string as an XML document.
  """
  @spec xml(String.t) :: {:error, :parse_failed | [binary]} | {:ok, Exogiri.Xml.Document.t()}
  def xml(string) do
    Exogiri.Xml.Document.from_string(string)
  end
end
