pload ALL
stepread Cone_surf.stp shape *
#tolerance shape
donly shape
axo
wait 1
fit
wait 5

pscale shape 0 0 0 0.1
tolerance shape
donly shape
axo

#bopfuse shape
#vdisplay shape
#vfit
#vsetdispmode 1
#vaspects -isoontriangulation 1
