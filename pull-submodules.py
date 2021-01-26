#!/usr/bin/python

##
## This script will pull and syncrhonize all REST submodules to the
## corresponding
## reference given at the main repository.
##
## Be carefull, if there are commits inside the sub-modules that have not been
## pushed
## to the corresponding remote, they risk from being lost!
##
## Use: python3.5 pull-submodules.py
##
import os
import sys
import subprocess

debug = 0

PROJECT_ROOT = os.path.dirname(os.path.realpath(__file__))

narg = len(sys.argv)
private = 0
latest = 0
debug = 0
force = 0

for x in range(narg - 1):
    if (sys.argv[x + 1] == "--private"):
        private = 1
        print("\nBe aware that you should add your local system public ssh to your GitLab and/or GitHub account!\nIt is usually placed at ~/.ssh/id_rsa.pub.\nIf it does not exist just create a new one using 'ssh-keygen -t rsa'.\n\nATTENTION: If a password it is requested the reason behind is no public key for this system is found at the remote repository.\nOnce you do that, only repositories where you have access rights will be pulled.\n\nIf no password is requested everything went fine!\n")
    if (sys.argv[x + 1] == "--latest"):
        latest = 1
        print("\nPulling latest submodules from their git repository")
    if (sys.argv[x + 1] == "--debug"):
        debug = 1
    if (sys.argv[x + 1] == "--force"):
        force = 1
        print("\nForce pulling submodules. It will overrite local changes on the files.")

def main():
# The following command may fail
#   os.system('cd {} && git submodule update --init
#   --recursive'.format(PROJECT_ROOT))

# In case the above command failed, also go through all submodules and update
# them individually
   for root, dirs, files in os.walk(PROJECT_ROOT):
      for filename in files:
         if filename == '.gitmodules':
            with open(os.path.join(root, filename), 'r') as gitmodules_file:
               for line in gitmodules_file:
                  line = line.replace(' ', '')
                  if 'path=' in line:
                     submodule = line.replace('path=', '').strip()
                     fullpath = os.path.join(root , submodule).replace(' ', '')
                     if fullpath.find("project") >= 0:
                        fullpath = fullpath[fullpath.find("project"):]
                     if fullpath.find("source") >= 0:
                        fullpath = fullpath[fullpath.find("source"):]
                     if fullpath.find("packages") >= 0:
                        fullpath = fullpath[fullpath.find("packages"):]
                     if fullpath.find("scripts") >= 0:
                        fullpath = fullpath[fullpath.find("scripts"):]
                  if 'url=' in line:
                     url = line.replace('url=', '').strip()
                     if(url.find("http") != -1 and private == 0) or (url.find("git@") != -1 and private == 1):
                         if(debug):
                             print("Pulling from: " + url)
                         # init
                         p = subprocess.run('cd {} && git submodule init {}'.format(root, submodule), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                         if(debug):
                             print (p.stdout.decode("utf-8"))
                             print (p.stderr.decode("utf-8"))
                         if (p.stdout.decode("utf-8").find("checkout") >= 0):
                             print(p.stdout.decode("utf-8"))
                         errorOutput = p.stderr.decode("utf-8")
                         if errorOutput.find("failed") != -1 or errorOutput.find("error") != -1:
                             print (fullpath.rstrip() + "[\033[91m Failed \x1b[0m]")
                             print ("Message: ")
                             print (errorOutput)
                             continue
                         # if force, overrite the changes with git reset
                         if force == 1:
                             p = subprocess.run('cd {}/{} && git reset --hard'.format(root, submodule), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                             if(debug):
                                 print (p.stdout.decode("utf-8"))
                                 print (p.stderr.decode("utf-8"))
                             errorOutput = p.stderr.decode("utf-8")
                             if errorOutput.find("failed") != -1 or errorOutput.find("error") != -1:
                                 print (fullpath.rstrip() + "[\033[91m Failed \x1b[0m]")
                                 print ("Message: ")
                                 print (errorOutput)
                                 continue
                         # update submodule
                         p = subprocess.run('cd {} && git submodule update {}'.format(root, submodule), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                         if(debug):
                             print (p.stdout.decode("utf-8"))
                             print (p.stderr.decode("utf-8"))
                         if (p.stdout.decode("utf-8").find("checkout") >= 0):
                             print(p.stdout.decode("utf-8"))
                         errorOutput = p.stderr.decode("utf-8")
                         if errorOutput.find("failed") != -1 or errorOutput.find("error") != -1:
                             print (fullpath.rstrip() + "[\033[91m Failed \x1b[0m]")
                             print ("Message: ")
                             print (errorOutput)
                             continue
                         # if latest, pull the latest commit instead of the one
                         # recorded in the main repo
                         if latest == 1:
                             p = subprocess.run('cd {}/{} && git pull'.format(root, submodule), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                             if(debug):
                                 print (p.stdout.decode("utf-8"))
                                 print (p.stderr.decode("utf-8"))
                             errorOutput = p.stderr.decode("utf-8")
                             if errorOutput.find("failed") != -1 or errorOutput.find("error") != -1:
                                 print (fullpath.rstrip() + "[\033[91m Failed \x1b[0m]")
                                 print ("Message: ")
                                 print (errorOutput)
                                 continue
                         # get commit id
                         p = subprocess.run('cd {}/{} && git rev-parse HEAD'.format(root, submodule), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

                         if errorOutput.find("failed") == -1 and errorOutput.find("error") == -1:
                             print (fullpath.rstrip() + "[\033[92m OK \x1b[0m] (" + p.stdout.decode("utf-8")[0:7] + ")")

if __name__ == '__main__':
   main()

