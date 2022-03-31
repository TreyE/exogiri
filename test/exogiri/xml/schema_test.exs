defmodule Exogiri.Xml.SchemaTest do
  use ExUnit.Case
  doctest Exogiri.Xml.Schema

  @basic_schema """
  <?xml version="1.0" encoding="UTF-8" ?>
  <xsd:schema xmlns:xsd="http://www.w3.org/2001/XMLSchema">
    <xsd:element name="root" type="xsd:string"/>
  </xsd:schema>
  """

  @bogus_schema """
  <?xml version="1.0" encoding="UTF-8" ?>
  <xsd:schema xmlns:xsd="http://www.w3.org/2001/XMLSchema">
  </xsd:schema
  """

  @bogus_inclusion_schema """
  <?xml version="1.0" encoding="UTF-8" ?>
  <xsd:schema xmlns:xsd="http://www.w3.org/2001/XMLSchema" targetNamespace="urn:some_namespace">
    <xsd:include schemaLocation="./included.xsd"/>
  </xsd:schema>
  """

  @simple_namespaced_schema """
  <?xml version="1.0" encoding="UTF-8" ?>
  <xsd:schema xmlns:xsd="http://www.w3.org/2001/XMLSchema" targetNamespace="urn:some_namespace">
    <xsd:element name="root" type="xsd:string"/>
  </xsd:schema>
  """

  test "can load a schema from a string" do
    {:ok, %Exogiri.Xml.Schema{}} = Exogiri.Xml.Schema.from_string(@basic_schema)
  end

  test "fails loading a bogus schema" do
    {:error, _list} = Exogiri.Xml.Schema.from_string(@bogus_schema)
  end

  test "fails loading a bogus inclusion schema" do
    {:error, ["failed to load external entity \"included.xsd\"\n", "Element '{http://www.w3.org/2001/XMLSchema}include': Failed to load the document 'included.xsd' for inclusion.\n"]} = Exogiri.Xml.Schema.from_string(@bogus_inclusion_schema)
  end

  test "loads an inclusion schema with path" do
    path = Path.join(
             to_string(__DIR__),
             "../../support/xsd/base.xsd"
           )
    {:ok, %Exogiri.Xml.Schema{}} = Exogiri.Xml.Schema.from_string(@bogus_inclusion_schema, path)
  end

  test "validates a simple schema against an invalid document - no namespaces" do
    {:ok, basic_schema} = Exogiri.Xml.Schema.from_string(@basic_schema)
    {:ok, doc} = Exogiri.Xml.Document.from_string("<wrong/>")
    {:error, ["Element 'wrong': No matching global declaration available for the validation root.\n"]} = Exogiri.Xml.Schema.validate_document(basic_schema, doc)
  end

  test "validates a simple schema against a valid document - no namespaces" do
    {:ok, basic_schema} = Exogiri.Xml.Schema.from_string(@basic_schema)
    {:ok, doc} = Exogiri.Xml.Document.from_string("<root/>")
    :ok = Exogiri.Xml.Schema.validate_document(basic_schema, doc)
  end

  test "validates a simple schema against an invalid document - namespaced" do
    {:ok, basic_schema} = Exogiri.Xml.Schema.from_string(@simple_namespaced_schema)
    {:ok, doc} = Exogiri.Xml.Document.from_string("<root/>")
    {:error, ["Element 'root': No matching global declaration available for the validation root.\n"]} = Exogiri.Xml.Schema.validate_document(basic_schema, doc)
  end

  test "validates a simple schema against a valid document - namespaced" do
    {:ok, basic_schema} = Exogiri.Xml.Schema.from_string(@simple_namespaced_schema)
    {:ok, doc} = Exogiri.Xml.Document.from_string("<root xmlns=\"urn:some_namespace\"/>")
    :ok = Exogiri.Xml.Schema.validate_document(basic_schema, doc)
  end
end
