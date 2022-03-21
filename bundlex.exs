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
        sources: ["xml/exogiri.c", "xml/document.c", "xml/node.c", "xml/xpath.c", "xml/errors.c", "xml/utils.c"],
        interface: :nif
      ]
    ]
  end
end
