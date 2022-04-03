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

  test "creates a new doc with no ns" do
    {%Exogiri.Xml.Document{}, %Exogiri.Xml.Node{}} = Exogiri.Xml.Document.new_without_ns("root")
  end

  test "creates a new doc with a blank namespace" do
    {%Exogiri.Xml.Document{} = doc, %Exogiri.Xml.Node{}} = Exogiri.Xml.Document.new_with_ns("root", nil, "urn:some_namespace")
    root = Exogiri.Xml.Document.root(doc)
    {nil, "urn:some_namespace"} = Exogiri.Xml.Node.namespace(root)
  end

  test "creates a new doc with a namespace" do
    {%Exogiri.Xml.Document{} = doc, %Exogiri.Xml.Node{}} = Exogiri.Xml.Document.new_with_ns("root", "ns1", "urn:some_namespace")
    root = Exogiri.Xml.Document.root(doc)
    {"ns1", "urn:some_namespace"} = Exogiri.Xml.Node.namespace(root)
  end

  test "can build a basic document with no namespaces" do
    {%Exogiri.Xml.Document{} = doc, %Exogiri.Xml.Node{} = root} = Exogiri.Xml.Document.new_without_ns("root")
    node = Exogiri.Xml.Node.new_no_ns(doc, "child1")
    :ok = Exogiri.Xml.Node.add_child(root, node)
  end

  test "can build a basic document with blank namespaces" do
    {%Exogiri.Xml.Document{} = doc, %Exogiri.Xml.Node{}} = Exogiri.Xml.Document.new_with_ns("root", nil, "urn:some_namespace")
    root = Exogiri.Xml.Document.root(doc)
    child = Exogiri.Xml.Node.new_with_ns(doc, "child1", nil, "urn:some_namespace")
    :ok = Exogiri.Xml.Node.add_child(root, child)
    {nil, "urn:some_namespace"} = Exogiri.Xml.Node.namespace(child)
  end

  test "can build a basic document with multiple namespaces" do
    {%Exogiri.Xml.Document{} = doc, %Exogiri.Xml.Node{}} = Exogiri.Xml.Document.new_with_ns("root", "ns1", "urn:some_namespace")
    root = Exogiri.Xml.Document.root(doc)
    child1 = Exogiri.Xml.Node.new_with_ns(doc, "child1", nil, "urn:some_namespace")
    :ok = Exogiri.Xml.Node.add_child(root, child1)
    child2 = Exogiri.Xml.Node.new_with_ns(doc, "child2", "ns1", "urn:some_namespace")
    :ok = Exogiri.Xml.Node.add_next_sibling(child1, child2)
  end
end
