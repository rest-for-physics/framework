from Tkinter import *
import os
import time
import updateREST
import installation
import vars

print "checking if root is installed...", installation.checkinstalled("root6")
print "checking if geant4 is installed...", installation.checkinstalled("geant4")
print "checking if garfield is installed...", installation.checkinstalled("garfield")
print "checking if REST is installed...", installation.checkinstalled("REST")
print "checking if tinyxml is installed...", installation.checkinstalled("tinyxml")

root = Tk()



class Application(Frame):
    """ Create the window and populate with widgets """

    def __init__(self,parent):
        """ initializes the frame """
        Frame.__init__(self,parent,background="white")
        self.parent = parent
        self.grid()
        self.create_widgets()
        self.isopen = 0


    def create_widgets(self):
        self.btn0 = Button(self,text="install REST",height=1,width=10)
        self.btn0.grid(row=0,column=0)
        self.btn0["command"] = self.installrestbtn

        self.btn1 = Button(self,text="install restG4",height=1,width=10)
        self.btn1.grid(row=0,column=1)
        self.btn1["command"] = self.installrestG4btn

        self.btn2 = Button(self,text="update REST",height=1,width=10)
        self.btn2.grid(row=0,column=2)
        self.btn2["command"] = self.updaterestbtn

        self.btn3 = Button(self,text="Cluster Jobs",height=1,width=10)
        self.btn3.grid(row=0,column=3)

        self.label = Label(self,textvariable=self.var,height=10,width=60)
        self.label.grid(row=1,column=0,columnspan=4)
        self.var.set("select an item to install")

        self.t = Text(self,height=1,width=30)  
        self.t.grid(row=3,column=1,columnspan=2)
        self.t.insert("1.0","input options here")

        self.btn5 = Button(self,text="previous",width=10)
        self.btn5.config(state='disabled')
        self.btn5.grid(row=3,column=0)
        self.btn5["command"] = self.stepminus

        self.btn6 = Button(self,text="next",width=10)
        self.btn6.config(state='disabled')
        self.btn6.grid(row=3,column=3)
        self.btn6["command"] = self.stepplus


    var = StringVar()
    step = 0
    #(0~9 REST, 10~19 restG4, <0 : install completed, 0\10\20...  : begin of
    #wizard)

    def installrestbtn(self):
        self.step = 0
        self.stepplus()

    def installrestG4btn(self):
        self.step = 10
        self.stepplus()

    def updaterestbtn(self):
        self.step = 20
        self.stepplus()

    def stepplus(self):
        self.applyopt()
        self.step = self.step + 1
        self.refreshdisplay()

    def stepminus(self):
        self.step = self.step - 1
        self.refreshdisplay()
    
    def refreshdisplay(self):
        if self.step == 1:
            self.t.delete("1.0",END)
            self.btn5.config(state='disabled')
            self.btn6.config(state='normal',text='next')
            if(installation.checkinstalled("REST")):
                self.var.set("REST has already been installed, update it?(yes/no)")
                vars.opt["Install_Path"] = os.environ["REST_PATH"]
                self.t.insert("1.0","yes")
            else:
                self.step = 2
                self.refreshdisplay()
        elif self.step == 2:
            self.t.delete("1.0",END)
            self.btn5.config(state='disabled')
            self.btn6.config(state='normal',text='next')
            self.var.set("Installing REST... Please follow this wizard")
        elif self.step == 3:
            self.t.delete("1.0",END)
            self.t.insert("1.0",vars.opt["Install_Path"])
            self.btn5.config(state='normal')
            self.btn6.config(state='normal',text='next')
            self.var.set("Choose an installation Path:")
        elif self.step == 4:
            self.t.delete("1.0",END)
            self.t.insert("1.0",vars.opt["DREST_WELCOME"])
            self.btn5.config(state='normal')
            self.btn6.config(state='normal',text='next')
            self.var.set("Enable welcome message in when logging in? (ON/OFF)")
        elif self.step == 5:
            self.t.delete("1.0",END)
            self.btn5.config(state='normal')
            self.btn6.config(state='normal',text='install')
            self.var.set("Confirm install")
        elif self.step == 11:
            self.t.delete("1.0",END)
            if(installation.checkinstalled("REST")):
                self.btn5.config(state='disabled')
                self.btn6.config(state='normal',text='install')
                self.var.set("Installing restG4, confirm?")
            else:
                self.btn5.config(state='disabled')
                self.btn6.config(state='disabled',text='install')
                self.var.set("REST mainbody has not been installed!")
        elif self.step == 21:
            self.t.delete("1.0",END)
            self.t.insert("1.0",vars.opt["Branch"])
            self.var.set("Updating REST, choose a branch to update")
            self.btn5.config(state='disabled')
            self.btn6.config(state='normal',text='next')
        elif self.step == 22:
            self.t.delete("1.0",END)
            self.var.set("Any local changes will be overwritten! \n Confirm update")
            self.btn5.config(state='normal')
            self.btn6.config(state='normal',text='update')
        elif self.step < 0:
            self.t.delete("1.0",END)
            self.btn5.config(state='disabled')
            self.btn6.config(state='disabled',text='next')

    def applyopt(self):
        if self.step == 1:
            if(installation.checkinstalled("REST")):
                if "Y" in self.t.get("1.0",END).strip('\n').upper():
                    self.step = 10
                    return
                elif "N" in self.t.get("1.0",END).strip('\n').upper():
                    self.step = 1
        elif self.step == 3:
            vars.opt["Install_Path"] = self.t.get("1.0",END).strip('\n')
        elif self.step == 4:
            vars.opt["DREST_WELCOME"] = self.t.get("1.0",END).strip('\n')
        elif self.step == 5:
            installation.install("REST")
            self.step = -10
            self.var.set("Completed! \n You need to source " + vars.opt["Install_Path"] + "/thisREST.sh before using it!")
        elif self.step == 11:
            if(installation.checkinstalled("REST")):
                installation.install("restG4")
                self.step = -10
                self.var.set("Completed!")
        elif self.step == 21:
            vars.opt["Branch"] = self.t.get("1.0",END).strip('\n')
        elif self.step == 22:
            vars.opt["Warning"] = "False"
            result = updateREST.main()
            self.step = -10
            self.var.set(result)






def main():
    root.title("REST Scripts")
    root.geometry('500x200')
    root.update()
    app = Application(root)
    root.mainloop()


if __name__ == '__main__':
	main()