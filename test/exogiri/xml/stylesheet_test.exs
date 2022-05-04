defmodule Exogiri.Xml.StylesheetTest do
  use ExUnit.Case
  doctest Exogiri.Xml.Stylesheet

  @valid_stylesheet """
  <?xml version="1.0"?>
  <xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:template match="/element">
      <xsl:element name="output">
        <xsl:value-of select="."/>
      </xsl:element>
    </xsl:template>
  </xsl:stylesheet>
  """

  @valid_source_xml """
  <?xml version="1.0"?>
  <element>VALUE</element>
  """

  test "fails to read from an empty string" do
    {:error, :parse_failed} = Exogiri.Xml.Stylesheet.from_string("")
  end

  test "fails to read from a malformed string" do
    {:error, ["StartTag: invalid element name\n"]} = Exogiri.Xml.Stylesheet.from_string("<")
  end

  test "fails to read from a string that isn't a stylesheet" do
    {:error, ["compilation error: element element", "xsltParseStylesheetProcess : document is not a stylesheet"]} = Exogiri.Xml.Stylesheet.from_string("<element></element>")
  end

  test "loads a valid stylesheet" do
    {:ok, %Exogiri.Xml.Stylesheet{}} = Exogiri.Xml.Stylesheet.from_string(@valid_stylesheet)
  end

  test "does a transform with no parameters" do
    {:ok, doc} = Exogiri.Xml.Document.from_string(@valid_source_xml)
    {:ok, %Exogiri.Xml.Stylesheet{} = ss} = Exogiri.Xml.Stylesheet.from_string(@valid_stylesheet)
    {:ok, %Exogiri.Xml.Document{} = new_doc} = Exogiri.Xml.Stylesheet.transform(ss, doc)
    node = Exogiri.Xml.Document.root(new_doc)
    "output" = Exogiri.Xml.Node.local_name(node)
    "VALUE" = Exogiri.Xml.Node.content(node)
  end
end
