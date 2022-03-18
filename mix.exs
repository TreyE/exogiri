defmodule Exogiri.MixProject do
  use Mix.Project

  @version "0.1.0"

  def project do
    [
      app: :exogiri,
      version: @version,
      elixir: "~> 1.9",
      description: "Nokogiri.  But like, for Elixir.",
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      docs: docs(),
      package: package(),
      compilers: [:bundlex] ++ Mix.compilers
    ]
  end

  # Run "mix help compile.app" to learn about applications.
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

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:bundlex, "~> 0.5.1"}
      # {:dep_from_hexpm, "~> 0.3.0"},
      # {:dep_from_git, git: "https://github.com/elixir-lang/my_dep.git", tag: "0.1.0"}
    ]
  end
end
