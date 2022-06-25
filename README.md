# Exogiri

Nokogiri.  But like, for Elixir.

**NOTE**: All objects are mutable, and thus process local.  If you try to pass XML nodes or XML documents between processes they will be inaccessible.

**DOUBLE NOTE**: C is **not** my first language.  I've probably got memory leaks and just plain logic errors hanging about all over.  Contributions and criticism welcome.

## Features

* XML Documents
  * [x] Read document from string
  * [ ] Read document from path
  * [x] Create new documents
  * [ ] Attribute support
    * [ ] Get/set for attributes by namespace
    * [x] Get/set without namespace
  * [ ] Element support
    * [x] Create new elements
    * [x] Remove elements
    * [x] Move elements
    * [ ] Copy elements
    * [ ] XML fragment support
    * [x] Set element content
    * [ ] Append element content
* XML Schemas
  * [x] Load schemas from string (with path awareness for includes)
  * [ ] Load schemas from path
  * [x] In-memory document validation
  * [ ] Streaming document validation (not fully supported by libxml2 - may not ever be completed)
* XSL Transforms
  * [x] Simple File transforms and parameters
  * [ ] Included/multi-file transform support
* XPath Support
  * [x] Return element lists
  * [ ] Return non-element result sets
  * [x] Namespaced queries
  * [ ] Namespaceless queries

## Installation

Using [Hex](https://hex.pm), the package can be installed by adding `exogiri` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:exogiri, "~> 0.3.0"}
  ]
end
```

The docs for the most recent version can be found at [Hex Docs](https://hexdocs.pm/exogiri).

