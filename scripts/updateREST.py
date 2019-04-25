#!/usr/bin/python

from __future__ import print_function

import os,sys, time, commands
import subprocess, StringIO
import vars
import installers



def commitid():
    if os.path.exists(vars.opt["Source_Path"] + "/.git/"):
        os.chdir(vars.opt["Source_Path"])
        out, err = subprocess.Popen(["git rev-parse --short HEAD"], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True).communicate()
        return str(out)
    return ""

def branchname():
    if os.path.exists(vars.opt["Source_Path"] + "/.git/"):
        os.chdir(vars.opt["Source_Path"])
        out, err = subprocess.Popen(["git branch"], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True).communicate()
        n1=str(out).find("*")
        s=str(out)[n1:-1]
        n2=s.find("\n")
        return str(out)[n1+2:n2+n1]
    return ""


def update():
    newgit=installers.initgit()
    if newgit==False:
        print("updating local git repository of REST")
        os.chdir(vars.opt["Source_Path"])
        os.system("git stash")
        p = subprocess.Popen(['git fetch origin '+vars.opt["Branch"] + ' --tags'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True)
        out, err = p.communicate()
        if "up-to-date" in out:
            print("REST is already up-to-date")
            return False
        elif "fatal:" in err:
            print(err)
            return False
        os.system("git reset --hard FETCH_HEAD")
        return True
    print("Project files has been updated!")
    print(commitid())
    print(branchname())
    return True



if __name__ == '__main__':
    vars.initvar()
    if vars.software["REST"] == True:
        print("")
        print("!!!Updating REST from git repository!!! Target branch: " + vars.opt["Branch"])
        print("!!!Local changes will be overwritten!!! (except additions). Make backup and do it carefully.")
        print("Press a key to continue, Press ctrl-c to cancel.")
        raw_input()
        update()
        a = installers.RESTinstaller()
        a.install()
    else:
        print("ERROR! REST is not installed. Install it first!")
