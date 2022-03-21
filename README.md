# Exogiri

Nokogiri.  But like, for Elixir.

**NOTE**: All objects are mutable, and thus process local.  If you try to pass XML nodes or XML documents between processes they will be inaccessible.

## Installation

Using [Hex](https://hex.pm), the package can be installed by adding `exogiri` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:exogiri, "~> 0.1.0"}
  ]
end
```

The docs for the most recent version can be found at [Hex Docs](https://hexdocs.pm/exogiri).

