# ini

> [!WARNING]
> Still in development.

Utilities for reading and writing ini files.

This parser doesn't have the notion of sections, only key/value pairs.

Current grammar definition.

```txt
<ini-file>      ::= <line>*
<line>          ::= <key-value> | <empty-line>

<key-value>     ::= <key> "=" <value> <newline>
<key>           ::= <string>
<value>         ::= <quoted-value> | <string>
<quoted-value>  ::= "\"" <string> "\""

<empty-line>    ::= <newline>

<string>        ::= <character>+
<character>     ::= <letter> | <digit> | <symbol> | <whitespace-except-newline>
<letter>        ::= "A" | "B" | "C" | ... | "Z" | "a" | "b" | "c" | ... | "z"
<digit>         ::= "0" | "1" | "2" | ... | "9"
<symbol>        ::= "!" | "@" | "#" | ... | "~"  ; Any printable non-whitespace character

<newline>       ::= "\n" | "\r\n"  ; Support for different OS line endings
<whitespace-except-newline> ::= " " | "\t"
```

# license

unlicense
