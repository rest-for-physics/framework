import os
import sys
import time
import commands
import subprocess
import StringIO
import vars
import updateREST

def checkinstalled(name):
    if "root" in name:
        p = subprocess.Popen(['root-config --version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True)
        out, err = p.communicate()
        if "6." in out and err == "":
            vars.software["root"] = True
            return True
        else:
            vars.software["root"] = False
            return False
    elif "geant4" in name:
        p = subprocess.Popen(['geant4-config --version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True)
        out, err = p.communicate()
        if ("10." in out or "9." in out) and err == "":
            vars.software["geant4"] = True
            return True
        else:
            vars.software["geant4"] = False
            return False
    elif "garfield" in name:
        try:
            dir1 = os.environ["GARFIELD_HOME"]
            dir3 = os.environ["LD_LIBRARY_PATH"]
            if dir1 != "" and dir3.find(dir1) != -1:
                vars.software["garfield"] = True
                return True
        except:
            vars.software["garfield"] = False
            return False
    elif "REST" in name:
        p = subprocess.Popen(['rest-config --version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True)
        out, err = p.communicate()
        if "2." in out and err == "":
            vars.software["REST"] = True
            vars.opt["Install_Path"] = os.environ["REST_PATH"]
            out, err = subprocess.Popen(['rest-config --flags | grep REST_WELCOME'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True).communicate()
            vars.cmakeflags[0]="-D"+out.strip('\n')
            out, err = subprocess.Popen(['rest-config --flags | grep REST_GARFIELD'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True).communicate()
            vars.cmakeflags[1]="-D"+out.strip('\n')
            return True
        else:
            vars.software["REST"] = False
            return False
    elif "tinyxml" in name:
        if vars.var["OS"] == "Centos":
            out1,err1 = subprocess.Popen(['yum list installed | grep tinyxml'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True).communicate()
        elif vars.var["OS"] == "Ubuntu":
            out1,err1 = subprocess.Popen(['dpkg -l | grep tinyxml'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True).communicate()
        if ("tiny" in out1) :
            vars.software["tinyxml"] = True
            return True
        else:
            vars.software["tinyxml"] = False
            return False
    elif "pgsql" in name:
        if vars.var["OS"] == "Centos":
            out1,err1 = subprocess.Popen(['yum list installed | grep postgresql'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True).communicate()
        elif vars.var["OS"] == "Ubuntu":
            out1,err1 = subprocess.Popen(['dpkg -l | grep postgresql'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True).communicate()
        if ("post" in out1) :
            vars.software["pgsql"] = True
            return True
        else:
            vars.software["pgsql"] = False
            return False
    else:
        print "What is ", name," ... "
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
        if vars.software["tinyxml"] == False:
            print "Tinyxml is not installed!"
            return False
        return True

    def install(self):
        updateREST.repairgit()
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


#def install(name):
#    flag = True
#    if vars.opt["Check_Installed"] == "True" or vars.opt["Check_Installed"] == "true" or vars.opt["Check_Installed"] == "1":
#        if checkinstalled(name):
#            print name,"has already been installed!"
#            flag = False
#    if flag:
#        if "REST" in name:
#            if checkinstalled("root") == False:
#                print "ROOT6 is not installed!"
#                return
#            if checkinstalled("tinyxml") == False:
#                print "Tinyxml is not installed!"
#                return
#            print "installing REST...\n\n"
#            updateREST.repairgit()
#            if vars.opt["Clean_Up"] == "True":
#                os.system("rm -rf " + vars.opt["Build_Path"])
#            os.system("mkdir -p " + vars.opt["Build_Path"])
#            os.system("mkdir -p " + vars.opt["Install_Path"])
#            os.chdir(vars.opt["Build_Path"])
#            cmakecmd = "cmake " + vars.opt["Source_Path"]
#            cmakecmd = cmakecmd + " -DINSTALL_PREFIX=" + vars.opt["Install_Path"]
#            for flag in vars.cmakeflags:
#                cmakecmd = cmakecmd + " " + flag
#            print cmakecmd
#            os.system(cmakecmd)
#            os.system("make -j" + vars.opt["Make_Threads"])
#            os.system("make install")
#        elif "restG4" in name:
#            if checkinstalled("REST") == False :
#                print "you must install REST mainbody first!"
#                return
#            if checkinstalled("geant4") == False :
#                print "geant4 is not installed!"
#                return
#            print "installing restG4...\n\n"            
#            if vars.opt["Clean_Up"] == "True":
#                os.system("rm -rf " + vars.opt["Build_Path"] + "/restG4")
#            os.system("mkdir -p " + vars.opt["Build_Path"] + "/restG4")
#            os.chdir(vars.opt["Build_Path"] + "/restG4")
#            os.system("cmake " + vars.opt["Source_Path"] + "/packages/restG4/")
#            os.system("make -j" + vars.opt["Make_Threads"])
#            os.system("make install")
#        else:
#            print "I cannot install ",name
                






#def main():
#    if len(sys.argv) < 2:
#        print "Usage: python installation.py Name(REST or restG4) [opt1=aaa] [opt2=bbb] ..."
#        print "options: \nCheck_Installed=(True or False)\nInstall_Path=(a path)\nBuild_Path=(a path)\nMake_Threads=(a number, 1~8)"
#        print "flags: \n-DREST_WELCOME=(ON or OFF)\n-DREST_GARFIELD=(ON or OFF)"
#    else :
#        if len(sys.argv) > 2:
#            for i in range(2, len(sys.argv)):
#                if str(sys.argv[i])[0] == "-":
#                    vars.cmakeflags.append(sys.argv[i])
#                elif len(str(sys.argv[i]).split('=')) == 2:
#                    vars.opt[str(sys.argv[i]).split('=')[0]] = sys.argv[i].split('=')[1]
#                    print vars.opt[sys.argv[i].split('=')[0]]
#        name = sys.argv[1]
#        install(name)



        
#if __name__ == '__main__':
#	main()
