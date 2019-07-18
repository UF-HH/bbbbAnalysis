import ROOT
ROOT.gROOT.SetBatch(True)

injlist = [0, 3000, 6000, 9000]
norm = 379 ## ggf: 379, vbf : 21
fIn_proto = 'higgsCombinefitresult_{inj}.MultiDimFit.mH120.root'

gr = ROOT.TGraphAsymmErrors()

print '... doing for list', injlist
for inj in injlist:
    fInname = fIn_proto.format(inj=inj)
    fIn = ROOT.TFile(fInname)
    tIn = fIn.Get('limit')
    if tIn.GetEntries() != 3:
        raise RuntimeError("cannot understand entries in file %s" % fIn)
    vals = []
    for i in range(3):
        tIn.GetEntry(i)
        vals.append(tIn.r)

    inj_r = inj/norm
    gr.SetPoint(gr.GetN(), inj_r, vals[0])
    gr.SetPointError(gr.GetN()-1, 0, 0, vals[0]-vals[1], vals[2]-vals[0])


c1 = ROOT.TCanvas('c1', 'c1', 600, 600)
c1.SetFrameLineWidth(3)

gr.SetMarkerSize(1)
gr.SetMarkerStyle(8)
gr.SetMarkerColor(ROOT.kBlue)
gr.SetLineColor(ROOT.kBlue)
gr.SetTitle(';Injected #mu;Best-fit #mu')
gr.Draw('AP')

ll = ROOT.TLine(0, 0, max(injlist)/norm, max(injlist)/norm)
ll.SetLineStyle(7)
ll.SetLineColor(ROOT.kGray)
ll.Draw()

c1.Print('sig_vs_inj.pdf', 'pdf')