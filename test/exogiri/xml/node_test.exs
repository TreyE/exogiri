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

  test "can remove a node" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<hello xmlns=\"urn:something\"></hello>")
    a = Exogiri.Xml.Document.root(a)
    Exogiri.Xml.Node.unlink(a)
  end

  test "can find, remove, and then re-parent nodes" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1/><child2/></root>")
    root = Exogiri.Xml.Document.root(a)
    {:ok, [node]} = Exogiri.Xml.Node.xpath(root,"//ns:child2",%{"ns" => "urn:something"})
    {:ok, [new_parent]} = Exogiri.Xml.Node.xpath(root,"//ns:child1",%{"ns" => "urn:something"})
    Exogiri.Xml.Node.unlink(node)
    :ok = Exogiri.Xml.Node.add_child(new_parent, node)
  end

  test "can get node content" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1>THE CONTENT</child1></root>")
    root = Exogiri.Xml.Document.root(a)
    {:ok, [node]} = Exogiri.Xml.Node.xpath(root,"//ns:child1",%{"ns" => "urn:something"})
    "THE CONTENT" = Exogiri.Xml.Node.content(node)
  end

  test "can get node attribute value" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1 attribute_name=\"THE ATTRIBUTE VALUE\">THE CONTENT</child1></root>")
    root = Exogiri.Xml.Document.root(a)
    {:ok, [node]} = Exogiri.Xml.Node.xpath(root,"//ns:child1",%{"ns" => "urn:something"})
    nil = Exogiri.Xml.Node.attribute_value(node, "bogus_attribute_name")
    "THE ATTRIBUTE VALUE" = Exogiri.Xml.Node.attribute_value(node, "attribute_name")
  end
end
