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


def calculatecoeffients(sample_list,couplingstoscan):
  
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

    with open('vbfhhc2vline.txt', 'w') as the_file:
     for j in range(0, len(couplingstoscan)):
         ###### target values
         couplings = couplingstoscan[j]
         t_CV  = couplings[0]
         t_C2V = couplings[1]
         t_kl  = couplings[2]  
         xsec  = sigma[0].evalf(subs={
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
         factors = [coeffs[x].evalf(subs={CV : t_CV, C2V : t_C2V, kl : t_kl}) for x in range(0,6)]
         for k in range(0, len(factors) ):
            if abs(factors[k]) == 0: factors[k]=0 
            if abs(factors[k]) < 0.001: factors[k]=0 
         print ">>> XS (fb) = %f, target (CV,C2V,kl) = (%.2f,%.1f, %.1f)" %((xsec)/(1), t_CV, t_C2V, t_kl )  
         the_file.write('%.2f %.6f \n'%(t_C2V,(xsec)/(1)))

#################################################################################################
print "WELCOME TO THE VBF-HH CALCULATOR"
#################################################################################################
#Create an array with samples couplings(cv,c2v,kl) and their L0 xs
sample_list = [
    sample(1,1,1,   val_xs = 0.000585),
    sample(1,2,1,   val_xs = 0.004688),
    sample(1,1,2,   val_xs = 0.000482),
    sample(1,1,0,   val_xs = 0.001585),
    sample(0.5,1,1, val_xs = 0.003586),
    sample(1.5,1,1, val_xs = 0.022297)
]  
#Create an array with couplings (cv,c2v,kl) to scan. In our case is c2v scan.
couplingstoscan_list = [ [1,-5.0 + (x*0.1),1] for x in range(0,121)]     
#Compute coefficients to be used during histogram building and save them in a text file
print "[INFO] VBF-HH CALCULATOR RESULTS"
calculatecoeffients(sample_list,couplingstoscan_list)
