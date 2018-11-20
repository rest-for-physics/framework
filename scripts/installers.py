import os
import sys
import time
import commands
import subprocess
import StringIO
import vars



def initgit():
    out, err = subprocess.Popen(["git rev-parse --short HEAD"], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True).communicate()
    if "fatal" in err:
        print "linking remote git repository to local"
        os.chdir(vars.opt["Source_Path"])
        os.system("rm -rf .git")
        os.system("git init")
        os.system("git remote add origin "+vars.var["REST_Repository"])
        os.system("git checkout -b "+vars.opt["Branch"])
        os.system("git fetch --depth 1")
        os.system("git reset --hard origin/"+vars.opt["Branch"])
        print "git repository is ready!"
        return True
    else :
        print "git repository is ready!"
        return False


class installer():

    def ready(self):
        print "checking environment"
        return False

    def install(self):
        print "installing"


class RESTinstaller(installer):

    def ready(self):
        if vars.software["root"] == False:
            print "ROOT6 is not installed!"
            return False
        return True

    def install(self):
        initgit()
        os.system("mkdir -p " + vars.opt["Build_Path"])
        os.system("mkdir -p " + vars.opt["Install_Path"])
        os.chdir(vars.opt["Build_Path"])
        cmakecmd = "cmake " + vars.opt["Source_Path"]
        cmakecmd = cmakecmd + " -DINSTALL_PREFIX=" + vars.opt["Install_Path"]
        for flag in vars.cmakeflags:
            cmakecmd = cmakecmd + " " + flag
        print cmakecmd
        os.system(cmakecmd)
        os.system("make -j" + vars.opt["Make_Threads"])
        os.system("make install")



class restG4installer(installer):

    def ready(self):
        if vars.software["geant4"] == False :
            print "geant4 is not installed!"
            return False
        return True


    def install(self):
        if vars.software["REST"] == False :
            print "you must install REST mainbody first!"
            return False
        print "installing restG4...\n\n"            
        os.system("mkdir -p " + vars.opt["Build_Path"] + "/restG4")
        os.chdir(vars.opt["Build_Path"] + "/restG4")
        os.system("cmake " + vars.opt["Source_Path"] + "/packages/restG4/")
        os.system("make -j" + vars.opt["Make_Threads"])
        os.system("make install")
        return True


class restDBinstaller(installer):

    def ready(self):
        if vars.software["pgsql"] == False:
            print "PostgreSQL is not installed!"
            return False
        return True


    def install(self):
        if vars.software["REST"] == False :
            print "you must install REST mainbody first!"
            return False
        print "installing restDataBaseImpl...\n\n"            
        os.system("mkdir -p " + vars.opt["Build_Path"] + "/restDataBaseImpl")
        os.chdir(vars.opt["Build_Path"] + "/restDataBaseImpl")
        os.system("cmake " + vars.opt["Source_Path"] + "/packages/restDataBaseImpl/")
        os.system("make -j" + vars.opt["Make_Threads"])
        os.system("make install")
        return True


class restCustomProcessinstaller(installer):

    def ready(self):
        return True


    def install(self):
        if vars.software["REST"] == False :
            print "you must install REST mainbody first!"
            return False
        print "installing userRESTLibrary...\n\n"            
        os.system("mkdir -p " + vars.opt["Build_Path"] + "/userRESTLibrary")
        os.chdir(vars.opt["Build_Path"] + "/userRESTLibrary")
        os.system("cmake " + vars.opt["Source_Path"] + "/packages/userRESTLibrary/")
        os.system("make -j" + vars.opt["Make_Threads"])
        os.system("make install")
        return True


class restGasinstaller(installer):

    def ready(self):
        if vars.software["garfield"] == False:
            print "Garfield is not installed!"
            return False
        return True


    def install(self):
        if vars.software["REST"] == False :
            print "you must install REST mainbody first!"
            return False
        print "installing restGas...\n\n"            
        os.system("mkdir -p " + vars.opt["Build_Path"] + "/restGas")
        os.chdir(vars.opt["Build_Path"] + "/restGas")
        os.system("cmake " + vars.opt["Source_Path"] + "/packages/restGas/")
        os.system("make -j" + vars.opt["Make_Threads"])
        os.system("make install")
        return True
