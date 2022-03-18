defmodule Exogiri.Xml.XpathTest do
  use ExUnit.Case

  test "reads a basic, correct document, and knows the root node local name and default namespace" do
    {:ok, a} = Exogiri.Xml.Document.from_string("<hello xmlns=\"urn:something\"><goodbye/></hello>")
    a = Exogiri.Xml.Document.root(a)
    b = Exogiri.Xml.Node.xpath(a,"//ns:goodbye",%{"ns" => "urn:something"})
    IO.inspect(b)
  end
end
