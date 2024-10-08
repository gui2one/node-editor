import os
import sys
import shutil

# print("Current Directory is ::" + os.getcwd())

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


ini_backup_name = "imgui.ini.backup"
if config_type != None:
    print("Copying ini file for " + config_type)
    ini_file = os.path.join(os.getcwd(), ini_backup_name)
    existing_file = os.path.join(os.getcwd(), "build/example", config_type, "imgui.ini")

    if os.path.exists(ini_file):
        if os.path.exists(existing_file):
            print("imgui.ini already exists. Skipping copy")
        else:
            print("Copying ini file")
            shutil.copyfile(ini_file, existing_file)
    else:
        print("No ini file found")
        # try and copy an existing one
        if os.path.exists(existing_file):
            print("Copying existing ini file")
            shutil.copyfile(existing_file, ini_file)