# Table of Contents
1. Set up:
    - [Ubuntu-based linux distributions](#set-up-on-ubuntu-based-linux-distributions)
    - [Windows 10](#set-up-on-windows-10)
2. [Compilie the project](#compile-the-project)
3. [Create a database](#create-a-database)
4. [Run a query](#run-a-query)

# Set up
## Set up on Ubuntu-based linux distributions
- Install prerrequisites to compile:
    - `sudo apt update`
    - `sudo apt install git g++ cmake libssl-dev libboost-dev gdb wamerican`

- Set up ssh key (optional, highly recomended):
     - `ssh-keygen -t rsa -b 4096 -C "you@example.com"` (replace `you@example.com` with your email. Unless you know what you are doing, just press enter when prompted).
    - `eval $(ssh-agent -s)`
    - `ssh-add ~/.ssh/id_rsa`
    - `cat ~/.ssh/id_rsa.pub`
    - Import your ssh key at https://github.com/settings/keys (copy and paste the output from the last command)

 - Configure git if you haven't yet:
    - `git config --global user.email "you@example.com"` (replace `you@example.com` with your email).
    - `git config --global user.name "Your Name"` (replace `Your Name` with your real name).

- Clone the repository:
    - using ssh key: `git clone git@github.com:DomagojVrgoc/GraphDB.git`
    - without ssh key: `git clone https://github.com/DomagojVrgoc/GraphDB.git`

- Install Visual Studio Code:
    - Download and install from https://code.visualstudio.com/. Select the .deb installer.

    - Install recomended Extensions (optional, highly recomended):
        - `ms-vscode.cpptools`
        - `twxs.cmake`
        - `wayou.vscode-todo-highlight`
        - `shardulm94.trailing-spaces`
        - Reload Visual Studio Code

## Set up on Windows 10
- Install WSL:
    - Open PowerShell as administrator and run `Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Windows-Subsystem-Linux`
    - Restart when prompted.
    - Install Ubuntu from Microsoft Store.
- Install Visual Studio Code
    - Download and install from https://code.visualstudio.com/
    - Install the extension Remote-WSL
- First time Setup
    - Open Ubuntu app
    - **Important**: how to copy-paste in the ubuntu app console
        - To copy text from the console you can select the text dragging left-click, then right-click.
        - To paste some copied text, just right-click (having no text selected, otherwise selected text will be copied).
    - Install prerrequisites to compile:
        - `sudo apt update`
        - `sudo apt install git g++ cmake libssl-dev libboost-dev gdb wamerican`
    - Set up ssh key (optional, highly recomended):
        - `ssh-keygen -t rsa -b 4096 -C "you@example.com"` (replace `you@example.com` with your email. Unless you know what you are doing, just press enter when prompted).
        - `eval $(ssh-agent -s)`
        - `ssh-add ~/.ssh/id_rsa`
        - `cat ~/.ssh/id_rsa.pub`
        - Import your ssh key at https://github.com/settings/keys (copy and paste the output from the last command)
    - Configure git:
        - `git config --global user.email "you@example.com"` (replace `you@example.com` with your email).
        - `git config --global user.name "Your Name"` (replace `Your Name` with your real name).

    - Clone the repository:
        - using ssh key: `git clone git@github.com:DomagojVrgoc/GraphDB.git`
        - without ssh key: `git clone https://github.com/DomagojVrgoc/GraphDB.git`
    - Enter to the project folder:
        - `cd GraphDB`
    - Open the project in Visual Studio Code:
        - `code .`
    - Install recomended Visual Studio Code Extensions (optional, highly recomended):
        - `ms-vscode.cpptools`
        - `twxs.cmake`
        - `wayou.vscode-todo-highlight`
        - `shardulm94.trailing-spaces`
        - Reload Visual Studio Code

- Reopening the project (after first setup have been completed)
    - Open Ubuntu app
    - Open Visual Studio Code:
        - `code ./path/to/project/folder`

# Compile the project:
- Release version:
    - `cmake -H. -Bbuild/Release -DCMAKE_BUILD_TYPE=Release`
    - `cmake --build build/Release/`
- Debug version:
    - `cmake -H. -Bbuild/Debug -DCMAKE_BUILD_TYPE=Debug`
    - `cmake --build build/Debug/`
- Cleaning:
    - `cmake --build build/Release/ --target clean`
    - `cmake --build build/Debug/ --target clean`

# Create a database
- Delete previous database files if they exists:
    - `rm -r test_files/*.*`
- `build/Release/bin/create_db ./path/to/nodes_file ./path/to/edges_file`

# Run a query
- `build/Release/bin/query ./path/to/query_file`

