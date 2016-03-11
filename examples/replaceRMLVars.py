#!/usr/bin/python3

import sys, os

#REST_NEVENTS
#REST_VOLUME
#REST_ISOTOPE
#REST_GEOMETRY_TYPE
#REST_GEOMETRY_SETUP
#REST_MAXSTEPSIZE
#REST_FULLCHAIN
#REST_EMIN
#REST_EMAX


def wrong(info):
    print("usage: fillRmlVars input_file [output_file] [REST_ISOTOPE=Bi214,REST_NEVENTS=100]")
    print(sys.argv)
    print("help information was printed because: {0}".format(info))
    exit(1)

def parseArg():

    inp = ''
    out = ''
    p = ''
    if len(sys.argv) == 3:
        inp = sys.argv[1]
        out = sys.argv[1]
        p = sys.argv[2]
    elif len(sys.argv) == 4:
        inp = sys.argv[1]
        out = sys.argv[2]
        p = sys.argv[3]
    else:
        wrong("wrong arg number {0}".format(len(sys.argv)))

    p = p.replace(' ','');
    if p[0] != '[': wrong("start with [ please"); 
    if p [-1] != ']': wrong("end with ] please");
    p = p[1:-1]  #remove braket
    
    pairs = p.split(';')
    
    froms = []
    tos = []
    for pair in pairs:
        split_pair = pair.split('=')
        if len(split_pair) != 2: wrong("use = please")

        froms.append('{'+split_pair[0]+'}')
        tos.append(split_pair[1])


    return inp, out, froms, tos

def main():

    
    inp_name, out_name, froms, tos = parseArg();
    #print("input : " inp_name)
    #print("output: " out_name)
    #print("froms : ", froms)
    #print("tos   : ", tos)
    inp = open(inp_name)
    text = inp.read()

    for i in range(0, len(froms)):
        text = text.replace(froms[i], tos[i]);

    #print(text)
    out = open(out_name, 'w')
    out.write(text);

main()




