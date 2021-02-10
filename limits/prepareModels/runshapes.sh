YEAR=$1 #2016 or #20172018

python add_shape_unc_fromshapes.py physicsmodel$YEAR/outPlotter_GGFcateg1_nobkgshapeunc.root $YEAR GGFcateg1
python add_shape_unc_fromshapes.py physicsmodel$YEAR/outPlotter_GGFcateg2_nobkgshapeunc.root $YEAR GGFcateg2
python add_shape_unc_fromfit.py    physicsmodel$YEAR/outPlotter_VBFcateg1_nobkgshapeunc.root $YEAR VBFcateg1
