defmodule Exogiri.BundlexProject do
  use Bundlex.Project

  def project() do
    [
      natives: natives(Bundlex.platform)
    ]
  end

  def natives(:linux) do
    [
      exogiri_xml_internal: [
        sources: ["xml/exogiri.c", "xml/document.c", "xml/node.c", "xml/xpath.c", "xml/errors.c", "xml/utils.c", "xml/schema.c"],
        includes: ["/include", "/usr/include", "/usr/include/libxml2"],
        interface: :nif
      ]
    ]
  end

  def natives(_) do
    [
      exogiri_xml_internal: [
        sources: ["xml/exogiri.c", "xml/document.c", "xml/node.c", "xml/xpath.c", "xml/errors.c", "xml/utils.c", "xml/schema.c"],
        interface: :nif
      ]
    ]
  end
end
