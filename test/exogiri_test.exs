defmodule ExogiriTest do
  use ExUnit.Case
  doctest Exogiri

  test "greets the world" do
    assert Exogiri.hello() == :world
  end
end
