# Exogiri

Nokogiri.  But like, for Elixir.

**NOTE**: All objects are mutable, and thus process local.  If you try to pass XML nodes or XML documents between processes they will be inaccessible.

**DOUBLE NOTE**: C is **not** my first language.  I've probably got memory leaks and just plain logic errors hanging about all over.  Contributions and criticism welcome.

## Installation

Using [Hex](https://hex.pm), the package can be installed by adding `exogiri` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:exogiri, "~> 0.2.3"}
  ]
end
```

The docs for the most recent version can be found at [Hex Docs](https://hexdocs.pm/exogiri).

