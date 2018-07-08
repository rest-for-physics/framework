import os,sys

opt = {
    'Install_Path': os.environ['HOME']+"/REST_Install",
    'Source_Path': os.path.abspath('.') + "/../",
    'Build_Path' : os.path.abspath('.') + "/../build/",
    'Make_Threads':"4",
    'Branch':"V2.2_processes",
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

