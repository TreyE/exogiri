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
    {nil, "urn:something"} = Exogiri.Xml.Node.namespace(root)
  end

  # TODO: Fix the ordering of the results so the test will always pass.
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

  test "can find, remove, and then re-parent nodes, and then serialize canonically" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1 xmlns=\"urn:something_else\"/><child2/></root>")
    root = Exogiri.Xml.Document.root(a)
    {:ok, [node]} = Exogiri.Xml.Node.xpath(root,"//ns:child2",%{"ns" => "urn:something"})
    {:ok, [new_parent]} = Exogiri.Xml.Node.xpath(root,"//ns:child1",%{"ns" => "urn:something_else"})
    :ok = Exogiri.Xml.Node.unlink(node)
    :ok = Exogiri.Xml.Node.add_child(new_parent, node)
    "<root xmlns=\"urn:something\"><child1 xmlns=\"urn:something_else\"><default:child2 xmlns:default=\"urn:something\"></default:child2></child1></root>" = Exogiri.Xml.Document.canonicalize(a)
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

  test "can get node children" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1 xmlns=\"urn:something_else\"/><child2/></root>")
    root = Exogiri.Xml.Document.root(a)
    [child1, child2] = Exogiri.Xml.Node.children(root)
    "child1" = Exogiri.Xml.Node.local_name(child1)
    "child2" = Exogiri.Xml.Node.local_name(child2)
  end

  test "can get node parent" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1 xmlns=\"urn:something_else\"><child2/></child1></root>")
    root = Exogiri.Xml.Document.root(a)
    {:ok, [node]} = Exogiri.Xml.Node.xpath(root,"//ns:child2",%{"ns" => "urn:something_else"})
    parent = Exogiri.Xml.Node.parent(node)
    "child1" = Exogiri.Xml.Node.local_name(parent)
  end

  test "can get previous sibling" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1 xmlns=\"urn:something_else\"/><child2/></root>")
    root = Exogiri.Xml.Document.root(a)
    {:ok, [node]} = Exogiri.Xml.Node.xpath(root,"//ns:child2",%{"ns" => "urn:something"})
    sibling = Exogiri.Xml.Node.previous_element_sibling(node)
    "child1" = Exogiri.Xml.Node.local_name(sibling)
  end

  test "can get next sibling" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1 xmlns=\"urn:something_else\"/><child2/></root>")
    root = Exogiri.Xml.Document.root(a)
    {:ok, [node]} = Exogiri.Xml.Node.xpath(root,"//ns:child1",%{"ns" => "urn:something_else"})
    sibling = Exogiri.Xml.Node.next_element_sibling(node)
    "child2" = Exogiri.Xml.Node.local_name(sibling)
  end

  test "can get a first child" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1 xmlns=\"urn:something_else\"/><child2/></root>")
    root = Exogiri.Xml.Document.root(a)
    child = Exogiri.Xml.Node.first_element_child(root)
    "child1" = Exogiri.Xml.Node.local_name(child)
  end

  test "can get a last child" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1 xmlns=\"urn:something_else\"/><child2/></root>")
    root = Exogiri.Xml.Document.root(a)
    child = Exogiri.Xml.Node.last_element_child(root)
    "child2" = Exogiri.Xml.Node.local_name(child)
  end

  test "can add a next sibling" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1 xmlns=\"urn:something_else\"/><child2/></root>")
    root = Exogiri.Xml.Document.root(a)
    child1 = Exogiri.Xml.Node.first_element_child(root)
    child2 = Exogiri.Xml.Node.last_element_child(root)
    :ok = Exogiri.Xml.Node.add_next_sibling(child2, child1)
    new_sibling = Exogiri.Xml.Node.next_element_sibling(child2)
    "child1" = Exogiri.Xml.Node.local_name(new_sibling)
  end

  test "can add a previous sibling" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:something\"><child1 xmlns=\"urn:something_else\"/><child2/></root>")
    root = Exogiri.Xml.Document.root(a)
    child1 = Exogiri.Xml.Node.first_element_child(root)
    child2 = Exogiri.Xml.Node.last_element_child(root)
    :ok = Exogiri.Xml.Node.add_previous_sibling(child1, child2)
    new_sibling = Exogiri.Xml.Node.previous_element_sibling(child1)
    "child2" = Exogiri.Xml.Node.local_name(new_sibling)
  end
end
