# This file is used for the header of python installation scripts
from __future__ import print_function

import os,sys
from multiprocessing import cpu_count
import platform
import subprocess


opt = {
    'Install_Path': os.environ['HOME']+"/REST_Install",
    'Source_Path': os.path.abspath('.') + "/../",
    'Build_Path' : os.path.abspath('.') + "/../build/",
    'Make_Threads':"4",
    'Branch':"V2.2_processes",
    'Clean_Up':"False",
    'Branch':"master",
      }
#change the Branch key to the actual branch name in each branch

cmakeflags=[
    '-DREST_WELCOME=ON',
    '-DREST_GARFIELD=ON'
    ]

software = {
    'REST': False,
    'geant4': False,
    'root': False,
    'tinyxml': False,
    'garfiled': False,
    'pgsql': False
    }

var = {
    'OS': "unknown",
    'REST_Repository': "http://pandax.physics.sjtu.edu.cn:8699/pandax-iii/REST_v2.git",
    'GARFIELD_Repository': "http://svn.cern.ch/guest/garfield/tags/v1r0",
    'Geant4_Version':"geant4_10_02_p02",
    'Geant4_Repository':"http://geant4.cern.ch/support/source/",
    'ROOT_Repository':"https://root.cern.ch/download/root_v6.10.06.Linux-centos7-x86_64-gcc4.8.tar.gz"
    }

def checkinstalled(name):
    if "root" in name:
        p = subprocess.Popen(['root-config --version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True)
        out, err = p.communicate()
        if "6." in out and err == "":
            software["root"] = True
            return True
        else:
            software["root"] = False
            return False
    elif "geant4" in name:
        p = subprocess.Popen(['geant4-config --version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True)
        out, err = p.communicate()
        if ("10." in out or "9." in out) and err == "":
            software["geant4"] = True
            return True
        else:
            software["geant4"] = False
            return False
    elif "garfield" in name:
        try:
            dir1 = os.environ["GARFIELD_HOME"]
            dir3 = os.environ["LD_LIBRARY_PATH"]
            if dir1 != "" and dir3.find(dir1) != -1:
                software["garfield"] = True
                return True
        except:
            software["garfield"] = False
            return False
    elif "REST" in name:
        p = subprocess.Popen(['rest-config --version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True)
        out, err = p.communicate()
        if "2." in out and err == "":
            software["REST"] = True
            opt["Install_Path"] = os.environ["REST_PATH"]
            out, err = subprocess.Popen(['rest-config --flags | grep REST_WELCOME'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True).communicate()
            cmakeflags[0]="-D"+out.strip('\n')
            out, err = subprocess.Popen(['rest-config --flags | grep REST_GARFIELD'], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True).communicate()
            cmakeflags[1]="-D"+out.strip('\n')
            return True
        else:
            software["REST"] = False
            return False
    elif "tinyxml" in name:
        if var["OS"] == "Centos":
            out1,err1 = subprocess.Popen(['yum list installed | grep tinyxml'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True).communicate()
        elif var["OS"] == "Ubuntu":
            out1,err1 = subprocess.Popen(['dpkg -l | grep tinyxml'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True).communicate()
        if ("tiny" in out1) :
            software["tinyxml"] = True
            return True
        else:
            software["tinyxml"] = False
            return False
    elif "pgsql" in name:
        if var["OS"] == "Centos":
            out1,err1 = subprocess.Popen(['yum list installed | grep postgresql'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True).communicate()
        elif var["OS"] == "Ubuntu":
            out1,err1 = subprocess.Popen(['dpkg -l | grep postgresql'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True).communicate()
        if ("post" in out1) :
            software["pgsql"] = True
            return True
        else:
            software["pgsql"] = False
            return False
    else:
        print("What is ", name," ... ")
    return False



def initvar():

    print("processors in cpu : ", cpu_count())
    opt["Make_Threads"] = str(cpu_count() / 2)
    if int(opt["Make_Threads"]) > 8:
        opt["Make_Threads"] = "8"

    s = platform.platform()
    if s.find('Ubuntu')!=-1:
        var["OS"]="Ubuntu"
    if s.find('centos')!=-1:
        var["OS"]="Centos"

    print("Operating system: ", var["OS"], "(", s,")")

    print("")

    print("checking if root is installed...", checkinstalled("root6"))
    print("checking if geant4 is installed...", checkinstalled("geant4"))
    print("checking if garfield is installed...", checkinstalled("garfield"))
    print("checking if REST is installed...", checkinstalled("REST"))
    print("checking if PostgreSQL is installed...", checkinstalled("pgsql"))
