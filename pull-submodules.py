#!/usr/bin/python

##
## This script will pull and syncrhonize all REST submodules to the corresponding
## reference given at the main repository.
##
## Be carefull, if there are commits inside the sub-modules that have not been pushed
## to the corresponding remote, they risk from being lost!
##
## Use: python3.5 pull-submodules.py
##

import os,sys
import subprocess

debug = 0

PROJECT_ROOT = os.path.dirname(os.path.realpath(__file__))

narg = len(sys.argv)
private = 0

for x in range(narg-1):
    if ( sys.argv[x+1] == "--private" ):
        private = 1
        print( "\nBe aware that you should add your local system public ssh to your GitLab and/or GitHub account!\nIt is usually placed at ~/.ssh/id_rsa.pub.\nIf it does not exist just create a new one using 'ssh-keygen -t rsa'.\n\nATTENTION: If a password it is requested the reason behind is no public key for this system is found at the remote repository.\nOnce you do that, only repositories where you have access rights will be pulled.\n\nIf no password is requested everything went fine!\n")

def main():
# The following command may fail
#   os.system('cd {} && git submodule update --init --recursive'.format(PROJECT_ROOT))

# In case the above command failed, also go through all submodules and update them individually
   for root, dirs, files in os.walk(PROJECT_ROOT):
      for filename in files:
         if filename == '.gitmodules':
            with open(os.path.join(root, filename), 'r') as gitmodules_file:
               for line in gitmodules_file:
                  line = line.replace(' ', '')
                  if 'path=' in line:
                     submodule = line.replace('path=', '').strip()
                     fullpath = os.path.join(root , submodule).replace( ' ', '' )
                     if fullpath.find("project") >= 0:
                        fullpath = fullpath[fullpath.find("project"):]
                     if fullpath.find("source") >= 0:
                        fullpath = fullpath[fullpath.find("source"):]
                     if fullpath.find("packages") >= 0:
                        fullpath = fullpath[fullpath.find("packages"):]
                  if 'url=' in line:
                     url = line.replace('url=', '').strip()
                     if( url.find("http") != -1 and private == 0) or (url.find("git@") != -1 and private == 1):
                         if( debug ):
                             print( "Pulling from: " + url )
                         p1 = subprocess.run('cd {} && git submodule init {}'.format(root, submodule), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                         if( debug ):
                             print ( p1.stdout.decode("utf-8") )
                             print ( p1.stderr.decode("utf-8") )
                         if ( p1.stdout.decode("utf-8").find("checkout") >= 0 ):
                             print( p1.stdout.decode("utf-8") )
                         p2 = subprocess.run('cd {} && git submodule update {}'.format(root, submodule), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                         if( debug ):
                             print ( p2.stdout.decode("utf-8") )
                             print ( p2.stderr.decode("utf-8") )
                         if ( p2.stdout.decode("utf-8").find("checkout") >= 0 ):
                             print( p2.stdout.decode("utf-8") )
                         errorOutput = p2.stderr.decode("utf-8")
                         if errorOutput.find("failed") == -1:
                             print ( fullpath.rstrip() + "[\033[92m OK \x1b[0m]" )

if __name__ == '__main__':
   main()

