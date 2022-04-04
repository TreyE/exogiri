defmodule Exogiri.MixProject do
  use Mix.Project

  @version "0.2.2"

  def project do
    [
      app: :exogiri,
      version: @version,
      elixir: "~> 1.9",
      description: "Nokogiri.  But like, for Elixir.",
      start_permanent: Mix.env() == :prod,
      elixirc_paths: elixirc_paths(Mix.env),
      deps: deps(),
      docs: docs(),
      dialyzer: [
        plt_file: {:no_warn, "priv/plts/dialyzer.plt"}
      ],
      package: package(),
      compilers: [:bundlex] ++ Mix.compilers
    ]
  end

  def application do
    [
      extra_applications: [:logger]
    ]
  end

  defp docs do
    [
      main: "readme",
      source_ref: "#{@version}",
      source_url: "https://github.com/TreyE/exogiri",
      extras: ["README.md"]
    ]
  end

  defp package() do
    [
      licenses: ["MIT"],
      maintainers: ["Trey Evans"],
      source_url: "https://github.com/TreyE/exogiri",
      links: %{"GitHub" => "https://github.com/TreyE/exogiri"}
    ]
  end

  defp deps do
    [
      {:bundlex, "~> 0.5.1"},
      {:dialyxir, "~> 1.0", only: [:dev], runtime: false},
      {:ex_doc, "~> 0.22", only: :dev, runtime: false}
    ]
  end

  defp elixirc_paths(:test), do: ["lib","test/support"]
  defp elixirc_paths(_), do: ["lib"]
end
