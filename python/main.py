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
(call_expression
  (field_expression
    (identifier) @object
    field: (field_identifier) @method)
  arguments: (argument_list
    (qualified_identifier) @event_type
    (lambda_expression
      (compound_statement) @body))
  (#eq? @object "dispatcher")
  (#eq? @method "Subscribe")
  (#eq? @event_type "EventType::ParamChanged"))
    
""")

# Apply the query to the syntax tree
captures = query.captures(tree.root_node)

# Filter for "dispatcher"
for entry in captures:
    nodes = captures[entry] 
    if entry == "body":
        for node in nodes:
            identifier_name = SOURCE_CODE[node.start_byte:node.end_byte]
            # if identifier_name == "dispatcher":
            print(f"Found {identifier_name} at position: {node.start_point}")
