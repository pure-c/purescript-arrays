{ name = "arrays"
, dependencies =
  [ "bifunctors"
  , "control"
  , "foldable-traversable"
  , "maybe"
  , "nonempty"
  , "partial"
  , "prelude"
  , "st"
  , "tailrec"
  , "tuples"
  , "unfoldable"
  , "unsafe-coerce"
  , "assert"
  , "console"
  , "const"
  ]
, packages = ../pure-c/package-sets/packages.dhall
, sources = [ "src/**/*.purs", "test/**/*.purs" ]
}
