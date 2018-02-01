#### a script that generates the code that defines the member variables of NanoAODTree_SetBranchImpl
# practical to use as both MakeSelector nor MakeClass are capable of making this kind of code

import ROOT
import os
import sys
import fnmatch

if len (sys.argv) < 2:
    print "please provide a file"
    sys.exit()

fname = sys.argv[1]
oname = sys.argv[2] if len(sys.argv) > 2 else None

## the parrents to skip - do not create anything for these
patt_to_skip = [
    "HLT_*",
    "Flag_*",
    "L1simulation_step",
    "HLTriggerFirstPath",
    "HLTriggerFinalPath"
]

## ask root to generate a MakeClass, so I can easily get the types
fscript = open('the_generate_script.txt', 'w')
fscript.write('Events->MakeClass("the_generate_class")\n')
fscript.write('.q\n')
fscript.close()
os.system("root -l %s < the_generate_script.txt" % fname )
os.system('rm the_generate_script.txt')

## parse the MakeClass .h file and redefine as I need the branches
lines = []

print "... parsing the .h file"
fMakeClass = open('the_generate_class.h')
started = False
for l in fMakeClass:
    if r'// Declaration of leaf types' in l: # this comment marks the beginning of the declaration block
        started = True
        continue
    if r'// List of branches' in l: # this comment marks the end of the declaration
        break
    
    if started: # I am parsing the vars
        l = l.strip()
        if not l: continue
        tokens = l.split()
        tipo = tokens[0]
        var  = tokens[1].replace(';', '') # no semicolon at the end
        isArray = True if '[' in var and ']' in var else False
        if isArray:
            var = var.split('[', 1)[0] ## trim everything in the parenthesis

        to_skip = False
        for pts in patt_to_skip:
            to_skip = (to_skip or fnmatch.fnmatch(var, pts))
        if to_skip:
            continue

        lines.append((tipo, var, isArray))
fMakeClass.close()
os.system('rm the_generate_class.h')
os.system('rm the_generate_class.C')


## build the output

## make a pass to get the column size
cols0 = [len(l[0]) for l in lines] 
cols1 = [len(l[1]) for l in lines]
maxcol0 = max(cols0)
maxcol1 = max(cols1)

if oname:
    ofile = open(oname, 'w')

val_proto = 'ReaderValuePatch<{0}> {2} {1} {3} {{tree_, "{1}"}};'
arr_proto = 'ReaderArrayPatch<{0}> {3} {1} {4} {{tree_, "{1}", {2}, 50}};'
for l in lines:
    
    filler0 = ' ' * (maxcol0 - len(l[0]) + 1)
    filler1 = ' ' * (maxcol1 - len(l[1]) + 1)
    
    if not l[2]:
        oline = val_proto.format(l[0], l[1], filler0, filler1)
    else:
        ## I need to look for the size branch. all are in the format n<NOME>_...
        sbname = 'n' + l[1].split('_',1)[0]
        oline  = arr_proto.format(l[0], l[1], sbname, filler0, filler1)

    if not oname:
        print oline
    else:
        ofile.write(oline + '\n')

if oname:
    ofile.close()

