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
    root = Exogiri.Xml.Document.root(a)
    "hello" = Exogiri.Xml.Node.local_name(root)
    {"", "urn:something"} = Exogiri.Xml.Node.namespace(root)
  end

  test "can get namespaces for a node" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root:hello xmlns:root=\"urn:something\"><child xmlns=\"urn:something_else\"/> </root:hello>")
    root = Exogiri.Xml.Document.root(a)
    {:ok, [node]} = Exogiri.Xml.Node.xpath(root,"//ns:child",%{"ns" => "urn:something_else"})
    [{"root", "urn:something"}, {"", "urn:something_else"}] = Exogiri.Xml.Node.namespaces(node)
  end

  test "can remove a node" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<hello xmlns=\"urn:something\"></hello>")
    root = Exogiri.Xml.Document.root(a)
    Exogiri.Xml.Node.unlink(root)
  end

  test "can find, remove, and then re-parent nodes" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1/><child2/></root>")
    root = Exogiri.Xml.Document.root(a)
    {:ok, [node]} = Exogiri.Xml.Node.xpath(root,"//ns:child2",%{"ns" => "urn:something"})
    {:ok, [new_parent]} = Exogiri.Xml.Node.xpath(root,"//ns:child1",%{"ns" => "urn:something"})
    :ok = Exogiri.Xml.Node.unlink(node)
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

  test "can set node attribute value" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1>THE CONTENT</child1></root>")
    root = Exogiri.Xml.Document.root(a)
    {:ok, [node]} = Exogiri.Xml.Node.xpath(root,"//ns:child1",%{"ns" => "urn:something"})
    :ok = Exogiri.Xml.Node.set_attribute_value(node, "attribute_name", "THE ATTRIBUTE VALUE")
    "THE ATTRIBUTE VALUE" = Exogiri.Xml.Node.attribute_value(node, "attribute_name")
  end

  test "can set node content" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1>THE CONTENT</child1></root>")
    root = Exogiri.Xml.Document.root(a)
    {:ok, [node]} = Exogiri.Xml.Node.xpath(root,"//ns:child1",%{"ns" => "urn:something"})
    :ok = Exogiri.Xml.Node.set_content(node, "THE NEW CONTENT")
    "THE NEW CONTENT" = Exogiri.Xml.Node.content(node)
  end
end
