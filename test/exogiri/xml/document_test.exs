defmodule Exogiri.Xml.DocumentTest do
  use ExUnit.Case
  doctest Exogiri.Xml.Document

  test "fails to read from an empty string" do
    {:error, :parse_failed} = Exogiri.Xml.Document.from_string("")
  end

  test "fails to read from an erroneous string" do
    {:error, ["Start tag expected, '<' not found\n"]} = Exogiri.Xml.Document.from_string(">")
  end

  test "fails to read from another erroneous string" do
    {:error, ["Start tag expected, '<' not found\n"]} = Exogiri.Xml.Document.from_string("hello></hello>")
  end

  test "reads a basic, correct document" do
    {:ok, %Exogiri.Xml.Document{}} = Exogiri.Xml.Document.from_string("<hello></hello>")
  end

  test "reads and writes a basic, correct document" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<hello></hello>")
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<hello/>\n" = Exogiri.Xml.Document.to_xml(a)
  end

  test "reads a basic, correct document, and knows the root" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<hello></hello>")
    %Exogiri.Xml.Node{} = Exogiri.Xml.Document.root(a)
  end
end
