from pathlib import Path
import sys
import tree_sitter_cpp as ts_cpp
from tree_sitter import Language, Parser
CPP_LANGUAGE = Language(ts_cpp.language())

SOURCE_FILE = Path()/ "../example/src/example.cpp"
SOURCE_CODE = ""
TYPE_LIST = [
    "int",
    "float",
    "bool",
    "std::string",
    "std::wstring",
    "glm::vec2",
    "glm::vec3",
]
def select_function_body() ->str:

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

    function_body = ""
    # Filter for "dispatcher"
    for entry in captures:
        nodes = captures[entry] 
        if entry == "body":
            for node in nodes:
                function_body = SOURCE_CODE[node.start_byte:node.end_byte]
                break
                # if identifier_name == "dispatcher":
                # print(f"Found {identifier_name} at position: {node.start_point}")

    return function_body

def check_function_body(source_code : str):
    parser = Parser(CPP_LANGUAGE)
    tree = parser.parse(bytes(source_code, "utf-8"))

    query = CPP_LANGUAGE.query("""
        ((identifier) @object (
        (template_argument_list) @template_args
        (#eq? @object "dynamic_cast")
        ))
    """)
    # Apply the query to the syntax tree
    captures = query.captures(tree.root_node)

    code_chunks = []
    for entry in captures:
        nodes = captures[entry] 
        if entry == "template_args":
            for node in nodes:
                code = source_code[node.start_byte:node.end_byte]
                code_chunks.append(code)

    types_in_code : list[str] = []
    for code in code_chunks:
        type_str = code.split("<const ParamChangedEvent<")[1].split(">")[0]
        types_in_code.append(type_str)

    for type in TYPE_LIST:
        if type not in types_in_code:
            print(f"Type {type} not found in code")
FUNCTION_BODY = select_function_body()
check_function_body(FUNCTION_BODY)