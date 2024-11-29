from pathlib import Path
import sys
import tree_sitter_cpp as ts_cpp
from tree_sitter import Language, Parser
CPP_LANGUAGE = Language(ts_cpp.language())

SOURCE_FILE = Path()/ "../example/src/example.cpp"
SOURCE_CODE = ""
with open(SOURCE_FILE, "r") as f:
    # print(f.read())
    SOURCE_CODE = f.read()

# sys.exit(0)

parser = Parser(CPP_LANGUAGE)

tree = parser.parse(bytes(SOURCE_CODE, "utf-8"))

# print(tree)

query = CPP_LANGUAGE.query("""

      ((identifier) @name
        (field_identifier) @function_call
                           (#eq? @name "dispatcher")
      )
    
""")

# Apply the query to the syntax tree
captures = query.captures(tree.root_node)

# Filter for "dispatcher"
for entry in captures:
    nodes = captures[entry] 
    for node in nodes:
        identifier_name = SOURCE_CODE[node.start_byte:node.end_byte]
        if identifier_name == "dispatcher":
            print(f"Found dispatcher at position: {node.start_point}")
