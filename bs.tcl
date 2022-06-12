pload ALL
stepread bs_curve.step shape *
stepread cmake-build-debug/out.step shape_2 *

donly shape shape_2
axo
fit


#bopfuse shape
#vdisplay shape
#vfit
#vsetdispmode 1
#vaspects -isoontriangulation 1
