defmodule Exogiri.Xml.NodeTest do
  use ExUnit.Case
  doctest Exogiri.Xml.Node

  test "reads a basic, correct document, and knows the root node local name and empty namespace" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<hello></hello>")
    a = Exogiri.Xml.Document.root(a)
    "hello" = Exogiri.Xml.Node.local_name(a)
    nil = Exogiri.Xml.Node.namespace(a)
  end

  test "reads a basic, correct document, and knows the root node local name and default namespace" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<hello xmlns=\"urn:something\"></hello>")
    a = Exogiri.Xml.Document.root(a)
    "hello" = Exogiri.Xml.Node.local_name(a)
    {"", "urn:something"} = Exogiri.Xml.Node.namespace(a)
  end
end
