import ROOT
import argparse
import collections
import fnmatch
from  ConfigParser import *
import ast
import os
from subprocess import Popen

def get_histoname(proc, selection, var):
    # QCD_baseline_SR_BDTG_nomasscut
    hname = '{name}_{sel}_{var}'.format(name=proc, sel=selection, var=var)
    # hname = '{}_{}_{}'.format(proc, selection, var)
    return hname

def get_histo (fIn, proc, selection, var):
    # print "call get histo on fIn={}. proc={}. selection={}, var={}".format()
    hname = get_histoname(proc, selection, var)
    # if not fIn.GetListOfKeys().Contains(hname):
    h = fIn.Get(hname)
    if not h:
        raise RuntimeError("histo not found : %s" % hname)
    return h

def writeln (f, txt):
    f.write(txt + '\n')

def writelnarr(f, txtarr, align='{:>25}', firstAlign = '{:<35}', secondAlign = '{:<6}', addEmptyAtIdx = None):
    if addEmptyAtIdx:
        txtarr = txtarr[0:addEmptyAtIdx] + [''] + txtarr[addEmptyAtIdx:]
    str_proto = ''
    for idx, txt in enumerate(txtarr):
        str_proto += '%s ' % (firstAlign if idx == 0 else secondAlign if idx == 1 else align)
    str_proto = str_proto[:-1] ## remove trailing space
    the_str = str_proto.format(*txtarr)
    writeln(f, the_str)

def affects_any(syst, systlist, procs):
    sytype = systs[syst][0]
    elemts = systs[syst][1:]
    for proc in procs: ## loop through the datacard columns
        iaffect = []
        for idx, el in enumerate(elemts):
            if fnmatch.fnmatch(proc, el[0]):
                return True
    return False

def readShapeFromAltHistos(root_inputfile, nominalname, systname, systformat='{nominalname}_{syst}'):
    
    name_nom = nominalname
    name_dn  = systformat.format(nominalname=nominalname, syst=systname+'Down')
    name_up  = systformat.format(nominalname=nominalname, syst=systname+'Up')


    h_nom = root_inputfile.Get(name_nom)
    h_dn  = root_inputfile.Get(name_dn)
    h_up  = root_inputfile.Get(name_up)

    if not h_nom : raise RuntimeError("histogram %s not found" % name_nom)
    if not h_dn  : raise RuntimeError("histogram %s not found" % name_dn)
    if not h_up  : raise RuntimeError("histogram %s not found" % name_up)

    y_nom = 1. * h_nom.Integral()
    y_dn  = 1. * h_dn.Integral()
    y_up  = 1. * h_up.Integral()

    if y_nom <= 0:
        return (0, 0)

    var_dn = y_dn / y_nom
    var_up = y_up / y_nom

    return (var_dn, var_up)


########################
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config',    dest='cfgfile',        help='Name of config file with MC information',   required = True)
args = parser.parse_args()
configfilename = args.cfgfile

print "[INFO] Reading configuration file "
cfgparser = ConfigParser()
cfgparser.optionxform = lambda option: option # preserve lower-upper case
cfgparser.read('%s'%configfilename)

categandobs = ast.literal_eval(cfgparser.get("configuration","categandobs"))
##########Get variables for datacard preparation
folder       = ast.literal_eval(cfgparser.get("configuration","folder"))
datas        = ast.literal_eval(cfgparser.get("configuration","datas"))
dsetname     = ast.literal_eval(cfgparser.get("configuration","dataset"))
card_name    = "datacard%s"%dsetname
categories   = [categandobs[k][0] for k in range(0,len(categandobs)) ]
combinations = ast.literal_eval(cfgparser.get("configuration","combinations"))
datacards    = []
datasets     = ast.literal_eval(cfgparser.get("configuration","datasets"))

os.system("mkdir %s"%folder)
#Get into the main folder
os.chdir("%s/%s"%(os.getcwd(),folder))
#Make datacards, one per category
print "[INFO] Making datacards: "
for categ in categories:
    #######################
    fname = "%s_%s.txt"%(card_name,categ)

    cstr="python ../../../../HiggsAnalysis/CombinedLimit/scripts/combineCards.py "
    outc=""
    for dat in datasets:
        cstr+="%s_%i=../physicsmodel%i/datacard%s_%s.txt "%(categ,dat,dat,dat,categ)
    cstr+="> %s_%s.txt"%(card_name,categ)
    os.system('%s &'%cstr)
    datacards.append("%s_%s.txt"%(card_name,categ))
    print " - %s/%s "%(folder,fname)

#Make combined datacards
print "[INFO] Making combined datacards: "
for comb in combinations:
    combstr="python ../../../../HiggsAnalysis/CombinedLimit/scripts/combineCards.py "
    outcomb=""
    for k in  range(0,len(comb)):
      for dat in datasets:
        combstr+="%s_%i=../physicsmodel%i/datacard%s_%s.txt "%(comb[k],dat,dat,dat,comb[k])
      outcomb+="%s"%comb[k]
    combstr+="> %s_comb_%s.txt"%(card_name,outcomb)
    os.system('%s &'%combstr)
    datacards.append("%s_comb_%s.txt"%(card_name,outcomb)) 
    print " - %s/%s_comb_%s.txt"%(folder,card_name,outcomb)

##Make workspace
print "[INFO] Making workspace for all datacards using HHModelPinv"
if datas[0] != 'data_obs':
    #for card in datacards: os.system('text2workspace.py %s -D %s -P HHModel:HHdefault &'%(card, datas[0]))
    for card in datacards: os.system('text2workspace.py %s -D %s -P HHModelPinv:HHdefault --PO doNNLOscaling=false &'%(card, datas[0]))
else:
    #for card in datacards: os.system('text2workspace.py %s -P HHModel:HHdefault &'%card)
    for card in datacards: os.system('text2workspace.py %s -P HHModelPinv:HHdefault --PO doNNLOscaling=false &'%card)