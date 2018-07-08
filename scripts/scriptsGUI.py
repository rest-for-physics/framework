#!/usr/bin/python


import Tkinter as tk
import ttk

import os
import time
import updateREST
import installers
import vars
import subprocess
from multiprocessing import cpu_count

version="0.1"

print "processors in cpu : ", cpu_count()
vars.opt["Make_Threads"] = str(cpu_count() / 2)
if int(vars.opt["Make_Threads"]) > 8:
    vars.opt["Make_Threads"] = "8"

import platform
s = platform.platform()
if s.find('Ubuntu')!=-1:
    vars.var["OS"]="Ubuntu"
if s.find('centos')!=-1:
    vars.var["OS"]="Centos"

print "Operating system: ", vars.var["OS"], "(", s,")"

print ""

print "checking if root is installed...", installers.checkinstalled("root6")
print "checking if geant4 is installed...", installers.checkinstalled("geant4")
print "checking if garfield is installed...", installers.checkinstalled("garfield")
print "checking if REST is installed...", installers.checkinstalled("REST")
print "checking if tinyxml is installed...", installers.checkinstalled("tinyxml")
print "checking if PostgreSQL is installed...", installers.checkinstalled("pgsql")

win = tk.Tk() 
win.title("REST Scripts GUI V"+version) 
win.resizable(0, 0) 

#define step actions
global step
step=0
global previoussteps
previoussteps=[]
#(0~9 REST, 10~19 restG4, <0 : install completed, 0\10\20...  : begin of the wizard)
install_begin=0
install_git=1
install_clearbuild=2
install_path=3
install_garfield=4
install_welcomemsg=5
install_confirm=6
install_finished=7
update_confirm=10

instruction1 = tk.StringVar()
instruction2 = tk.StringVar()
instruction3 = tk.StringVar()
chVarDis1 = tk.IntVar()
chVarDis2 = tk.IntVar()
chVarDis3 = tk.IntVar()
chVarDis4 = tk.IntVar()

rest=installers.RESTinstaller()
restG4=installers.restG4installer()
restDB=installers.restDBinstaller()
restCProc=installers.restCustomProcessinstaller()


import tkFileDialog

def refreshdisplay():
    global step
    print "step: ", step
    if step == install_begin:
        text.delete("1.0",tk.END)
        btnprev.config(state='disabled',text='previous')
        btnnext.config(state='normal',text='next')
        instruction1.set("Source directory: "+vars.opt["Source_Path"])
        instruction2.set("Build directory: "+vars.opt["Build_Path"])
        instruction3.set("Installing REST... Please follow the wizard")
    elif step == install_git:
        text.delete("1.0",tk.END)
        text.insert("1.0",vars.opt["Branch"])
        btnprev.config(state='normal',text='previous')
        btnnext.config(state='normal',text='next')
        instruction1.set("Source directory is not a git repository!")
        instruction2.set("We will initialize git, and connect to remote server")
        instruction3.set("Choose a branch to syncronize: ")
    elif step == install_clearbuild:
        text.delete("1.0",tk.END)
        text.insert("1.0","no")
        btnprev.config(state='normal',text='previous')
        btnnext.config(state='normal',text='next')
        instruction1.set("The build path seems not empty, ")
        instruction2.set("Clear it or not? (yes/no)")
        instruction3.set("(keeping it may make the build quicker)")    
    elif step == install_path:
        text.delete("1.0",tk.END)
        text.insert("1.0",vars.opt["Install_Path"])
        btnprev.config(state='normal',text='previous')
        btnnext.config(state='normal',text='next')
        instruction1.set("")
        instruction2.set("Choose the installation path:")
        instruction3.set("")
    elif step == install_garfield:
        text.delete("1.0",tk.END)
        instruction1.set("")
        if(installers.checkinstalled("garfield")):
            instruction2.set("Turning on REST garfield accessibility? (ON/OFF)")
            text.insert("1.0",vars.cmakeflags[1].split("=")[1])
        else:
            instruction2.set("Garfield has not been installed, install REST anyway? (yes/no)")
            text.insert("1.0","yes")
        instruction3.set("")
    elif step == install_welcomemsg:
        text.delete("1.0",tk.END)
        text.insert("1.0",vars.cmakeflags[0].split("=")[1])
        instruction1.set("")
        instruction2.set("Enable welcome message in when logging in? (ON/OFF)")
        instruction3.set("")
    elif step == install_confirm:
        text.delete("1.0",tk.END)
        text.insert("1.0","Confirm install")
        btnprev.config(state='normal',text='previous')
        btnnext.config(state='normal',text='install')
        instruction1.set("Cmake flages: "+vars.cmakeflags[0]+" "+vars.cmakeflags[1])
        instruction2.set("Install path: "+vars.opt["Install_Path"])
        instruction3.set("After installation you need to source thisREST.sh manually")
    elif step == install_finished:
        text.delete("1.0",tk.END)
        btnprev.config(state='normal',text='previous')
        btnnext.config(state='disabled',text='install')
        instruction1.set("")
        instruction2.set("Finished!")
        instruction3.set("")
    elif step == update_confirm:
        text.delete("1.0",tk.END)
        text.insert("1.0",vars.opt["Branch"])
        btnprev.config(state='normal',text='previous')
        btnnext.config(state='normal',text='update')
        instruction1.set("")
        instruction2.set("Select a branch to update: ")
        instruction3.set("")








def nextstep():
    global step
    if step == install_begin:
        out, err = subprocess.Popen(["git rev-parse --short HEAD"], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True).communicate()
        if "fatal" in err:
            step = install_git
        elif os.path.exists(vars.opt["Build_Path"]) and os.listdir(vars.opt["Build_Path"]):
            step=install_clearbuild
        else :
            step = install_path
    elif step == install_git:
        vars.opt['Branch']=text.get("1.0", tk.END).strip('\n')
        updateREST.repairgit()
        if os.path.exists(vars.opt["Build_Path"]) and os.listdir(vars.opt["Build_Path"]):
            step=install_clearbuild
        else :
            step = install_path
    elif step == install_clearbuild:
        if "Y" in text.get("1.0", tk.END).strip('\n').upper():
            os.system("rm -rf " + vars.opt["Build_Path"])
        elif "N" in text.get("1.0", tk.END).strip('\n').upper():
            step = install_path
        step = install_path
    elif step == install_path:
        vars.opt["Install_Path"] = text.get("1.0", tk.END).strip('\n')
        step=install_garfield
    elif step == install_garfield:
        if(installers.checkinstalled("garfield")):
            if "ON" in text.get("1.0", tk.END).strip('\n').upper():
                vars.cmakeflags[1]="-DREST_GARFIELD=ON"
            elif "OFF" in text.get("1.0", tk.END).strip('\n').upper():
                vars.cmakeflags[1]="-DREST_GARFIELD=OFF"
        else:
            if "Y" in text.get("1.0", tk.END).strip('\n').upper():
                vars.cmakeflags[1]="-DREST_GARFIELD=OFF"
            elif "N" in text.get("1.0", tk.END).strip('\n').upper():
                step=install_finished
        step=install_welcomemsg
    elif step == install_welcomemsg:
        vars.cmakeflags[0]="-DREST_WELCOME="+ text.get("1.0", tk.END).strip('\n')
        step=install_confirm
    elif step == install_confirm:
        if chVarDis1.get() == 1:
            rest.install()
            os.environ["REST_PATH"] = vars.opt["Install_Path"]
            os.environ["LD_LIBRARY_PATH"] = vars.opt["Install_Path"]+"/lib:"+os.environ["LD_LIBRARY_PATH"]
            vars.software["REST"] = True
        if chVarDis2.get() == 1:
            restG4.install()
        if chVarDis3.get() == 1:
            restDB.install()
        if chVarDis4.get() == 1:
            restCProc.install()
        step=install_finished
    elif step == install_finished:
        print "You can't hit this!"
    elif step == update_confirm:
        vars.opt['Branch']=text.get("1.0", tk.END).strip('\n')
        updateREST.Warning=False
        updateREST.main()
        step =install_begin;
    previoussteps.append(step)
    refreshdisplay()

def prevstep():
    global step
    if len(previoussteps)>1:
        step=previoussteps[len(previoussteps)-2]
        previoussteps.pop()
        refreshdisplay()


def begininstall():
    global step,previoussteps
    step=install_begin
    previoussteps=[]
    previoussteps.append(step)
    refreshdisplay()


def beginupdate():
    global step,previoussteps
    step=update_confirm
    previoussteps=[]
    previoussteps.append(step)
    refreshdisplay()
 
def callCheckbutton1():
    print 'how can you check this button!', chVarDis1.get()

def callCheckbutton2():
    if chVarDis2.get() == 1:
        if(restG4.ready()):
            print "selected package restG4"
        else:
            print "lacking dependency of restG4! cannot add"
            check2.deselect()
    else:
        print "deselect package restG4"

def callCheckbutton3():
    if chVarDis3.get() == 1:
        if(restDB.ready()):
            print "selected package restDabaBaseImpl"
        else:
            print "lacking dependency of restDabaBase! cannot add"
            check3.deselect()
    else:
        print "deselect package restDabaBaseImpl"

def callCheckbutton4():
    if chVarDis4.get() == 1:
        if(restCProc.ready()):
            print "selected package restCustomProcesses"
        else:
            print "lacking dependency of restCustomProcesses! cannot add"
            check4.deselect()
    else:
        print "deselect package restCustomProcesses"


btninstall = ttk.Button(win, text="Install!", command=begininstall,width=10)
btninstall.grid(column=0, row=0)

btnupdate = ttk.Button(win, text="Update!", command=beginupdate,width=10)
btnupdate.grid(column=0, row=1)

labelsFrame = ttk.LabelFrame(win, text=' Select package to install ')
labelsFrame.grid(column=1, row=0, rowspan=2, sticky=tk.EW)


check1 = tk.Checkbutton(labelsFrame, text="REST", variable=chVarDis1, state ='disabled',command = callCheckbutton1)
check1.grid(column=0, row=0, sticky=tk.W)
check1.select()

check2 = tk.Checkbutton(labelsFrame, text="restG4", variable=chVarDis2, state ='active',command = callCheckbutton2)
check2.grid(column=0, row=1, sticky=tk.W)

check3 = tk.Checkbutton(labelsFrame, text="restDataBase", variable=chVarDis3, state ='active',command = callCheckbutton3)
check3.grid(column=0, row=2, sticky=tk.W)

check4 = tk.Checkbutton(labelsFrame, text="restCustomProcessLibray", variable=chVarDis4, state ='active',command = callCheckbutton4)
check4.grid(column=0, row=3, sticky=tk.W)



wizardFrame = ttk.LabelFrame(win, text=' Installation Wizard ')
wizardFrame.grid(column=0, row=2, columnspan=2, sticky=tk.EW)



label1 = ttk.Label(wizardFrame,textvariable=instruction1)
label1.grid(row=0,column=0,columnspan=3, sticky=tk.EW)

label2 = ttk.Label(wizardFrame,textvariable=instruction2)
label2.grid(row=1,column=0,columnspan=3, sticky=tk.EW)

label3 = ttk.Label(wizardFrame,textvariable=instruction3)
label3.grid(row=2,column=0,columnspan=3, sticky=tk.EW)

btnprev = ttk.Button(wizardFrame, text="Previous", command=prevstep,width=10)
btnprev.grid(column=0, row=3)
btnprev.config(state='disabled',text='previous')

btnnext = ttk.Button(wizardFrame, text="Next", command=nextstep,width=10)
btnnext.grid(column=2, row=3)
btnnext.config(state='disabled',text='next')

text = tk.Text(wizardFrame,height=1,width=35)
text.grid(column=1, row=3)
text.insert("1.0","input options here")


if(rest.ready()):
    instruction2.set("select to install or update")
else:
    instruction2.set("Lacking package, cannot continue")
    btninstall.config(state='disabled')
    btnupdate.config(state='disabled')
    btnnext.config(state='disabled')
    btnprev.config(state='disabled')





win.mainloop()