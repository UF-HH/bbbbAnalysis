import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

### add here the data to be plotted
### the data are plotted top to bottom
### values are pasted in the same order as they are printed by combine (to make things a bit easier)

plot_data = [
    {
        'type'        : 'UL',
        'label'       : 'ggF Low m_{HH}',
        'annotations' : ['Obs: 23.4', 'Exp: 25.1'],
        'obs'         : 23.3861,
        'exp -2sigma' : 13.1514,
        'exp -1sigma' : 17.7350,
        'exp median'  : 25.1250,
        'exp +1sigma' : 36.4419,
        'exp +2sigma' : 51.5828,
    },
    {
        'type'        : 'UL',
        'label'       : 'ggF High m_{HH}',
        'annotations' : ['Obs: 4.0', 'Exp: 7.7'],
        'obs'         : 3.9821,
        'exp -2sigma' : 4.1006,
        'exp -1sigma' : 5.4857,
        'exp median'  : 7.7188,
        'exp +1sigma' : 10.9801,
        'exp +2sigma' : 15.1277,
    },
    {
        'type'        : 'UL',
        'label'       : 'VBF categories',
        'annotations' : ['Obs: 43.2', 'Exp: 70.8'],
        'obs'         : 43.1941,
        'exp -2sigma' : 37.0332,
        'exp -1sigma' : 49.6770,
        'exp median'  : 70.7500,
        'exp +1sigma' : 103.1814,
        'exp +2sigma' : 145.6034,
    },
    #####################
    {
        'type'        : 'hline',
        'style'       : 1,
        'width'       : 2,
    },
    #####################
    {
        'type'        : 'UL',
        'label'       : 'Combined',
        'annotations' : ['Obs: 3.7', 'Exp: 7.3'],
        'obs'         : 3.6585,
        'exp -2sigma' : 3.8682,
        'exp -1sigma' : 5.2014,
        'exp median'  : 7.2812,
        'exp +1sigma' : 10.4158,
        'exp +2sigma' : 14.3090,
    },
]




## get the range of the plot
## FIXME: can be done automatically from the data, now done by hand

xmin = 0
xmax = 150
xtitle = '#mu = #sigma_{HH}/#sigma_{HH}^{SM}'
whiteborderfraction = 0.1 # how much space between horizontal bars
labelalign = "R" # L or R
oname = 'SMplotbycateg.pdf'
vcenter_annotated = True

###
textfont = 43
textsize = 18
labelsize = 16
vadjust  = 0.05 # overall vertical shift (upwards) - in fraction of  band
canvunits = 150 # height of a band - relative to 600 in width

## determine canvas aspect ratio - every "band" will have the same height, making plot longer
nULs = sum([1 for x in plot_data if x['type'] == 'UL'])
print('.. will plot', nULs, 'upper limits')
## override to do manually

# c1 = ROOT.TCanvas('c1', 'c1', 600, canvunits*nULs)
# c1.SetLeftMargin(0.3) # large, for labels
# c1.SetBottomMargin(0.15) # large, for labels

xwcanv = 600
ywcanv = canvunits*nULs
c1 = ROOT.TCanvas('c1', 'c1', xwcanv, ywcanv)

## get a constant border: it is 180 for 600 (left) and 90 for bottom, 60 for top/right

c1.SetLeftMargin(140./xwcanv) # large, for labels
c1.SetBottomMargin(90./xwcanv) # large, for labels
c1.SetRightMargin(60./xwcanv) # large, for labels
c1.SetTopMargin(60./xwcanv) # large, for labels

frame = ROOT.TH1D('frame', ';%s;'%xtitle, 100, xmin, xmax)

## in frame units, every band takes 1 unit
ymin  = 0+vadjust
ymax  = nULs+vadjust

## build the points of the graph
gr_obs    = ROOT.TGraphAsymmErrors()
gr_exp    = ROOT.TGraphAsymmErrors()
gr_1sigma = ROOT.TGraphAsymmErrors()
gr_2sigma = ROOT.TGraphAsymmErrors()

hlines = []
labels = []

def add_point(desc, ipoint):
    ybar = 0.5-0.5*whiteborderfraction
    ypos = nULs - 1.*ipoint - 0.5+vadjust  ## top to bottom
    annot_yshift = 0.15

    exp = desc['exp median']
    obs = desc['obs']

    e_1s_up = desc['exp +1sigma'] - exp
    e_1s_do = exp - desc['exp -1sigma']

    e_2s_up = desc['exp +2sigma'] - exp
    e_2s_do = exp - desc['exp -2sigma']

    gr_obs.SetPoint(ipoint, obs, ypos)
    gr_exp.SetPoint(ipoint, exp, ypos)
    gr_1sigma.SetPoint(ipoint, exp, ypos)
    gr_2sigma.SetPoint(ipoint, exp, ypos)

    gr_obs.SetPointError(ipoint, 0, 0, ybar, ybar)
    gr_exp.SetPointError(ipoint, 0, 0, ybar, ybar)
    gr_1sigma.SetPointError(ipoint, e_1s_do, e_1s_up, ybar, ybar)
    gr_2sigma.SetPointError(ipoint, e_2s_do, e_2s_up, ybar, ybar)

    # now make the label for this
    if labelalign == "L":
        xlabel = 0 - 0.4*(xmax-xmin)
        align = 12
    if labelalign == "C":
        xlabel = 0 - 0.3*(xmax-xmin)
        align = 22
    if labelalign == "R":
        xlabel = 0 - 0.05*(xmax-xmin)
        align = 32
    
    ypos_lab = ypos
    if vcenter_annotated and 'annotations' in desc:
        ypos_lab += 0.5*(annot_yshift * len(desc['annotations']))

    l = ROOT.TLatex(xlabel, ypos_lab, desc['label'])
    l.SetTextAlign(align)
    l.SetTextFont(textfont)
    l.SetTextSize(textsize)
    labels.append(l)

    if 'annotations' in desc:
        for iann, ann in enumerate(desc['annotations']):
            ylabel = ypos_lab - (iann+1)*annot_yshift -0.1
            l = ROOT.TLatex(xlabel, ylabel, ann)
            l.SetTextAlign(align)
            l.SetTextFont(textfont)
            l.SetTextSize(0.75*textsize)
            labels.append(l)

def make_yline(desc, ipoint):
    ypos = ypos = nULs - 1.*ipoint+vadjust
    l = ROOT.TLine(xmin, ypos, xmax, ypos)
    l.SetLineStyle(desc['style'])
    l.SetLineWidth(desc['width'])
    l.SetLineColor(ROOT.kBlack)
    return l

idx_UL = 0
for d in plot_data:
    if d['type'] == 'UL':
        add_point(d, idx_UL)
        idx_UL += 1
    if d['type'] == 'hline':
        l = make_yline(d, idx_UL)
        hlines.append(l)

##### set the graoh styles

gr_exp.SetMarkerStyle(24)
gr_exp.SetMarkerColor(4)
gr_exp.SetMarkerSize(0.8)
gr_exp.SetLineColor(ROOT.kBlue+2)
gr_exp.SetLineWidth(2)
gr_exp.SetLineStyle(2)
gr_exp.SetFillColor(0)

gr_obs.SetLineColor(1)
gr_obs.SetLineWidth(2)
gr_obs.SetMarkerColor(1)
gr_obs.SetMarkerStyle(20)
gr_obs.SetFillStyle(0)

gr_1sigma.SetMarkerStyle(0)
gr_1sigma.SetMarkerColor(3)
gr_1sigma.SetFillColor(ROOT.kGreen+1)
gr_1sigma.SetLineColor(ROOT.kGreen+1)
gr_1sigma.SetFillStyle(1001)

gr_2sigma.SetMarkerStyle(0)
gr_2sigma.SetMarkerColor(5)
gr_2sigma.SetFillColor(ROOT.kOrange)
gr_2sigma.SetLineColor(ROOT.kOrange)
gr_2sigma.SetFillStyle(1001)


#### adjust fonts etc
frame.GetYaxis().SetTitleSize(0)
frame.GetYaxis().SetNdivisions(0)
frame.GetXaxis().SetTitleFont(textfont)
frame.GetXaxis().SetTitleSize(textsize)
frame.GetXaxis().SetLabelFont(textfont)
frame.GetXaxis().SetLabelSize(labelsize)
frame.GetYaxis().SetNdivisions(0)
frame.GetXaxis().SetTitleOffset(1.25)
frame.SetMinimum(0)
frame.SetMaximum(ymax)


# legend
legend = ROOT.TLegend(0,0,0,0)
legend.SetTextFont(43)
legend.SetTextSize(15)
legend.SetX1(0.62)
legend.SetY1(0.68)
legend.SetX2(0.93)
legend.SetY2(0.84)
legend.SetFillColor(ROOT.kWhite)
legend.SetBorderSize(0)
legend.SetHeader('95% CL upper limits')
legend.AddEntry(gr_obs,"Observed","pl")
legend.AddEntry(gr_exp, "Median expected", "pl")
legend.AddEntry(gr_1sigma, "68% expected", "f")
legend.AddEntry(gr_2sigma, "95% expected", "f")

##channel
pt4 = ROOT.TPaveText(0.624,0.85,0.744,0.89,"brNDC")
#pt4.SetTextAlign(12)
pt4.SetFillColor(ROOT.kWhite)
pt4.SetFillStyle(1001)
pt4.SetTextFont(43)
pt4.SetTextSize(15)
pt4.SetBorderSize(0)
pt4.SetTextAlign(32)
# pt4.AddText(channelsName[ic]) 
pt4.AddText("HH#rightarrowbbbb") 

## now plot everything
frame.Draw()
gr_2sigma.Draw('E2same')
gr_1sigma.Draw('E2same')
gr_exp.Draw("Pz same")
gr_obs.Draw("Pz same")
legend.Draw()
pt4.Draw()

for l in hlines:
    l.Draw()

for l in labels:
    l.Draw()

### labels for CMS Preliminary and sqrt


def redrawBorder():
   # this little macro redraws the axis tick marks and the pad border lines.
   ROOT.gPad.Update();
   ROOT.gPad.RedrawAxis();
   l = ROOT.TLine()
   l.SetLineWidth(3)
   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymax(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymax());
   l.DrawLine(ROOT.gPad.GetUxmin(), ROOT.gPad.GetUymin(), ROOT.gPad.GetUxmax(), ROOT.gPad.GetUymin());

def make_texts():
    ypos = ymax + 0.05
    CMS_text = ROOT.TLatex(0, ypos, 'CMS')
    CMS_text.SetTextFont(63)
    CMS_text.SetTextSize(textsize)
    CMS_text.SetTextAlign(11)
    
    prelim_text = ROOT.TLatex(0 + 0.12*(xmax-xmin), ypos, 'Preliminary')
    prelim_text.SetTextFont(53)
    prelim_text.SetTextSize(textsize)
    prelim_text.SetTextAlign(11)

    sqrte_text = ROOT.TLatex(xmax, ypos, '13 TeV')
    sqrte_text.SetTextFont(43)
    sqrte_text.SetTextSize(textsize)
    sqrte_text.SetTextAlign(31)
    
    tts = [CMS_text, prelim_text, sqrte_text]
    return tts

tt = make_texts()
for t in tt:
    t.Draw()

print('\ngr_2sigma')
gr_2sigma.Print()
print('\ngr_1sigma')
gr_1sigma.Print()
print('\ngr_exp')
gr_exp.Print()
print('\ngr_obs')
gr_obs.Print()

redrawBorder()
c1.Update()


c1.Print(oname, 'pdf')