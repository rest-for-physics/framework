#!/usr/bin/python3

##
## This script will pull and synchronize all REST submodules to the corresponding reference given at the main repository
##
## Be carefully, if there are commits inside the submodules that have not been pushed to the corresponding remote,
## they are at risk of being lost!
##
## Usage: python3 pull-submodules.py [--clean] [--force] [--dontask] [--latest:COMMIT_HASH]
##

import os
import sys
import subprocess

PROJECT_ROOT = os.path.dirname(os.path.realpath(__file__))

lfna = False
sjtu = False
latest = False
debug = False
force = False
dontask = False
clean = False
fbName = ""
onlylibs = False

def print_help():
    print( " pull-submodules.py is a helper script to pull REST-for-Physics related submodules" )
    print( " " )
    print( "Usage: " )
    print( "python3 pull-submodules.py --clean" )
    print( "python3 pull-submodules.py --latest" )
    print( " " )
    print( " --clean : It will restore all submodules to the official release versions" )
    print( " --latest : It will pull the master branch of each submodule" )
    print ( " ")
    print( "When using --clean make sure the framework local repository is at an official release" )
    print ( " " )
    print ( " Other complementary options: " )
    print ( "  --force : It will override changes using git reset" )
    print ( "  --dontask : It wont ask when overriding changes" )
    print ( "  --lfna : It will pull LFNA Git repositories. SSH grant required" )
    print ( "  --sjtu : It will pull SJTU Git repositories. SSH grant required" )
    print ( "  --exclude:lib1,lib2 will prevent lib1,lib2 from being pulled" )
    print ( "  --onlylibs: It will pull only the REST library submodules" )
    print ( "  --data: It will pull also data based repositories" )
    print ( "  --only:restG4,geant4lib will pull only the selected repositories." )
    print ( " " )

if( len(sys.argv ) <= 1 ):
    print( " " )
    print( "ERROR pull-submodules.py requires arguments." )
    print_help()
    sys.exit(1)

exclude_elems = ["userguide", "data"]
only_elems = []

for x in range(len(sys.argv) - 1):
    if sys.argv[x + 1] == "--data":
        exclude_elems = ["userguide"]

    if sys.argv[x + 1] == "--lfna":
        lfna = True
        print("""\
Adding submodules from lfna repositories.
Be aware that you should add your local system public ssh to your GitLab and/or GitHub account!
It is usually placed at ~/.ssh/id_rsa.pub.
If it does not exist just create a new one using 'ssh-keygen -t rsa'.

ATTENTION: If a password it is requested the reason behind is no public key for this system is found at the remote repository.
Once you do that, only repositories where you have access rights will be pulled.

If no password is requested everything went fine!
            """)

    if sys.argv[x + 1] == "--sjtu":
        sjtu = True
        print("Adding submodules from sjtu repositories. You may be asked to enter password for it.")
    if sys.argv[x + 1].find("--latest") >= 0:
        print("""\
Pulling latest submodules from their git repository, instead of the version recorded by REST.
This may cause the submodules to be uncompilable.
        """)
        latest = True
        if sys.argv[x + 1].find("--latest:") >= 0:
            fbName = sys.argv[x + 1][9:]
    if sys.argv[x + 1] == "--debug":
        debug = True
    if sys.argv[x + 1] == "--dontask":
        dontask = True
    if sys.argv[x + 1] == "--force":
        force = True
    if sys.argv[x + 1] == "--onlylibs":
        onlylibs = True
    if sys.argv[x + 1] == "--clean":
        force = True
        clean = True
    if sys.argv[x + 1] == "--help" or sys.argv[x + 1] == "-h":
        print_help()
        sys.exit(1)
    if sys.argv[x + 1].find("--exclude:") >= 0:
        elems = sys.argv[x + 1][10:].split(",")
        for y in elems:
            exclude_elems.append(y)
    if sys.argv[x + 1].find("--only:") >= 0:
        elems = sys.argv[x + 1][7:].split(",")
        for y in elems:
            only_elems.append(y)



def main():
    # The following command may fail
    #   os.system('cd {} && git submodule update --init
    #   --recursive'.format(PROJECT_ROOT))

    if force and not dontask:
        answer = input(
            """\
This will override local changes on the files. And will bring your local repository to a clean state
Are you sure to proceed? (y/n)
            """)
        if answer != "y":
            sys.exit(0)

    if force:
        print("Force pulling submodules.")

    if fbName == "":
        bNamePcs = subprocess.run("git rev-parse --abbrev-ref HEAD", shell=True, stdout=subprocess.PIPE,
                                  stderr=subprocess.PIPE)
        frameworkBranchName = bNamePcs.stdout.decode("utf-8").rstrip("\n")
    else:
        frameworkBranchName = fbName

    print(f"Framework branch name: {frameworkBranchName}")

    # In case the above command failed, also go through all submodules and update them individually
    for root, dirs, files in os.walk(PROJECT_ROOT):
        for filename in files:
            if filename == ".gitmodules":
                with open(os.path.join(root, filename), 'r') as gitmodules_file:
                    for line in gitmodules_file:
                        line = line.replace(' ', '')
                        if "path=" in line:
                            exclude = False
                            submodule = line.replace("path=", '').strip()
                            fullpath = os.path.join(root, submodule).replace(' ', '')
                            if fullpath.find("project") >= 0:
                                fullpath = fullpath[fullpath.find("project"):]
                            if fullpath.find("source") >= 0:
                                fullpath = fullpath[fullpath.find("source"):]
                            if fullpath.find("packages") >= 0:
                                fullpath = fullpath[fullpath.find("packages"):]
                            if fullpath.find("scripts") >= 0:
                                fullpath = fullpath[fullpath.find("scripts"):]

                            for exclude_element in exclude_elems:
                                if fullpath.lower().find(exclude_element.lower()) > 0:
                                    exclude = True
                            if onlylibs and fullpath.lower().find("libraries") == -1:
                                exclude = True
                            for only_element in only_elems:
                                exclude = True
                                if fullpath.find(only_element) > 0:
                                    exclude = False

                        if "url=" in line:
                            url = line.replace("url=", '').strip()

                            for exclude_element in exclude_elems:
                                if url.lower().find(exclude_element.lower()) > 0:
                                    exclude = True

                            if (not exclude and url.find("github") != -1) or (
                                    url.find("lfna.unizar.es") != -1 and lfna) or (
                                    url.find("gitlab.pandax.sjtu.edu.cn") != -1 and sjtu):
                                print("Updating submodule: ", end="") 
                                print(fullpath.rstrip(), end='')
                                # init
                                p = subprocess.run(f"cd {root} && git submodule update --init {submodule}",  #
                                                   shell=True,
                                                   stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                                if debug:
                                    print(p.stdout.decode("utf-8"))
                                    print(p.stderr.decode("utf-8"))
                                if p.stdout.decode("utf-8").find("checkout") >= 0:
                                    print(p.stdout.decode("utf-8"))
                                errorOutput = p.stderr.decode("utf-8")
                                if errorOutput.find("failed") != -1 or errorOutput.find("error") != -1 or errorOutput.find("ERROR") != -1 or errorOutput.find("fatal") != -1:
                                    print("[\033[91m Failed \x1b[0m]")
                                    if debug:
                                        print("Message: ")
                                        print(errorOutput)
                                else:
                                    print("[\033[92m OK \x1b[0m]")

    # back to the git framework directory
    p = subprocess.run(f"cd {root}",  #
                       shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # if 'force', override the changes with git reset
    if force:
        print("Forcing reset: ", end="") 
        p = subprocess.run(f"git submodule foreach --recursive 'git reset --hard'",  #
                           shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if debug:
            print(p.stdout.decode("utf-8"))
            print(p.stderr.decode("utf-8"))
        errorOutput = p.stderr.decode("utf-8")
        if errorOutput.find("failed") != -1 or errorOutput.find("error") != -1 or errorOutput.find("ERROR") != -1 or errorOutput.find("fatal") != -1:
            print("[\033[91m Failed \x1b[0m]")
            if debug:
                print("Message: ")
                print(errorOutput)
        else:
            print("[\033[92m OK \x1b[0m]")

    # if latest, pull the latest commit instead of the one recorded in the main repo
    if latest:
        print("Pulling submodules: ", end="") 
        p = subprocess.run(f"git submodule foreach  --recursive 'git fetch; if [ -z \"$(git ls-remote --heads origin {frameworkBranchName})\" ]; then git checkout master; else git checkout {frameworkBranchName};fi;git pull'",  #
                           shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if debug:
            print(p.stdout.decode("utf-8"))
            print(p.stderr.decode("utf-8"))
        errorOutput = p.stderr.decode("utf-8")
        if errorOutput.find("failed") != -1 or errorOutput.find("error") != -1 or errorOutput.find("ERROR") != -1 or errorOutput.find("fatal") != -1:
            print("[\033[91m Failed \x1b[0m]")
            if debug:
                print("Message: ")
                print(errorOutput)
        else:
            print("[\033[92m OK \x1b[0m]")
    # get commit id
    p = subprocess.run(f"git submodule foreach  --recursive 'git rev-parse HEAD'",  #
                           shell=True,
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    print(p.stdout.decode("utf-8"))

    if clean:
        subprocess.run(f"git clean -xfd", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        subprocess.run(f"git reset --hard", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)


if __name__ == "__main__":
    main()
