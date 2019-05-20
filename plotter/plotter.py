### this script takes cmd line options, opens a config,
### uses the to create and instruct the plotting class,
### and finally displays/saves the plot

import argparse
import modules.ConfigReader as cfgr
import modules.ToolsNew as Tools
import modules.SampleHist as sh
import modules.UserHistoError as uhe
from styles import plotStyles
import ROOT
import collections
import fnmatch
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True


# def parsePostiftFile(fname, chan):
#     pfyieldfile = open(fname)
#     scales = {}
#     errors = {}
#     for line in pfyieldfile:
#         tokens = line.split()
#         if tokens[0] != ('ch'+str(chan)):
#             continue
#         procname = tokens[1]
#         yprefit  = float(tokens[2])
#         ypostfit = float(tokens[8])
#         scale = ypostfit/yprefit if yprefit > 0 else 0
#         scales[procname] = scale

#         errpostfit = float(tokens[10])
#         err = errpostfit/ypostfit if ypostfit > 0 else 0
#         errors[procname] = err

#     return (scales, errors)

def check_selected_bkg(allbkg, chosenbkg):
    """ check that all the elements in allbkg appear once, and just once, in chosenbkg """
    
    chosen_list = []
    for key in chosenbkg.keys():
        chosen_list += chosenbkg[key]

    ## 1 : check for doubles
    duplicates = [item for item, count in collections.Counter(chosen_list).items() if count > 1]

    if len(duplicates) > 0:
        print "\n*** WARNING: these samples are listed multiple times to be plotted, please check if it is intentional"
        print duplicates
        print ""

    ## 2 : check for missing
    missing = [x for x in allbkg if x not in chosen_list]
    if len(missing) > 0:
        print "\n*** WARNING: these samples have been processed but not listed to be plotted, please check if it is intentional"
        print missing
        print ""

######################################################################################
######################################################################################
######################################################################################

ROOT.TH1.AddDirectory(0)

###########################################
#############   parse options  ############
###########################################

parser = argparse.ArgumentParser(description='Command line parser of plotting options')
#string opts
parser.add_argument('--var', dest='var', help='variable name', default=None)
parser.add_argument('--sel', dest='sel', help='selection name', default=None)
parser.add_argument('--dir', dest='dir', help='analysis output folder name', default="./")
parser.add_argument('--title',   dest='title',   help='plot title', default=None)
parser.add_argument('--channel', dest='channel', help='channel = (MuTau, ETau, TauTau)', default=None)
parser.add_argument('--bcateg',  dest='bcateg',  help='b category = (resbb, resbj, bstbb)', default=None)
parser.add_argument('--SRtype', dest='SRtype',   help='SR type: nonres, resLM, resHM', default=None)
parser.add_argument('--siglegextratext', dest='siglegextratext', help='additional optional text to be plotted in legend after signal block', default=None)
parser.add_argument('--channame', dest='channame', help='explicit chan name (overrides automatic creation)', default=None)

#bool opts
parser.add_argument('--log',     dest='log', help='use log scale',  action='store_true', default=False)
parser.add_argument('--logx',    dest='logx', help='use log scale on x',  action='store_true', default=False)
# parser.add_argument('--postfit', dest='postfit', help = 'scale to postfit values', action='store_true', default=False)
parser.add_argument('--no-data', dest='dodata', help='disable plotting data',  action='store_false', default=True)
parser.add_argument('--no-sig',  dest='dosig',  help='disable plotting signal',  action='store_false', default=True)
parser.add_argument('--no-legend',   dest='legend',   help = 'disable drawing legend',       action='store_false', default=True)
parser.add_argument('--div-by-binwidth', dest='binwidth', help = 'enable scaling by bin width', action='store_true', default=False)
parser.add_argument('--ratio',    dest='ratio', help = 'do ratio plot at the botton', action='store_true', default=False)
parser.add_argument('--sbplot',    dest='sbplot', help = 'do s/b plot at the botton', action='store_true', default=False)
parser.add_argument('--no-print', dest='printplot', help = 'no pdf output', action='store_false', default=True)
parser.add_argument('--root',     dest='root', help = 'print root canvas', action='store_true', default=False)
parser.add_argument('--quit',     dest='quit', help = 'quit at the end of the script, no interactive window', action='store_true', default=False)
parser.add_argument('--no-sr-namecompl',  dest='srnamecompl', help = 'complete the sel name with string SR if not specified by the user', action='store_false', default=True)
parser.add_argument('--tab',      dest='tab', help = 'print table with yields', action='store_true', default=False)
parser.add_argument('--postfit',  dest='postfit', help = 'draw postfit plots', action='store_true', default=False)


# par list opt
parser.add_argument('--blind-range',   dest='blindrange', type=float, nargs=2, help='start and end of blinding range', default=None)
# parser.add_argument('--arrow-xy',      dest='arrowxy', nargs=2, help='x and y (max) of the arrow of BDT WP', default=None)
parser.add_argument('--leg-coords',    dest='legcoords', type=float, nargs=4, help='x and y coordinates of the legend', default=None)

#float opt
parser.add_argument('--lxmin', dest='lxmin', type=float, help='legend min x position in pad fraction', default=None)
parser.add_argument('--lymin', dest='lymin', type=float, help='legend min y position in pad fraction', default=None)
parser.add_argument('--ymin', dest='ymin', type=float, help='min y range of plots', default=None)
parser.add_argument('--ymax', dest='ymax', type=float, help='max y range of plots', default=None)
parser.add_argument('--xmax', dest='xmax', type=float, help='max x range of plots', default=None)
parser.add_argument('--xmin', dest='xmin', type=float, help='min x range of plots', default=None)
parser.add_argument('--sigscale', dest='sigscale', type=float, help='scale to apply to all signals', default=None)

args = parser.parse_args()

if args.quit : ROOT.gROOT.SetBatch(True)

###########################################
#############   read configs   ############
###########################################

cfgName        = Tools.findInFolder  (args.dir+"/", '*main*.cfg')
outplotterName = Tools.findInFolder  (args.dir+"/", 'outPlotter.root')
# outplotterName = Tools.findInFolder  (args.dir+"/", 'analyzedOutPlotter.root')

cfg = cfgr.ConfigReader (args.dir + "/" + cfgName)
dataList = cfg.readListOption("general::data")
# dataList = ['data_obs'] ## data are merged by hand into a single collection
sigList  = cfg.readListOption("general::signals")
bkgList  = cfg.readListOption("general::backgrounds")

if not dataList : dataList = []
if not sigList  : sigList  = []
if not bkgList  : bkgList  = []

# the background that were merged together - either at plotting level or at postprocessings
replacements = {}
secname = 'pp_merge'
if cfg.hasSection(secname) and cfg.config[secname]:
    for name in cfg.config[secname]:
        replacements[name] = cfg.readListOption(secname+'::'+name)
secname = 'merge'
if cfg.hasSection(secname) and cfg.config[secname]:
    for name in cfg.config[secname]:
        replacements[name] = cfg.readListOption(secname+'::'+name)

for name in replacements:
    thelist = None
    if replacements[name][0] in dataList:
        thelist = dataList
    elif replacements[name][0] in bkgList:
        thelist = bkgList
    elif replacements[name][0] in sigList:
        thelist = sigList
    else:
        print "** WARNING: I cannot find the name", replacements[name][0], "to replace"
    
    for old in replacements[name]:
        thelist.remove(old)
    thelist.append(name)

# if cfg.hasSection('pp_QCD') and args.srnamecompl:
#     bkgList.append('QCD')
# else:
#     print "** Warning: No QCD section was found in mainCfg, was it computed?"

###########################################
#############  retrieve plots  ############
###########################################

rootFile = ROOT.TFile.Open (args.dir+"/"+outplotterName)
print '... opened file' , rootFile.GetName()
sel = args.sel
if not '_SR' in sel and args.srnamecompl: sel += '_SR'

hSigs    = Tools.retrieveHistos  (rootFile, sigList,  args.var, sel) #, "sig": tags are unused now
hBkgs    = Tools.retrieveHistos  (rootFile, bkgList,  args.var, sel) #, "bkg": tags are unused now
hDatas   = Tools.retrieveHistos  (rootFile, dataList, args.var, sel) #, "DATA": tags are unused now
# hQCD     = Tools.retrieveQCD     (rootFile, args.var, sel, dataList)


#### FIXME: leaving some portion of the old code as example, but most likely something different is needed for postfit
if args.postfit:
    raise RuntimeError("Postfit plots are not yet implemented")    
#     print "** INFO: going to scale to postfit yields"
#     postfitscales, postfiterrors = parsePostiftFile(pfyieldfilename, chtransl[(chnumber, categnumber)])
#     for bkg in hBkgs:
#         if bkg in postfitscales:
#             # print "** scale : " , bkg, postfitscales[bkg], ' ... ' , chtransl[(chnumber, categnumber)], chnumber, categnumber
#             hBkgs[bkg].Scale(postfitscales[bkg])
#         else:
#             print " ** warning: no postfit scale for ", bkg


## scales to apply to the histos to fix the missing pieces of the xs
scales = {
    'VBF_HH'                :  0.00055169,
    'VBF_HH_BSM'            :  0.0127212361335*0.58*0.58,
    'GGF_HH'                 :  0.01125258,
    'QCD_HT_100_200'        :  27990000,
    'QCD_HT_200_300'        :  1547000,
    'QCD_HT_300_500'        :  322600,
    'QCD_HT_500_700'        :  29980,
    'QCD_HT_700_1000'       :  6334,
    'QCD_HT_1000_1500'      :  1088 ,
    'QCD_HT_1500_2000'      :  99.11,
    'QCD_HT_2000_Inf'       :  20.23,
    'TTToSemiLeptonic'      :  365.34,
    'TTToHadronic'          :  377.96,
    'TTTo2L2Nu'             :  88.29,  
}

### scale the histograms by their specified scale
## FIXME: to remove as soon as xs propagated to the event weight
for hbg in hBkgs.keys():
    if hbg in scales:
        print  " >>> Scaling {:30} by   {}".format(hbg, scales[hbg])
        hBkgs[hbg].Scale(scales[hbg])

for hbg in hSigs.keys():
    if hbg in scales:
        print  " >>> Scaling {:30} by   {}".format(hbg, scales[hbg])
        hSigs[hbg].Scale(scales[hbg])


###########################################
########  retrieve/compute errors  ########
###########################################

# print 'Cannot state channel type ' , args.channel, ' --> no way to determine errors, using dummy cfg'
systFile = 'userSystCfgExample.cfg'
print  "... evaluating the bkg syst errors for the envelope from file", systFile
uh = uhe.UserHistoError(systFile)

## FIXME: uh.histos currently using hBkgs, i.e. all the bkgs processed and not the restricted list
## however the code checks that all bks are being used with check_selected_bkg
## restriction to implement when the 
uh.histos = dict (hBkgs)
if args.postfit:
  uh.externalDict = dict(postfiterrors)
histoErr = uh.getErrorEnvelope()

########## Signal scales ##########
## you can define a dictionary that rescales by the specifies value a specific signal process
## note that this is applied independently from the global "sigscale" passed to the SampleHist class
## FIXME : the two scalings can be harmonised in a single place

sigscales = {}

###########################################
############  read the styles  ############
###########################################


linestyles = dict(plotStyles.linestyles)
linecolors = dict(plotStyles.linecolors)
fillcolors = dict(plotStyles.fillcolors)
titles     = dict(plotStyles.titles)

######### Things to plot ##################
bkgToPlot = collections.OrderedDict()
bkgToPlot['TT']      = ['TTTo2L2Nu', 'TTToHadronic', 'TTToSemiLeptonic']
bkgToPlot['QCD']     = ['QCD_HT_100_200', 'QCD_HT_200_300', 'QCD_HT_300_500', 'QCD_HT_500_700', 'QCD_HT_700_1000', 'QCD_HT_1000_1500', 'QCD_HT_1500_2000', 'QCD_HT_2000_Inf']

### decide what to plot - a plain list of signals
sigToPlot = ["GGF_HH", "VBF_HH", "VBF_HH_BSM"]


# bkgToPlot['QCD_HT_100_200']   = ['QCD_HT_100_200']
# bkgToPlot['QCD_HT_200_300']   = ['QCD_HT_200_300']
# bkgToPlot['QCD_HT_300_500']   = ['QCD_HT_300_500']
# bkgToPlot['QCD_HT_500_700']   = ['QCD_HT_500_700']
# bkgToPlot['QCD_HT_700_1000']  = ['QCD_HT_700_1000']
# bkgToPlot['QCD_HT_1000_1500'] = ['QCD_HT_1000_1500']
# bkgToPlot['QCD_HT_1500_2000'] = ['QCD_HT_1500_2000']
# bkgToPlot['QCD_HT_2000_Inf']  = ['QCD_HT_2000_Inf']

### check that I am not forgetting any background
check_selected_bkg(allbkg = bkgList, chosenbkg=bkgToPlot)


###########################################
############  prepare plotter  ############
###########################################

shc = sh.SampleHistColl()
shc.stackErrorHist = histoErr
shc.logy       = args.log
shc.logx       = args.logx
shc.ratio      = args.ratio
shc.sbplot     = args.sbplot
shc.lumi       = float(cfg.readOption ("general::lumi"))/1000. # from pb to fb
shc.chan       = 'HH #rightarrow bbbb'
shc.title      = args.title
shc.divByBW    = args.binwidth
shc.plotData   = args.dodata
shc.plotSig    = args.dosig
shc.plotLegend = args.legend
shc.ymin       = args.ymin
shc.ymax       = args.ymax
shc.xmin       = args.xmin
shc.xmax       = args.xmax
shc.legxmin    = args.lxmin
shc.legymin    = args.lymin
shc.legcoords  = None if not args.legcoords else list([args.legcoords[0], args.legcoords[1], args.legcoords[2], args.legcoords[3]])
shc.siglegextratext = args.siglegextratext
shc.blindrange = None if not args.blindrange else list([args.blindrange[0], args.blindrange[1]])
shc.linecolors = dict(linecolors)
shc.fillcolors = dict(fillcolors)
shc.linestyles = dict(linestyles)
shc.sigscale   = args.sigscale
# shc.legtextsize  = 23
shc.legtextsize  = 19
shc.chantextsize  = 23
shc.cmstextinframe = False
shc.ispreliminary = True
shc.ytitleoffset  = 1.3
shc.lumiformat = '{:.1f}'

## added 18/ago/2017 for BDT plots
shc.ratioxtitleoffset = 3.5
shc.lowerpadbottommargin = 0.45


for sig in sigToPlot:
    if not sig in hSigs:
        print '** Warning: signal' , sig , ' not found in the input, removing from plotting list...'
sigToPlot[:] = [x for x in sigToPlot if x in hSigs]

# scale to expected sigscale before. NOTE: a second sigscale can be applied in the plotter
for h in hSigs:
    match = [key for key in sigscales if fnmatch.fnmatch(h, key)]
    if len(match) == 1:
        print " >> info: histo of sample", h, ('scaled by factor %.3g' % sigscales[match[0]])
        hSigs[h].Scale(sigscales[match[0]])
    shc.addHisto (hSigs[h], h, 'sig', (titles[h] if h in titles else hSigs[h].GetName()))

# for h in hBkgs:
for hname in bkgToPlot:
    for h in bkgToPlot[hname]:
        shc.addHisto (hBkgs[h], hname, 'bkg', (titles[hname] if hname in titles else hBkgs[h].GetName()))
for h in hDatas:
    shc.addHisto (hDatas[h], h, 'data', 'Data')
## mu, e, for pre-mass cut
# sigNameList = ["k_{#lambda} = 1 (SM) #times 5000"]
# sigScale = [5000.*(0.073/1.)*33.45/1000.] # tutto a 1 pb di rpoduzione hh (tolgo il BR in bbtautau)
shc.setListToPlot(bkgToPlot.keys(), 'bkg')
shc.setListToPlot(reversed(sigToPlot), 'sig') # sigs instead are listed from top to bottom of the legend
shc.setListToPlot(dataList, 'data')

shc.makePlot()
shc.c1.cd()
shc.c1.Update()

if args.tab: shc.printTable(floatFormat='.6', printMCstat=True)

if not args.quit:
    raw_input()

if args.printplot:
    # outname = '_'.join(['plot', args.var, args.sel, args.channel])
    outname = '_'.join(['plots/plot', args.var, args.sel])
    if args.log: outname += ('_'+'log')
    if args.postfit: outname += '_postfit'
    outname += '.pdf'
    # print outname
    shc.c1.Print(outname, 'pdf')

if args.root:
    shc.c1.SaveAs(rootname)
