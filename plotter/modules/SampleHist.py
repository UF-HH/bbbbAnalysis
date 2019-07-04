# import collections
import math
import ROOT
from prettytable import PrettyTable
# for the prettytable library: https://pypi.python.org/pypi/PrettyTable#downloads; then run command: sudo python setup.py install 

class SampleHist:
    """ 
    contains all information about the histogram of a single sample
    including input histogram, bin-width-scaled histogram, normlized histogram etc
    """
    def __init__ (self, name='sampleName', title='histoTitle', htype='bkg', inputH=None):
        self.type        = htype
        self.name        = name
        self.title       = title # will appear in legend
        
        self.evtHist     = None ## histogram with events
        self.bwHist      = None ## histogram with events, scaled by bw        
        # shapebwHist = None ## histogram, scaled by bw and normalized to unit area for shapes
        
        if inputH:
            self.evtHist = inputH
            self.bwHist  = self.evtHist.Clone(self.evtHist.GetName()+"_bw")
            self.bwHist.Scale(1., "width")

    def sumFromList (self, inputList, name=None):
        """ the existing sampleHist is re-created as the sum of SampleHists in the list """

        # if not isinstance(inputList, list):
        #     raise TypeError ("** SampleHist : createFromListSum : you need to pass an input list")
        if len(inputList) == 0:
            raise ValueError ("** SampleHist : createFromListSum : input list has size 0")

        if name: ## if no name in input, leave it with its current name
            self.name = name
        
        self.evtHist = inputList[0].evtHist
        for idx in range(1, len(inputList)):
            self.evtHist.Add(inputList[idx].evtHist)
        self.bwHist  = self.evtHist.Clone(self.evtHist.GetName()+"_bw")
        self.bwHist.Scale(1., "width")

    def addHisto(self, newHisto):
        """ add a new histogram to the current one. newHisto is an event histogram """
        tmp = newHisto.Clone('TMP'+self.evtHist.GetName())
        self.evtHist.Add(tmp)
        tmp.Scale(1., "width")
        self.bwHist.Add(tmp)

    def getHisto(self, bw=False):
        if bw:
            return self.bwHist
        else:
            return self.evtHist

    def getGraph(self, bw=False, grass=False):
        if bw and grass:
            return self.bwGr_grass
        elif bw and not grass:
            return self.bwGr
        elif not bw and grass:
            return self.evtGr_grass
        elif not bw and not grass:
            return self.evtGr

    def scale(self, scale):
        self.evtHist.Scale(scale)
        self.bwHist.Scale(scale)

    def getTitle(self):
        return self.title

    def setLineColors(self, color):
        self.bwHist.SetLineColor(color)
        self.evtHist.SetLineColor(color)

    def setFillColors(self, color):
        self.bwHist.SetFillColor(color)
        self.evtHist.SetFillColor(color)

    def setLineStyles(self, style):
        self.bwHist.SetLineStyle(style)
        self.evtHist.SetLineStyle(style)

    def setLineWidths(self, width):
        self.bwHist.SetLineWidth(width)
        self.evtHist.SetLineWidth(width)

    def makeDataGraphs(self, forcePoissonErrors = True, horizErrs = False):
        """ converts the members histograms into TGraphAsymmErrors
            and adds them as class members 
            NOTE: to plot the grass 0-evt bins without dots, two TGraphs are created
            one contains the non-0 points, the other the 0-only points and has a markersize of 0 """
        
        if forcePoissonErrors:
            self.evtHist.SetBinErrorOption(ROOT.TH1.kPoisson)
        
        self.evtGr = ROOT.TGraphAsymmErrors()
        self.evtGr.SetName ('gr_'+self.evtHist.GetName())

        self.evtGr_grass = ROOT.TGraphAsymmErrors()
        self.evtGr_grass.SetName ('gr_'+self.evtHist.GetName()+'_grass')

        for ibin in range (1, self.evtHist.GetNbinsX()+1):
            x = self.evtHist.GetBinCenter(ibin);
            y = self.evtHist.GetBinContent(ibin);
            dxHigh  = self.evtHist.GetBinLowEdge(ibin+1) - x;
            dxLow   = x - self.evtHist.GetBinLowEdge(ibin);
            dyHigh  = self.evtHist.GetBinErrorUp(ibin);
            dyLow   = self.evtHist.GetBinErrorLow(ibin);
            if horizErrs:
                dxLow = 0
                dxHigh  = 0
            if y == 0:
                self.evtGr_grass.SetPoint (self.evtGr_grass.GetN(), x, y)
                self.evtGr_grass.SetPointError (self.evtGr_grass.GetN()-1, dxLow, dxHigh, dyLow, dyHigh)
            else:
                self.evtGr.SetPoint (self.evtGr.GetN(), x, y)
                self.evtGr.SetPointError (self.evtGr.GetN()-1, dxLow, dxHigh, dyLow, dyHigh)


        ## now make also the bin width scaled gr - start from non-scaled so that errors will be ok
        self.bwGr = ROOT.TGraphAsymmErrors()
        self.bwGr.SetName ('gr_'+self.evtHist.GetName()+'_bw')

        self.bwGr_grass = ROOT.TGraphAsymmErrors()
        self.bwGr_grass.SetName ('gr_'+self.evtHist.GetName()+'_bw_grass')

        for ibin in range (1, self.evtHist.GetNbinsX()+1):
            x = self.evtHist.GetBinCenter(ibin);
            y = self.evtHist.GetBinContent(ibin);
            dxHigh  = self.evtHist.GetBinLowEdge(ibin+1) - x;
            dxLow   = x - self.evtHist.GetBinLowEdge(ibin);
            dyHigh  = self.evtHist.GetBinErrorUp(ibin);
            dyLow   = self.evtHist.GetBinErrorLow(ibin);
            bw = self.evtHist.GetBinLowEdge(ibin+1) - self.evtHist.GetBinLowEdge(ibin)
            y = y/bw
            dyHigh = dyHigh/bw
            dyLow = dyLow/bw

            if horizErrs:
                dxLow = 0
                dxHigh  = 0
            if y == 0:
                self.bwGr_grass.SetPoint (self.bwGr_grass.GetN(), x, y)
                self.bwGr_grass.SetPointError (self.bwGr_grass.GetN()-1, dxLow, dxHigh, dyLow, dyHigh)
            else:            
                self.bwGr.SetPoint (self.bwGr.GetN(), x, y)
                self.bwGr.SetPointError (self.bwGr.GetN()-1, dxLow, dxHigh, dyLow, dyHigh)

        self.setGraphStyles()

    def blindRegion (self, xmin, xmax):
        """ blind data before plotting them. Note that this removes only points from data graphs, not histograms """

        try: self.evtGr
        except AttributeError:
            print "** Warning: SampleHist : blindRegion : event data TGraphs were not created yet, I have nothing to blind"
            return

        for ipt in reversed(range (0, self.evtGr.GetN())):
            xpt = ROOT.Double(0.0)
            ypt = ROOT.Double(0.0)
            self.evtGr.GetPoint(ipt, xpt, ypt)
            if xpt > xmin and xpt < xmax:
                self.evtGr.RemovePoint(ipt)

        for ipt in reversed(range (0, self.evtGr_grass.GetN())):
            xpt = ROOT.Double(0.0)
            ypt = ROOT.Double(0.0)
            self.evtGr_grass.GetPoint(ipt, xpt, ypt)
            if xpt > xmin and xpt < xmax:
                self.evtGr_grass.RemovePoint(ipt)

        for ipt in reversed(range (0, self.bwGr.GetN())):
            xpt = ROOT.Double(0.0)
            ypt = ROOT.Double(0.0)
            self.bwGr.GetPoint(ipt, xpt, ypt)
            if xpt > xmin and xpt < xmax:
                self.bwGr.RemovePoint(ipt)

        for ipt in reversed(range (0, self.bwGr_grass.GetN())):
            xpt = ROOT.Double(0.0)
            ypt = ROOT.Double(0.0)
            self.bwGr_grass.GetPoint(ipt, xpt, ypt)
            if xpt > xmin and xpt < xmax:
                self.bwGr_grass.RemovePoint(ipt)
        
    def setGraphStyles(self):
        try: 
            self.bwGr
        except AttributeError:
            print "** Warning: SampleHist : setGraphStyles: graphs do not exist yet, creating now with default options..."
            self.makeDataGraphs()
        
        self.bwGr.SetMarkerStyle(8)
        self.bwGr.SetMarkerSize(1.0)
        self.bwGr.SetMarkerColor(ROOT.kBlack)
        self.bwGr.SetLineColor(ROOT.kBlack)

        self.evtGr.SetMarkerStyle(8)
        self.evtGr.SetMarkerSize(1.0)
        self.evtGr.SetMarkerColor(ROOT.kBlack)
        self.evtGr.SetLineColor(ROOT.kBlack)

        self.bwGr_grass.SetMarkerStyle(0)
        self.bwGr_grass.SetMarkerSize(0)
        self.bwGr_grass.SetMarkerColor(ROOT.kBlack)
        self.bwGr_grass.SetLineColor(ROOT.kBlack)

        self.bwGr_grass.SetMarkerStyle(0)
        self.bwGr_grass.SetMarkerSize(0)
        self.bwGr_grass.SetMarkerColor(ROOT.kBlack)
        self.bwGr_grass.SetLineColor(ROOT.kBlack)

class SampleHistColl:
    """ 
    collection of SampleHist. This class knows what has to be plotted
    """
    def __init__(self):
        
        ## the histogram classes
        self.sigs = {}
        self.bkgs = {}
        self.data = {}

        # what to plot : order determines stack order -- bottom up
        self.sigsToPlot = []
        self.bkgsToPlot = []
        self.dataToPlot = []

        # color and lines - one dict is enough if the same name is not used for data,sig,and bkg
        self.linestyles = {}
        self.linecolors = {}
        self.fillcolors = {}

        # this modify signal histos and scales them by a factor
        self.sigscale = None

        ## plotting options
        self.logy       = False
        self.logx       = False
        self.divByBW    = True
        self.plotData   = True
        self.plotSig    = True
        self.dataGrass  = True
        self.ratio      = False
        self.sbplot     = False
        self.ratioherrs = False
        self.plotLegend = True

        ## optional customizations for cmd line - if defined, override automatic calculations
        self.ymin = None
        self.ymax = None
        self.xmin = None
        self.xmax = None
        self.legxmin = None
        self.legymin = None
        self.legcoords = None # [xmin, ymin, xmax, ymax]
        self.siglegextratext = None
        self.blindrange = None # [xmin, xmax] for blinding box

        ## fonts and sizes
        self.textfont      = 43
        self.lumitextfont  = 42
        self.legtextsize   = 18
        self.axistextsize  = 30
        self.axislabelsize = 22
        self.ndivisions    = 505
        self.xtitleoffset  = 1.0
        self.ytitleoffset  = 1.2
        self.ratioxtitleoffset = 2.8

        self.cmstextfont   = 61  # font of the "CMS" label
        self.cmstextsize   = 0.05  # font size of the "CMS" label
        self.chantextsize = 18
        self.extratextfont = 52     # for the "preliminary"
        self.extratextsize = 0.76 * self.cmstextsize # for the "preliminary"
        self.cmstextinframe = True
        self.selectiontextsize = 18

        self.lowerpadbottommargin = 0.35

        self.ratioymin = 0
        self.ratioymax = 2
        self.sbymin = 0
        self.sbymax = 5

        ## other info on the plot
        self.lumi = -999.99 ## in fb^-1
        self.lumiformat = '{:.1f}'
        self.chan = None    ## e.g. 'bb XX'
        self.ispreliminary = False
        self.selection = None    ## e.g. 'bb XX'
        self.region = None    ## e.g. 'bb XX'
        ## titles and plot properties
        # self.xtitle = 'xtitle'
        # self.ytitle = 'ytitle'
        self.title  = None # note: y automatically set to 'Events' if no division by bin width
        self.xtitle = None
        self.ytitle = None

    def addHisto(self, histo, name, htype, title='histoTitle'):
        # print "@@ adding histo", histo.GetName(), "to", name , '( type:', htype ,')'
        print "@@ adding histo {:80} to {:15} ( type: {:4})".format(histo.GetName(), name, htype)
        mycoll = None
        if   htype == 'sig':
            mycoll = self.sigs
        elif htype == 'bkg':
            mycoll = self.bkgs
        elif htype == 'data':
            mycoll = self.data
        else:
            raise ValueError ('** SampleHistColl : addHisto : type needs to be sig, bkg, or data, you passed %s' % str(htype))

        if not name in mycoll:
            sh = SampleHist(name=name, title=title, htype=htype, inputH=histo)
            mycoll[name] = sh
        else:
            mycoll[name].addHisto(histo)

    def setListToPlot (self, orderList, htype):
        if   htype == 'sig':
            self.sigsToPlot = list(orderList)
        elif htype == 'bkg':
            self.bkgsToPlot = list(orderList)
        elif htype == 'data':
            self.dataToPlot = list(orderList)
        else:
            raise ValueError ("SampleHistColl : setListToPlot : htype needs to be sig, bkg, or data, you passed %s" % str(htype))

    def makeStacks (self, stackname='bkgstack'): ## only stack bkg, but can insert additional input to get 
        
        ## no bw scaled stack
        self.bkgstack = ROOT.THStack(stackname, stackname)
        for bkgname in self.bkgsToPlot:
            self.bkgstack.Add(self.bkgs[bkgname].evtHist)

        ## w/ bw scaled stack
        self.bwbkgstack = ROOT.THStack(stackname+'_bw', stackname+'_bw')
        for bkgname in self.bkgsToPlot:
            self.bwbkgstack.Add(self.bkgs[bkgname].bwHist)

    def makeLegend (self):
        legxmin = 0.55
        legymin = 0.45
        if not self.ispreliminary: #shif down and left for more space
            legxmin = 0.475
            legymin = 0.40
        if self.legxmin:
            legxmin = self.legxmin
        if self.legymin:
            legymin = self.legymin
        legxmax = 0.37+legxmin
        legymax = 0.93
        if not self.ispreliminary: #shif down and left for more space
            legxmax = 0.50+legxmin
            legymax = 0.93
        if self.legcoords:
            legxmin = self.legcoords[0]
            legymin = self.legcoords[1]
            legxmax = self.legcoords[2]
            legymax = self.legcoords[3]
        self.legend = ROOT.TLegend (legxmin, legymin, legxmax, legymax)
        self.legend.SetFillStyle(0)
        self.legend.SetNColumns(2)
        self.legend.SetBorderSize(0)
        self.legend.SetTextFont(self.textfont)
        self.legend.SetTextSize(self.legtextsize)

        if self.plotData:
            for dp in self.dataToPlot:
                try:
                    self.data[dp].evtGr
                except AttributeError:
                    print ' : makeLegend : data does not have graphs yet, doing now...'
                    self.data[dp].makeDataGraphs()
                self.legend.AddEntry(self.data[dp].getGraph(bw = self.setDivideByBinWidth, grass = False), self.data[dp].getTitle(), "pe")

        for sh in reversed(self.bkgsToPlot):
            his = self.bkgs[sh].getHisto(bw = self.divByBW)
            self.legend.AddEntry(his, self.bkgs[sh].getTitle(), "f")

        if hasattr(self, 'stackErrorHist'):
            self.legend.AddEntry (self.stackErrorHist, "Bkg. unc.", "f")

        if self.plotSig:
            for name in reversed(self.sigsToPlot):
                sigsh = self.sigs[name]
                self.legend.AddEntry (sigsh.getHisto(self.divByBW), sigsh.getTitle(), "l")
        if self.siglegextratext:
            self.legend.AddEntry(None, self.siglegextratext, "")


    def makeCanvas(self):
        self.c1 = ROOT.TCanvas("c1", "c1", 600, 600)
    
        ## case with 2 pads drawn
        if self.ratio or self.sbplot:
            self.c1.cd()
            self.pad1 = ROOT.TPad ("pad1", "pad1", 0, 0.25, 1, 1.0)
            self.pad1.SetFrameLineWidth(3)
            self.pad1.SetLeftMargin(0.15);
            self.pad1.SetBottomMargin(0.02);
            self.pad1.SetTopMargin(0.055);
            # self.pad1.Draw()

            self.c1.cd()
            self.pad2 = ROOT.TPad ("pad2", "pad2", 0, 0.0, 1, 0.2496)
            self.pad2.SetLeftMargin(0.15);
            self.pad2.SetTopMargin(0.05);
            # self.pad2.SetBottomMargin(0.35);
            self.pad2.SetBottomMargin(self.lowerpadbottommargin);
            self.pad2.SetGridy(True);
            self.pad2.SetFrameLineWidth(3)
            # self.pad2.Draw()
            # self.pad2.SetGridx(True);

        else:
            self.c1.cd()
            self.pad1 = ROOT.TPad ("pad1", "pad1", 0, 0.0, 1.0, 1.0)
            self.pad1.SetFrameLineWidth(3)
            self.pad1.SetLeftMargin(0.15);
            self.pad1.SetBottomMargin(0.12);
            self.pad1.SetTopMargin(0.055);
            self.pad1.Draw()

            self.pad2 = None

        if self.logy:
            self.pad1.SetLogy(True)

        if self.logx:
            self.pad1.SetLogx(True)
            if self.pad2:
                self.pad2.SetLogx(True)

    def makePlot(self):
        
        self.setHistoStyles()

        try:
            self.c1
        except AttributeError:
            self.makeCanvas()
        self.c1.cd()
        self.pad1.Draw()
        self.pad1.cd()

        ### scale all signal histograms if required
        if self.plotSig and self.sigscale:
            for sh in self.sigs: 
                self.sigs[sh].scale(self.sigscale)

        try:
            self.bkgstack
        except AttributeError:
            self.makeStacks()

        ### look for plot boundaries
        ymin = 0
        if self.logy: ymin = 1.E-5

        maxs = []
        stackToUse = self.bwbkgstack if self.divByBW else self.bkgstack
        try: maxs.append(stackToUse.GetStack().Last().GetMaximum())
        except ReferenceError:
            print '** Error in SampleHistColl : makePlot : stack is empty : ' , stackToUse.GetNhists()
            raise
        if self.plotData:
            for dh in self.dataToPlot: 
                maxs.append(self.data[dh].getHisto(self.divByBW).GetMaximum())
        if self.plotSig:
            for sh in self.sigsToPlot: 
                maxs.append(self.sigs[sh].getHisto(self.divByBW).GetMaximum())
        ymax = max(maxs)
        # print ymax, maxs

        extraspace = 0.1
        if not self.logy:
            ymax += extraspace* (ymax-ymin)
        else:
            ymaxnew = extraspace * (math.log(ymax, 10) - math.log(ymin, 10)) + math.log(ymax, 10)
            ymax = math.pow(10, ymaxnew)

        ## override if user configuration
        if self.ymin: ymin = self.ymin
        if self.ymax: ymax = self.ymax
        stackToUse.SetMinimum(ymin)
        stackToUse.SetMaximum(ymax)
        stackToUse.Draw('HIST') ## draw to create axis
        
        if self.xmax or self.xmin:
            usrxmin = stackToUse.GetStack().Last().GetBinLowEdge(1)
            usrxmax = stackToUse.GetStack().Last().GetBinLowEdge(stackToUse.GetStack().Last().GetNbinsX()+1)
            if self.xmin : usrxmin = self.xmin 
            if self.xmax : usrxmax = self.xmax 
            stackToUse.GetXaxis().SetRangeUser(usrxmin, usrxmax)

        ## titles and styles
        # stackToUse.SetTitle('')
        # stackToUse.GetXaxis().SetTitle(self.xtitle)
        # stackToUse.GetYaxis().SetTitle(self.ytitle)
        stackToUse.SetTitle('')
        stackToUse.GetXaxis().SetTitle(stackToUse.GetStack().Last().GetXaxis().GetTitle())

        if self.title:
            decTitle = self.decodeTitle()
            stackToUse.SetTitle(decTitle[0])
            if decTitle[1] : stackToUse.GetXaxis().SetTitle(decTitle[1])            
            if decTitle[2] : stackToUse.GetYaxis().SetTitle(decTitle[2])

        if not self.divByBW: stackToUse.GetYaxis().SetTitle('Events')

        
        ## overrides if input 
        # if self.ytitle: stackToUse.GetYaxis().SetTitle(self.ytitle)
        # # else: stackToUse.GetYaxis().SetTitle(stackToUse.GetStack().Last().GetYaxis().GetTitle())
        # if not self.divByBW: stackToUse.GetYaxis().SetTitle('Events') # does not change title, but works if commenting previous line... ??
        
        # if self.xtitle: stackToUse.GetXaxis().SetTitle(self.xtitle)
        # else: stackToUse.GetXaxis().SetTitle(stackToUse.GetStack().Last().GetXaxis().GetTitle())

        stackToUse.GetXaxis().SetTitleFont(self.textfont)
        stackToUse.GetYaxis().SetTitleFont(self.textfont)
        stackToUse.GetXaxis().SetLabelFont(self.textfont)
        stackToUse.GetYaxis().SetLabelFont(self.textfont)
        stackToUse.GetXaxis().SetTitleOffset(self.xtitleoffset)
        stackToUse.GetYaxis().SetTitleOffset(self.ytitleoffset)
        stackToUse.GetXaxis().SetTitleSize(self.axistextsize)
        stackToUse.GetYaxis().SetTitleSize(self.axistextsize)
        stackToUse.GetXaxis().SetLabelSize(self.axislabelsize)
        stackToUse.GetYaxis().SetLabelSize(self.axislabelsize)
        if self.pad2:
            stackToUse.GetXaxis().SetTitleSize(0)
            stackToUse.GetXaxis().SetLabelSize(0)

        stackToUse.Draw('HIST')

        if self.plotData:
            for dh in self.dataToPlot: 
                try: self.data[dh].evtGr
                except AttributeError: self.data[dh].makeDataGraphs()
                if self.blindrange:
                    self.data[dh].blindRegion(self.blindrange[0], self.blindrange[1])
                self.data[dh].getGraph(bw = self.divByBW, grass = False).Draw('pz same') ## Z: no small line at the end of error bar
                if self.dataGrass:
                    self.data[dh].getGraph(bw = self.divByBW, grass = True).Draw('pz same') ## Z: no small line at the end of error bar

        if self.plotSig:
            for sh in self.sigsToPlot: 
                self.sigs[sh].getHisto(self.divByBW).Draw('hist same')

        ## error histogram
        self.makeErrorHisto()
        if hasattr(self, 'stackErrorHist'):
            hhist = self.bw_stackErrorHist if self.divByBW else self.stackErrorHist 
            hhist.Draw("E2 same")

        if self.blindrange:
            self.blindbox = ROOT.TBox (float(self.blindrange[0]), ymin, float(self.blindrange[1]), 0.93*ymax)
            self.blindbox.SetFillStyle(3003) # NB: does not look the same in interactive sessions and printed pdf
            self.blindbox.SetFillColor(ROOT.kGray+2) # NB: does not look the same in interactive sessions and printed pdf
            self.blindbox.Draw()


        try: self.legend
        except AttributeError: self.makeLegend()
        if self.plotLegend: self.legend.Draw()

        ## additional texts on the plot
        self.makeExtraTexts()
        for tx in self.extratexts:
            self.extratexts[tx].Draw()

        ### if a second pad is needed
        if self.ratio or self.sbplot:
            self.c1.cd()
            self.pad2.Draw()
            self.pad2.cd()
            if self.ratio: self.makeDataToMCRatios()
            elif self.sbplot: self.makeSBPlots()
            
            ## a frame to do the plots
            try:
                self.hratioframe
            except AttributeError:
                if self.dataToPlot:
                    self.hratioframe = self.data[self.dataToPlot[0]].getHisto().Clone('hratioframe')
                elif self.bkgsToPlot:
                    self.hratioframe = self.bkgs[self.bkgsToPlot[0]].getHisto().Clone('hratioframe')
                elif self.sigsToPlot:
                    self.hratioframe = self.sigs[self.sigsToPlot[0]].getHisto().Clone('hratioframe')
                else:
                    raise RuntimeError("problems in creating the range for ratio plot")

            self.hratioframe.GetXaxis().SetTitleFont(self.textfont) # so that size is in pixels
            self.hratioframe.GetYaxis().SetTitleFont(self.textfont) # so that size is in pixels
            self.hratioframe.GetXaxis().SetLabelFont(self.textfont) # so that size is in pixels
            self.hratioframe.GetYaxis().SetLabelFont(self.textfont) # so that size is in pixels
            self.hratioframe.GetYaxis().SetNdivisions(self.ndivisions)

            #self.hratioframe.GetXaxis().SetTitle(bkgStack.GetXaxis().GetName())
            if self.title: self.hratioframe.SetTitle(self.title)
            if self.ratio: self.hratioframe.GetYaxis().SetTitle ("Data/MC")
            elif self.sbplot: self.hratioframe.GetYaxis().SetTitle ("S/B")
            self.hratioframe.GetXaxis().SetTitleOffset(self.ratioxtitleoffset)
            self.hratioframe.GetYaxis().SetTitleOffset(self.ytitleoffset)

            self.hratioframe.GetXaxis().SetTitleSize(self.axistextsize);
            self.hratioframe.GetXaxis().SetLabelSize(self.axislabelsize);
            self.hratioframe.GetYaxis().SetTitleSize(self.axistextsize);
            self.hratioframe.GetYaxis().SetLabelSize(self.axislabelsize);

            self.hratioframe.GetXaxis().SetTickSize(0.10)
            self.hratioframe.GetYaxis().SetTickSize(0.05)

            if self.xmax or self.xmin:
                self.hratioframe.GetXaxis().SetRangeUser(usrxmin, usrxmax)

            self.hratioframe.SetStats(0)
            
            if self.ratio:
                self.hratioframe.SetMinimum(self.ratioymin)
                self.hratioframe.SetMaximum(self.ratioymax)

            elif self.sbplot:
                self.hratioframe.SetMinimum(self.sbymin)
                self.hratioframe.SetMaximum(self.sbymax)

            self.hratioframe.Draw('axis')
            if hasattr(self, 'ratioErrorHist'):
                self.ratioErrorHist.Draw("E2 same")

            if self.ratio:
                for dh in self.dataToPlot:
                    self.dataToMCRatios[dh].Draw('p z 0 same') ## z: no small lines at the end of the points, 0: lines outside the range
            elif self.sbplot:
                for sh in self.sbRatioPlots:
                    self.sbRatioPlots[sh].Draw('p l same')

            self.pad2.RedrawAxis();
            self.pad2.RedrawAxis("g"); #otherwise no grid..

    def makeDataToMCRatios (self):
        
        try:
            self.bkgstack
        except AttributeError:
            self.makeStacks()

        try:
            self.dataToMCRatios
        except AttributeError:
            self.dataToMCRatios = {}

        ## can use the non bw scaled histos only -- bw factors out
        for dh in self.data:
            print '.. doing data/mc ratio for: ' , dh
            self.dataToMCRatios[dh] = ROOT.TGraphAsymmErrors()
            hdata = self.data[dh].getHisto(False)
            hMC   = self.bkgstack.GetStack().Last()
            self.dataToMCRatios[dh].SetName('grRatioToMC_'+hdata.GetName())
            for ibin in range (1, hdata.GetNbinsX()+1):
                num = hdata.GetBinContent(ibin)
                den = hMC.GetBinContent(ibin)
                if den > 0:
                    # Y
                    x = hdata.GetBinCenter(ibin)
                    y = num/den
                    exlow  = hdata.GetBinCenter(ibin) - hdata.GetBinLowEdge(ibin) if self.ratioherrs else 0.
                    exhigh = hdata.GetBinLowEdge(ibin+1) - hdata.GetBinCenter(ibin) if self.ratioherrs else 0.
                    eyhigh = hdata.GetBinErrorUp(ibin) / den
                    eylow  = hdata.GetBinErrorLow(ibin) / den
                    self.dataToMCRatios[dh].SetPoint(self.dataToMCRatios[dh].GetN(), x, y)
                    self.dataToMCRatios[dh].SetPointError(self.dataToMCRatios[dh].GetN()-1, exlow, exhigh, eylow, eyhigh)

                ## styles
                self.dataToMCRatios[dh].SetMarkerStyle(8)
                self.dataToMCRatios[dh].SetMarkerSize(1.0)
                self.dataToMCRatios[dh].SetMarkerColor(ROOT.kBlack)
                self.dataToMCRatios[dh].SetLineColor(ROOT.kBlack)

                if self.blindrange:
                    xmin = self.blindrange[0]
                    xmax = self.blindrange[1]
                    for ipt in reversed(range (0, self.dataToMCRatios[dh].GetN())):
                        xpt = ROOT.Double(0.0)
                        ypt = ROOT.Double(0.0)
                        self.dataToMCRatios[dh].GetPoint(ipt, xpt, ypt)
                        if xpt > xmin and xpt < xmax:
                            self.dataToMCRatios[dh].RemovePoint(ipt)


    def makeSBPlots(self):

        try:
            self.bkgstack
        except AttributeError:
            self.makeStacks()

        try:
            self.sbRatioPlots
        except AttributeError:
            self.sbRatioPlots = {}

        for sh in self.sigsToPlot:
            print '.. doing S/B ratio for: ' , sh
            self.sbRatioPlots[sh] = ROOT.TGraph()
            hsig = self.sigs[sh].getHisto(False)
            hMC  = self.bkgstack.GetStack().Last()
            self.sbRatioPlots[sh].SetName('grSBplot_'+hsig.GetName())
            for ibin in range (1, hsig.GetNbinsX()+1):
                num = hsig.GetBinContent(ibin)
                den = hMC.GetBinContent(ibin)
                if den > 0:
                    # Y
                    x = hsig.GetBinCenter(ibin)
                    y = num/den
                    # exlow  = hsig.GetBinCenter(ibin) - hsig.GetBinLowEdge(ibin) if self.ratioherrs else 0.
                    # exhigh = hsig.GetBinLowEdge(ibin+1) - hsig.GetBinCenter(ibin) if self.ratioherrs else 0.
                    # eyhigh = hsig.GetBinErrorUp(ibin) / den
                    # eylow  = hsig.GetBinErrorLow(ibin) / den
                    self.sbRatioPlots[sh].SetPoint(self.sbRatioPlots[sh].GetN(), x, y)
                    # self.sbRatioPlots[sh].SetPointError(self.sbRatioPlots[sh].GetN()-1, exlow, exhigh, eylow, eyhigh)

                ## styles
                self.sbRatioPlots[sh].SetMarkerStyle(8)
                self.sbRatioPlots[sh].SetMarkerSize(0.8)
                self.sbRatioPlots[sh].SetMarkerColor(self.linecolors[sh])
                self.sbRatioPlots[sh].SetLineColor(self.linecolors[sh])
                self.sbRatioPlots[sh].SetLineStyle(self.linestyles[sh])


    def makeExtraTexts (self):
        try: self.extratexts
        except AttributeError: self.extratexts = {}

        ## luminosity
        # lumibox = ROOT.TLatex  (0.9, 0.964, "%.1f fb^{-1} (13 TeV)" % self.lumi)       
        lumibox = ROOT.TLatex  (0.9, 0.964, ("%s fb^{{-1}} (13 TeV)" % self.lumiformat).format(self.lumi))
        lumibox.SetNDC()
        lumibox.SetTextAlign(31)
        lumibox.SetTextSize(self.extratextsize)
        lumibox.SetTextFont(self.lumitextfont)
        lumibox.SetTextColor(ROOT.kBlack)
        self.extratexts['lumi'] = lumibox

        ## CMS
        xpos  = 0.177
        if self.cmstextinframe:
            ypos  = 0.94 ## inside the frame
        else:
            ypos  = 0.995  ## ouside the frame
        # CMSbox = ROOT.TLatex  (xpos, ypos , "CMS Phase-2")       
        CMSbox = ROOT.TLatex  (xpos, ypos , "CMS")
        CMSbox.SetNDC()
        CMSbox.SetTextSize(self.cmstextsize)
        CMSbox.SetTextFont(self.cmstextfont)
        CMSbox.SetTextColor(ROOT.kBlack)
        CMSbox.SetTextAlign(13) ## inside the frame
        self.extratexts['cms'] = CMSbox
        
        ## preliminary
        # prelimBox = ROOT.TLatex  (xpos, ypos - 0.06 , "Simulation Preliminary")
        # prelimBox = ROOT.TLatex  (xpos, ypos - 0.06 , "Preliminary")
        prelimBox = ROOT.TLatex  (xpos + 0.12, ypos-0.01, "Preliminary")
        prelimBox.SetNDC()
        prelimBox.SetTextSize(self.extratextsize)
        prelimBox.SetTextFont(self.extratextfont)
        prelimBox.SetTextColor(ROOT.kBlack)
        prelimBox.SetTextAlign(13)
        if self.ispreliminary: self.extratexts['preliminary'] = prelimBox
        
        ## channel
        if self.chan:
            # xposch = xpos + 0.14 if self.ispreliminary else xpos + 0.11 ## shift left for more space if not preliminary
            # xposch = xpos + 0.19 if self.ispreliminary else xpos + 0.11 ## shift left for more space if not preliminary
            xposch = xpos
            # if not self.cmstextinframe:
            #     xposch = xpos
            # yposch = ypos if self.cmstextinframe else 0.94
            yposch = ypos if not self.cmstextinframe - 0.007 else ypos - 0.065
            # chBox = ROOT.TLatex  (xposch, yposch - 0.007, self.chan)
            chBox = ROOT.TLatex  (xposch, yposch, self.chan)
            # chBox = ROOT.TLatex  (xposch, yposch - 0.069+ 0.025 - 0.007, self.chan)
            chBox.SetNDC()
            # chBox.SetTextSize(self.cmstextsize+18)
            chBox.SetTextSize(self.chantextsize)
            chBox.SetTextFont(self.textfont)
            chBox.SetTextColor(ROOT.kBlack)
            chBox.SetTextAlign(13)
            self.extratexts['chan'] = chBox
            chBox2 = ROOT.TLatex  (xposch, yposch - 0.069+ 0.025 - 0.007, "channel")
            chBox2.SetNDC()
            # chBox2.SetTextSize(self.cmstextsize+18)
            chBox2.SetTextSize(self.chantextsize)
            chBox2.SetTextFont(self.textfont)
            chBox2.SetTextColor(ROOT.kBlack)
            chBox2.SetTextAlign(13)
            # self.extratexts['chan2'] = chBox2
        ## selection_
        if self.selection:
            # xposch = xpos + 0.14 if self.ispreliminary else xpos + 0.11 ## shift left for more space if not preliminary
            # xposch = xpos + 0.19 if self.ispreliminary else xpos + 0.11 ## shift left for more space if not preliminary
            xposch = xpos
            # if not self.cmstextinframe:
            #     xposch = xpos
            # yposch = ypos if self.cmstextinframe else 0.94
            yposch = ypos if not self.cmstextinframe - 0.007 else ypos - 0.065
            # chBox = ROOT.TLatex  (xposch, yposch - 0.007, self.chan)
            chBox = ROOT.TLatex  (xposch, yposch-0.05, self.selection)
            # chBox = ROOT.TLatex  (xposch, yposch - 0.069+ 0.025 - 0.007, self.chan)
            chBox.SetNDC()
            # chBox.SetTextSize(self.cmstextsize+18)
            chBox.SetTextSize(self.selectiontextsize)
            chBox.SetTextFont(self.textfont)
            chBox.SetTextColor(ROOT.kBlack)
            chBox.SetTextAlign(13)
            self.extratexts['selection'] = chBox
            chBox2 = ROOT.TLatex  (xposch, yposch - 0.069+ 0.025 - 0.007, "selection")
            chBox2.SetNDC()
            # chBox2.SetTextSize(self.cmstextsize+18)
            chBox2.SetTextSize(self.selectiontextsize)
            chBox2.SetTextFont(self.textfont)
            chBox2.SetTextColor(ROOT.kBlack)
            chBox2.SetTextAlign(13)
            # self.extratexts['chan2'] = chBox2
        ## 
        if self.region:
            # xposch = xpos + 0.14 if self.ispreliminary else xpos + 0.11 ## shift left for more space if not preliminary
            # xposch = xpos + 0.19 if self.ispreliminary else xpos + 0.11 ## shift left for more space if not preliminary
            xposch = xpos
            # if not self.cmstextinframe:
            #     xposch = xpos
            # yposch = ypos if self.cmstextinframe else 0.94
            yposch = ypos if not self.cmstextinframe - 0.007 else ypos - 0.065
            # chBox = ROOT.TLatex  (xposch, yposch - 0.007, self.chan)
            chBox = ROOT.TLatex  (xposch, yposch-0.1, self.region)
            # chBox = ROOT.TLatex  (xposch, yposch - 0.069+ 0.025 - 0.007, self.chan)
            chBox.SetNDC()
            # chBox.SetTextSize(self.cmstextsize+18)
            chBox.SetTextSize(self.selectiontextsize)
            chBox.SetTextFont(self.textfont)
            chBox.SetTextColor(ROOT.kBlack)
            chBox.SetTextAlign(13)
            self.extratexts['region'] = chBox
            chBox2 = ROOT.TLatex  (xposch, yposch - 0.069+ 0.025 - 0.007, "region")
            chBox2.SetNDC()
            # chBox2.SetTextSize(self.cmstextsize+18)
            chBox2.SetTextSize(self.selectiontextsize)
            chBox2.SetTextFont(self.textfont)
            chBox2.SetTextColor(ROOT.kBlack)
            chBox2.SetTextAlign(13)

    def makeErrorHisto(self):
        """ creates a shaded histo for syst error.
        It uses an externally passed histogram that has the same binning and right errors
        NOTE: this function needs the histograms to be defined, if they are not given in input return False
        """

        try:
            self.stackErrorHist
        except AttributeError:
            print "** Warning : SampleHistColl : makeErrorHisto : no self.stackErrorHist provided, no bkg errors will be drawn"
            return
        
        self.stackErrorHist.SetFillStyle(3345)
        self.stackErrorHist.SetFillColor(ROOT.kGray+2)
        self.stackErrorHist.SetMarkerColor(ROOT.kGray+2)
        self.stackErrorHist.SetLineColor(ROOT.kGray+2)
        self.stackErrorHist.SetLineWidth(0)
        self.stackErrorHist.SetMarkerSize(0)
        self.stackErrorHist.SetMarkerStyle(0)

        self.bw_stackErrorHist = self.stackErrorHist.Clone('bw_'+self.stackErrorHist.GetName())

        # scale bw errors by the bw , and histo by the width
        for ibin in range(1,self.bw_stackErrorHist.GetNbinsX()+1):
            bcont = self.bw_stackErrorHist.GetBinContent(ibin)
            berr = self.bw_stackErrorHist.GetBinError(ibin)
            bw = self.bw_stackErrorHist.GetBinLowEdge(ibin+1) - self.bw_stackErrorHist.GetBinLowEdge(ibin)
            self.bw_stackErrorHist.SetBinContent(ibin, bcont/bw)
            self.bw_stackErrorHist.SetBinError(ibin, berr/bw)


        if self.ratio:
            self.ratioErrorHist = self.stackErrorHist.Clone('ratioerr_'+self.stackErrorHist.GetName())
            for ibin in range(1, self.ratioErrorHist.GetNbinsX()+1):
                bc = self.ratioErrorHist.GetBinContent(ibin)
                berr = self.ratioErrorHist.GetBinError(ibin)
                self.ratioErrorHist.SetBinContent(ibin, 1)
                self.ratioErrorHist.SetBinError(ibin, berr/bc) if bc > 0 else self.ratioErrorHist.SetBinError(ibin, 0)

    def setHistoStyles(self):
        # bkg
        for name in self.bkgs:
            his = self.bkgs[name]
            if name in self.linecolors:
                his.setLineColors(self.linecolors[name])
            if name in self.linestyles:
                his.setLineStyles(self.linestyles[name])
            if name in self.fillcolors:
                his.setFillColors(self.fillcolors[name])

        # sig - line only
        for name in self.sigs:
            his = self.sigs[name]
            if name in self.linecolors:
                his.setLineColors(self.linecolors[name])
            if name in self.linestyles:
                his.setLineStyles(self.linestyles[name])
            his.setLineWidths(3)

        ## NOTE: data styles are set automatically when TGraphs are created with makeGraph()
        ## this is not good in case one wants to plot more data on the same plot
        ## - everything is ready and multiple data are drawn, but the same style
        ## need to add a setGraphStyle function to class SampleHist

    def decodeTitle(self):
        """ check whether this is a plain title or a title;xtitle;ytitle format """
        if not ';' in self.title:
            return (self.title, None, None)
        decoded = self.title.split(';')
        if len(decoded) == 2:
            return (decoded[0], decoded[1], None)
        else:
            return (decoded[0], decoded[1], decoded[2])

    def printTable(self, uoflow=False, printMCstat=False, floatFormat='.2'):
        """ print a table containing the event and yields """
        
        ############# BKGS
        tab = PrettyTable (['process', 'num evts'])
        if printMCstat: tab.add_column('MC stat', [])
        
        totyield = 0
        for name in self.bkgsToPlot:
            evtyield = self.bkgs[name].getHisto().Integral(-1, -1) if uoflow else self.bkgs[name].getHisto().Integral()
            totyield += evtyield
            MCstat   = self.bkgs[name].getHisto().GetEntries()
            row = [name, evtyield]
            if printMCstat: row.append(MCstat)
            tab.add_row(row)
        row = ['*TOTAL*', totyield]
        if printMCstat: row.append('-')
        tab.add_row(row)
        tab.float_format = floatFormat ## means print line %.<float_format>f

        print "\n--- table : BACKGROUNDS"
        print tab
        print ''
        
        ############# SIGS
        tab = PrettyTable (['process', 'num evts'])
        if printMCstat: tab.add_column('MC stat', [])
        
        totyield = 0
        for name in self.sigsToPlot:
            evtyield = self.sigs[name].getHisto().Integral(-1, -1) if uoflow else self.sigs[name].getHisto().Integral()
            totyield += evtyield
            MCstat   = self.sigs[name].getHisto().GetEntries()
            row = [name, evtyield]
            if printMCstat: row.append(MCstat)
            tab.add_row(row)
        # row = ['TOTAL', totyield]
        # if printMCstat: row.append('-')
        # tab.add_row(row)
        tab.float_format = floatFormat ## means print line %.<float_format>f

        print "--- table : SIGNALS"
        print tab
        print ''

        ############# DATA
        tab = PrettyTable (['process', 'num evts'])
        if printMCstat: tab.add_column('MC stat', [])
        
        totyield = 0
        for name in self.dataToPlot:
            evtyield = self.data[name].getHisto().Integral(-1, -1) if uoflow else self.data[name].getHisto().Integral()
            totyield += evtyield
            MCstat   = self.data[name].getHisto().GetEntries()
            row = [name, evtyield]
            if printMCstat: row.append(MCstat)
            tab.add_row(row)
        # row = ['TOTAL', totyield]
        # if printMCstat: row.append('-')
        # tab.add_row(row)
        tab.float_format = floatFormat ## means print line %.<float_format>f

        print "--- table : DATA"
        print tab
        print ''


    ## plotter opiton setters -- really needed in python?
    def setLogY(self, val):
        self.logy = bool(val)

    def setDivideByBinWidth(self, val):
        self.divByBW = bool(val)

    def setPlotData(self, val):
        self.plotData = bool(val)

    def setPlotSignal(self, val):
        self.plotSig = bool(val)

    def setLineColors(self, dic):
        self.linecolors = dict(dic)

    def setLineStyles(self, dic):
        self.linestyles = dict(dic)

    def setFillColors(self, dic):
        self.fillcolors = dict(dic)
        
    def addLineColor(self, name, val):
        self.linecolors[name] = val

    def addLineStyle(self, name, val):
        self.linestyles[name] = val

    def addFillColor(self, name, val):
        self.fillcolors[name] = val

    def setSitLegExtraText(self, text):
        self.siglegextratext = text

    def setXtitle(self, title):
        self.xtitle = title

    def setYtitle(self, title):
        self.ytitle = title



