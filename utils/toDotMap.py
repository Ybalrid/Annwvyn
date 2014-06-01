#!/bin/python
from lxml import etree
import sys

#test if sufficient number of arguments : 
if len(sys.argv) < 2:
    print("Please specify in argument a .scene file !")
    quit(-1)


#Open .scene file
file = open(str(sys.argv[1]))

#Load XML into an etree
scene = etree.XML(file.read())
file.close()

out = "" 

light = 0

#extract and print informations
for node in scene[0]:
    if node[4].tag == "light":
        light = 1
        out = "Light " + node[4].get("powerScale") 
    else:
        light = 0
        out = "Object " + node[4].get("meshFile")
    print(out)
    out = "Pos "
    out += node[0].get("x") + " "
    out += node[0].get("y") + " "
    out += node[0].get("z") + " "
    print(out)
    if light == 0:
        out = "Orent "
        out += node[1].get("qx") + " "
        out += node[1].get("qy") + " "
        out += node[1].get("qz") + " "
        out += node[1].get("qw") + " "
        print(out)
        print("PhysicShape STATIC 0")
    print("EndObject")
    print("") #I don't know Python, This is the way I found to print a newline character

quit(0)
