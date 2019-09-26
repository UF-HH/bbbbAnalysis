from sympy import *
from numpy import matrix
from numpy import linalg
from sympy import Matrix

## implements the formula of https://indico.cern.ch/event/844260/contributions/3568369/attachments/1909554/3154880/2019_09_17_bbtautau_HHrew.pdf, slide 2
## c -> vector of couplings
## s -> vector of samples (generated xs)
## v -> vector of components (a,b,c,iab,ibc,iac)
## M -> matrix that describes the single samples as sum of components, so that s = M * v
## so that M^-1 * s = v
##
## sigma (CV, C2V, kl) = c^T * v = c^T * M-1 * s

class sample:
    def __init__(self, val_CV, val_C2V, val_kl, val_xs):
        self.val_CV  = val_CV
        self.val_C2V = val_C2V
        self.val_kl  = val_kl
        self.val_xs  = val_xs

def build_matrix(sample_list):    
    if len(sample_list) != 6:
        print "[ERROR] : expecting 6 samples in input"
        raise RuntimeError("malformed input sample list")
    M_tofill = [
        [None,None,None,None,None,None],
        [None,None,None,None,None,None],
        [None,None,None,None,None,None],
        [None,None,None,None,None,None],
        [None,None,None,None,None,None],
        [None,None,None,None,None,None]
    ]

    for isample, sample in enumerate(sample_list):
        # print isample, " CV, C2V, kl = ", sample.val_CV, sample.val_C2V, sample.val_kl
        
        ## implement the 6 scalings
        M_tofill[isample][0] = sample.val_CV**2 * sample.val_kl**2
        M_tofill[isample][1] = sample.val_CV**4
        M_tofill[isample][2] = sample.val_C2V**2
        M_tofill[isample][3] = sample.val_CV**3 * sample.val_kl
        M_tofill[isample][4] = sample.val_CV    * sample.val_C2V    * sample.val_kl
        M_tofill[isample][5] = sample.val_CV**2 * sample.val_C2V
    
    # print M_tofill

    res = Matrix(M_tofill)
    return res

## this is the matrix that determines the sigmas as a function of the components
# Mxcheck = Matrix([
#     [1,1,1,1,1,1],
#     [0,1,1,0,1,0],
#     [1,1,0,1,0,0],
#     [0,1,0,0,0,0],
#     [0,0,1,0,0,0],
#     [4,1,0,2,0,0]
# ])

##### FIXME: set the actual xs
# sample_list = [
#     sample(1,1,1, val_xs = 1.000),
#     sample(1,1,0, val_xs = 1.000),
#     sample(1,0,1, val_xs = 1.000),
#     sample(1,0,0, val_xs = 1.000),
#     sample(0,1,0, val_xs = 1.000),
#     sample(1,0,2, val_xs = 1.000)
# ]

sample_list = [
    sample(0,1,0, val_xs = 0.016790554173),
    sample(1,0,0, val_xs = 0.032157781896),
    sample(1,0,1, val_xs = 0.0239103408526),
    sample(1,1,0, val_xs = 0.003968863499),
    sample(1,1,1, val_xs = 0.00151726045561),
    sample(1,0,2, val_xs = 0.0178529494906)
]

# sample_list = [
#     sample(1,  2,1, val_xs = 0.1111111111),
#     sample(1,  1,2, val_xs = 0.1111111111),
#     sample(1,  1,1, val_xs = 0.1111111111),
#     sample(1,  1,0, val_xs = 0.1111111111),
#     sample(1.5,1,1, val_xs = 0.1111111111),
#     sample(0.5,1,1, val_xs = 0.1111111111)
# ]



M = build_matrix(sample_list) ## automatically build it given a list of samples

##############################################

CV, C2V, kl, a, b, c, iab, iac, ibc, s1, s2, s3, s4, s5, s6 = symbols('CV C2V kl a b c iab iac ibc s1 s2 s3 s4 s5 s6')

### the vector of couplings
c = Matrix([
    [CV**2 * kl**2] ,
    [CV**4]         ,
    [C2V**2]        ,
    [CV**3 * kl]    ,
    [CV * C2V * kl] ,
    [CV**2 * C2V]   
])

### the vector of components
v = Matrix([
    [a]   ,
    [b]   ,
    [c]   ,
    [iab] ,
    [iac] ,
    [ibc]  
])


### the vector of samples (i.e. cross sections)
s = Matrix([
    [s1] ,
    [s2] ,
    [s3] ,
    [s4] ,
    [s5] ,
    [s6] 
])

####

Minv   = M.inv()
coeffs = c.transpose() * Minv # coeffs * s is the sigma, accessing per component gives each sample scaling
sigma  = coeffs*s

# print coeffs
# print coeffs*s

###### target values
t_CV  = 1
t_C2V = 5
t_kl  = 2

### to access the coefficients
print ">>> target : CV = %f , C2V = %f , kl = %f" % (t_CV, t_C2V, t_kl)
for i in range(0, 6):
    print ".... for sample number", i
    print coeffs[i]
    print coeffs[i].evalf(subs={CV : t_CV, C2V : t_C2V, kl : t_kl})

print "sigma = ", sigma[0].evalf(subs={
    CV  : t_CV,
    C2V : t_C2V,
    kl  : t_kl,
    s1  : sample_list[0].val_xs,
    s2  : sample_list[1].val_xs,
    s3  : sample_list[2].val_xs,
    s4  : sample_list[3].val_xs,
    s5  : sample_list[4].val_xs,
    s6  : sample_list[5].val_xs,
})

