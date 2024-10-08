import os
from pathlib import Path
import sys
import shutil
print("Hey there !!!!!!!!!!!!!!!!!!!!")
print("Current Directory is ::" + os.getcwd())

config_type : str = None
if len(sys.argv) < 2:
    print("No file path passed")
    sys.exit(0)
if sys.argv[1] == "Release" : 
    print("Script detects Release build")
    config_type = "Release"
elif sys.argv[1] == "Debug" : 
    print("Script detects Debug build")
    config_type = "Debug"
else :
    print("Script detects unknown build")
    sys.exit(0)

if config_type != None:
    print("Copying ini file for " + config_type)
    ini_file = Path(os.path.join(os.getcwd(), "imgui.ini"))
    existing_file = Path() / os.getcwd()/ "build/example" / config_type / "imgui.ini"
    if ini_file.is_file() and not existing_file.is_file():
        print("Copying ini file")
        shutil.copyfile(Path() / os.getcwd() / "imgui.ini", "build/example/" + config_type + "/imgui.ini")
        # os.system("copy imgui.ini build/example/" + config_type + "/imgui.ini")
    else:
        print("No ini file found")