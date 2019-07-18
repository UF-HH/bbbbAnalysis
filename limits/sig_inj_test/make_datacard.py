import ROOT
import argparse
import collections
import fnmatch

def get_histoname(proc, selection, var):
    # QCD_baseline_SR_BDTG_nomasscut
    hname = '{name}/{sel}/{name}_{sel}_{var}'.format(name=proc, sel=selection, var=var)
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

def writelnarr(f, txtarr, align='{:>12}', firstAlign = '{:<12}', secondAlign = '{:<6}', addEmptyAtIdx = None):
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

########################

parser = argparse.ArgumentParser(description='Command line parser of plotting options')
parser.add_argument('--var',     dest='var',     help='variable name',     default = 'BDTG_nomasscut')
parser.add_argument('--sel',     dest='sel',     help='selection name',    default = 'baseline_SR')
parser.add_argument('--fileIn',  dest='filein',  help='input file',        default = '', required=True)
parser.add_argument('--cardOut', dest='cardout', help='output card name',  default = 'cards/datacard.txt')
parser.add_argument('--sigs',    dest='sigs',    help='list of signals',   nargs='+', default = ['GGF_HH'])
parser.add_argument('--bkgs',    dest='bkgs',    help='list of backgrounds', nargs='+', default = [])
parser.add_argument('--dataset', dest='dataset', help='name of obs dataset', default = "DATA")
# parser.add_argument('--QCDsyst', dest='QCDsyst', help='sigmas for the QCD syst',  default = None)
args = parser.parse_args()

fIn_name = args.filein
fIn = ROOT.TFile.Open(fIn_name)

# sigs  = ['HH']
sigs  = list(args.sigs)
# bkgs  = ['QCD', 'ttbar', 'ttH', 'ZH', 'WH', 'VBFH', 'ggH']
bkgs = ['MODEL']
if args.bkgs: bkgs = list(args.bkgs)
dsetname = args.dataset

# all_proc = sigs + bkgs ## everything that goes in the card

selection = args.sel
final_var = args.var

card_name = args.cardout

## SYSTEMATICS - list the name and the process affected by how much
## processes are matched with wildcards
systs = collections.OrderedDict()
# first element of the list : type
# second+ element of the list :  (process affected, value)
# systs['lumi'] = ['lnN', ('*', '1.015')]
# systs['btag'] = ['lnN', ('*', '1.06')]
# if args.QCDsyst: systs['QCDbin'] = ['shape', ('QCD', args.QCDsyst)]
systs['lumi'    ] = ['lnN' , ('GGF_HH', '1.025')]
systs['bTag'    ] = ['lnN' , ('GGF_HH', '1.086')]
systs['JER'     ] = ['lnN' , ('GGF_HH', '1.021')]
systs['JEC'     ] = ['lnN' , ('GGF_HH', '1.029')]
systs['trigger' ] = ['lnN' , ('GGF_HH', '1.090')]
systs['PDF'     ] = ['lnN' , ('GGF_HH', '1.035')]
systs['xS'      ] = ['lnN' , ('GGF_HH', '1.050')]
systs['xB'      ] = ['lnN' , ('MODEL*', '1.050')]

#######################
print "** input file:  ", fIn_name
print "** output card: ", card_name
print "** selection:   ", selection
print "** variable:    ", final_var
print "** signals:     ", sigs
print "** backgrounds: ", bkgs
print "** obs dataset: ", dsetname
#######################

fcard = open(card_name, 'w')

########################

writeln    (fcard, '# Card created for sel = {}, var = {}'.format(selection, final_var))
writeln    (fcard, 'imax *  number of channels')
writeln    (fcard, 'jmax *  number of backgrounds')
writeln    (fcard, 'kmax *  number of nuisance parameters (sources of systematical uncertainties)')
writeln    (fcard, '----------------------------------------------------------------------------------------------------------------------------------')
## shapes
hname = get_histoname('$PROCESS', selection, final_var)
writeln    (fcard, 'shapes *   bin1 {} {}'.format(fIn_name, hname))
# if args.QCDsyst: writeln    (fcard, 'shapes QCD bin1 {} {} {}'.format(fIn_name, hname, hname + "_$SYSTEMATIC"))
writeln    (fcard, '----------------------------------------------------------------------------------------------------------------------------------')
## observation
writelnarr (fcard, ('bin', 'bin1'))
hdata = get_histo(fIn, dsetname, selection, final_var)
writelnarr (fcard, ('observation', '%.0f' % hdata.Integral()))
writeln    (fcard, '----------------------------------------------------------------------------------------------------------------------------------')
writeln    (fcard, '# list the expected events for signal and all backgrounds in that bin')
writeln    (fcard, '# the second process line must have a positive number for backgrounds, and 0 or neg for signal')

nsig = len(sigs)
nbkg = len(bkgs)
ntot = nsig + nbkg

writelnarr (fcard, ['bin'] + (['bin1',]*ntot) , addEmptyAtIdx=1)
writelnarr (fcard, ['process'] + sigs + bkgs ,  addEmptyAtIdx=1)
writelnarr (fcard, ['process'] + range(-len(sigs)+1, 1) + range(1, len(bkgs)+1), addEmptyAtIdx=1)
writelnarr (fcard, ['rate']
                + ["%.6f" % get_histo(fIn, hname, selection, final_var).Integral() for hname in sigs]
                + ["%.6f" % get_histo(fIn, hname, selection, final_var).Integral() for hname in bkgs], addEmptyAtIdx=1)
writeln    (fcard, '----------------------------------------------------------------------------------------------------------------------------------')

for syst_source in systs.keys():
    processes = []
    sytype = systs[syst_source][0]
    elemts = systs[syst_source][1:]
    
    if not affects_any(syst_source, systs, sigs + bkgs):
        continue

    line_tokens = [syst_source, sytype]

    for proc in sigs + bkgs: ## loop through the datacard columns
        iaffect = []
        for idx, el in enumerate(elemts):
            if fnmatch.fnmatch(proc, el[0]):
                iaffect.append(idx)
        ## multiple assignments?
        if len(iaffect) == 0:
            line_tokens.append('-')
        else:
            if len(iaffect) > 1:
                print ">> [WARNING] << process", proc, "matches twice the syst source", syst_source, ', using the 1st occurrence'
            the_idx = iaffect[0]
            line_tokens.append(elemts[the_idx][1])
    writelnarr (fcard, line_tokens)
