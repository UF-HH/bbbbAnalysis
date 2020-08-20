from sympy import *
from numpy import matrix
from numpy import linalg
from sympy import Matrix

class sample:
    def __init__(self, val_kl, val_kt, val_xs):
        self.val_kl  = val_kl
        self.val_kt  = val_kt
        self.val_xs  = val_xs

def build_matrix(sample_list):    
    if len(sample_list) != 3:
        print "[ERROR] : expecting 3 samples in input"
        raise RuntimeError("malformed input sample list")
    M_tofill = [
            [None,None,None],
            [None,None,None],
            [None,None,None],
        ]

    for isample, sample in enumerate(sample_list):
        ## implement the 3 scalings - box, triangle, interf
        M_tofill[isample][0] = sample.val_kt**4
        M_tofill[isample][1] = sample.val_kt**2 * sample.val_kl**2
        M_tofill[isample][2] = sample.val_kt**3 * sample.val_kl    
    # print M_tofill

    res = Matrix(M_tofill)
    return res

def calculatecoeffients(sample_list,couplingstoscan):
  
    M = build_matrix(sample_list) ## automatically build it given a list of samples    

    ##############################################    
    kl, kt, box, tri, interf, s1, s2, s3 = symbols('kl kt box tri interf s1 s2 s3')   
    ### the vector of couplings
    c = Matrix([
        [kt**4]         ,
        [kt**2 * kl**2] ,
        [kt**3 * kl]    ,
    ])
    ### the vector of components
    v = Matrix([
        [box]   ,
        [tri]   ,
        [interf],
    ])    
    ### the vector of samples (i.e. cross sections)
    s = Matrix([
        [s1] ,
        [s2] ,
        [s3]
    ])     
    ####    
    Minv   = M.inv()
    coeffs = c.transpose() * Minv # coeffs * s is the sigma, accessing per component gives each sample scaling
    sigma  = coeffs*s  

    with open('config/ggfhhc3line.txt', 'w') as the_file:
     for j in range(0, len(couplingstoscan)):
         ###### target values
         couplings = couplingstoscan[j]
         t_kl  = couplings[0]
         t_kt  = couplings[1]
         xsec  = sigma[0].evalf(subs={
             kl  : t_kl,
             kt  : t_kt,
             s1  : sample_list[0].val_xs,
             s2  : sample_list[1].val_xs,
             s3  : sample_list[2].val_xs,
         }) 
         factors = [coeffs[x].evalf(subs={kl : t_kl, kt : t_kt}) for x in range(0,3)]
         #for k in range(0, len(factors) ):
         #   if abs(factors[k]) == 0: factors[k]=0 
         #   if abs(factors[k]) < 0.001: factors[k]=0 
         print ">>> XS(fb) = %f, target (kt,kt) = (%.2f,%.2f), coefficients (c1,c2,c3) = (%.5f,%.5f,%.5f)" %((xsec)/(1), t_kl, t_kt,factors[0],factors[1],factors[2])  
         the_file.write('%.2f %.5f\n'%(t_kl,(xsec)/(1)))

#################################################################################################
print "WELCOME TO THE GGF-HH CALCULATOR"
#################################################################################################
#Create an array with samples couplings(cv,c2v,kl) and their L0 xs
sample_list = [
    sample(1,1,  val_xs = 0.010517),
    sample(0,1,     val_xs = 0.023618),
    #sample(2.45,1,  val_xs = 0.004455),
    sample(5,1,     val_xs = 0.031072),
]  
#Create an array with couplings (cv,c2v,kl) to scan. In our case is c2v scan.
couplingstoscan_list = [ [-28 + (x*0.5),1] for x in range(0,113)]     
#Compute coefficients to be used during histogram building and save them in a text file
print "[INFO] VBF-HH CALCULATOR RESULTS"
calculatecoeffients(sample_list,couplingstoscan_list)
