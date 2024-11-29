### This scripts try to check if all types are present in some code block
### I am using treesitter to parse C++ code and make some pretty specific queries
### which is a little scary, but if my code doesn't change to much I should be ok 

from pathlib import Path
import sys
import tree_sitter_cpp as ts_cpp
from tree_sitter import Language, Parser
from colorama import Fore, Back, Style
import colorama

colorama.init(autoreset=True)

CPP_LANGUAGE = Language(ts_cpp.language())

SOURCE_FILE = Path()/ "../src/NodeManager.cpp"
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

    try:
        with open(SOURCE_FILE, "r") as f:
            # print(f.read())
            SOURCE_CODE = f.read()
    except FileNotFoundError as e:
        print(f"{Fore.RED}Cound not find file {SOURCE_FILE}")
        sys.exit(0)

    print(Fore.WHITE + f"Searching for function body in {Style.BRIGHT}{SOURCE_FILE}" + Style.RESET_ALL)
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

    function_body = None
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
    str_template = f"""\
{Fore.YELLOW}\
Seaching for types:{Style.RESET_ALL}\
{[type_str for type_str in TYPE_LIST]    }"""
    
    print(str_template)

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
        '''
            tree-sitter has gone far enough, 
            finish the job using str.split()
        '''
        type_str = code.split("<const ParamChangedEvent<")[1].split(">")[0]
        types_in_code.append(type_str)

    found_missing_type = False
    for type in TYPE_LIST:
        if type not in types_in_code:
            found_missing_type = True
            print(f"{Fore.RED + Style.BRIGHT}Type{Fore.RESET} {Back.WHITE + Fore.RED}{type}{Back.RESET + Fore.RED} not found in code")
    if not found_missing_type:
        # print(Style.BRIGHT + Fore.WHITE+ Back.GREEN  + "All types found in code")
        print(Fore.GREEN + Style.BRIGHT + "All types found in code")
FUNCTION_BODY = select_function_body()
if FUNCTION_BODY is None:
    print(f"{Fore.RED}Cound not find function body in {SOURCE_FILE}")
    sys.exit(0)
else:
    print(f"{Fore.GREEN + Style.BRIGHT}Found Function Body\n")
    check_function_body(FUNCTION_BODY)