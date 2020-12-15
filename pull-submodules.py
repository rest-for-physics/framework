#!/usr/bin/python

import os
import subprocess

PROJECT_ROOT = os.path.dirname(os.path.realpath(__file__))

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
                     p1 = subprocess.run('cd {} && git submodule init {}'.format(root, submodule), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
##                     print ( p1.stdout )
##print ( p1.stderr )
                     p2 = subprocess.run('cd {} && git submodule update {}'.format(root, submodule), shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                     ##print ( p2.stdout )
                     ##print ( p2.stderr )
                     errorOutput = p2.stderr.decode("utf-8")
                     if errorOutput.find("failed") == -1:
                        print ( fullpath.rstrip() + "[\033[92m OK \x1b[0m]" )

##                     p1.stdout
##updateOutput = os.popen('cd {} && git submodule update {}'.format(root, submodule)).read()

if __name__ == '__main__':
   main()

