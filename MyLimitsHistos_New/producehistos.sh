mkdir Histos2016 Histos20172018

hadd -f Histos2016/outPlotter.root          DataSignalShapeHistos2016/outPlotter.root Model3b24bHistos2016/outPlotter.root
hadd -f Histos20172018/outPlotter.root      DataSignalShapeHistos2017/outPlotter.root DataSignalShapeHistos2018/outPlotter.root Model3b24bHistos20172018/outPlotter.root
hadd -f Histos2016/outPlotter_norm.root     DataSignalNormHistos2016/outPlotter.root
hadd -f Histos20172018/outPlotter_norm.root DataSignalNormHistos2017/outPlotter.root DataSignalNormHistos2018/outPlotter.root
