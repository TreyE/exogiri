defmodule Exogiri.BundlexProject do
  use Bundlex.Project

  def project() do
    [
      natives: natives(Bundlex.platform)
    ]
  end

  def natives(_) do
    [
      exogiri_xml_internal: [
        sources: ["xml/exogiri.c", "xml/document.c", "xml/node.c", "xml/xpath.c", "xml/errors.c", "xml/utils.c", "xml/schema.c", "xml/namespace.c", "xml/unlinked_node.c", "xml/stylesheet.c"],
        pkg_configs: ["libxml-2.0", "libxslt"],
        libs: ["xml2", "xslt"],
        interface: :nif
      ]
    ]
  end

end
