# Parser

### Dependencies

This parser depends on the boost library. To install in Ubuntu:
```
sudo apt-get install libboost-all-dev
```
It uses [Boost's Spirit X3](https://www.boost.org/doc/libs/1_69_0/libs/spirit/doc/x3/html/index.html) as a parsing library, so C++14 is required.

The following syntax in EBNF accepted by the parser:
```
SELECT           := [Ss][Ee][Ll][Ee][Cc][Tt]
WHERE            := [Ww][Hh][Ee][Rr][Ee]
MATCH            := [Mm][Aa][Tt][Cc][Hh]
AND              := [Aa][Nn][Dd]
OR               := [Oo][Rr]
TRUE             := [Tt][Rr][Uu][Ee]
FALSE            := [Ff][Aa][Ll][Ss][Ee]

EQUAL            := ==
LESS_OR_EQUAL    := <=
MORE_OR_EQUAL    := >=
NOT_EQUAL        := !=
LESS             := <
MORE             := >

VAR              := \?\w+
KEY              := \w+
FUNC_NAME        := \w+
LABEL            := :\w+

STRING           := (".*")|('.*')
NUMBER           := -?\d+(\.\d+)?
BOOLEAN          := TRUE|FALSE
VALUE            := STRING|NUMBER|BOOLEAN

CONECTOR         := AND|OR
COMPARATOR       := EQUAL|LESS_OR_EQUAL|MORE_OR_EQUAL|NOT_EQUAL|LESS|MORE

PROPERTY         := KEY\s*:\s*VALUE
PROPERTIES       := {\s*PROPERTY(\s*,\s*PROPERTY)*\s*}

NODE             := \(\s*VAR?(\s+LABEL)*(\s+PROPERTIES)?*\s*\)
INNER_EDGE       := \[\s*VAR?(\s+LABEL)*(\s+PROPERTIES)?*\s*\]
EDGE             := (-INNER_EDGE?->)|(<-INNER_EDGE?-)

LINEAR_PATTERN   := NODE(EDGE NODE)*
GRAPH_PATTERN    := LINEAR_PATTERN(\s*,\s*LINEAR_PATTERN)*
MATCH_STATEMENT  := MATCH\s+GRAPH_PATTERN

ELEMENT          := (VAR\.KEY)|(FUNC\s*\(\s*VAR\.KEY\s*\))
SELECT_STATEMENT := SELECT(\s+\*|(\s+ELEMENT)+)

WHERE_STATEMENT  := WHERE\s+FORMULA
FORMULA          := CONDITION(\s+CONNECTOR\s+CONDITION)
CONDITION        := (\(FORMULA\))|STATEMENT
STATEMENT        := (ELEMENT\s*COMPARATOR\s*(ELEMENT|VALUE))|

QUERY_ROOT       := SELECT_STATEMENT\s+MATCH_STATEMENT(\s+WHERE_STATEMENT)?
```
