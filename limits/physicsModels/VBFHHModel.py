from HiggsAnalysis.CombinedLimit.PhysicsModel import *
from sympy import *
from numpy import matrix
from numpy import linalg
from sympy import Matrix

class VBFHHSample:
    def __init__(self, val_CV, val_C2V, val_kl, val_xs, label):
        self.val_CV  = val_CV
        self.val_C2V = val_C2V
        self.val_kl  = val_kl
        self.val_xs  = val_xs
        self.label   = label

class VBFHHModel(PhysicsModel):
    """ Models the VBF HH production as linear sum of 6 components """
    
    def __init__(self, sample_list, name):
        PhysicsModel.__init__(self)
        
        if len(sample_list) != 6:
            raise RuntimeError("malformed input sample list - expect 6 samples")
        if not isinstance(sample_list, list) and not isinstance(sample_list, tuple):
            raise RuntimeError("malformed input sample list - expect list or tuple")
        for s in sample_list:
            if not isinstance(s, VBFHHSample):
                raise RuntimeError("malformed input sample list - each element must be a VBFHHSample")
        self.sample_list = sample_list
        self.name = name
        # print what is being done
        
        print "---" , self.name, "---- This VBF HH model is built with:"
        for i,s in enumerate(self.sample_list):
            print " {0:<3} ... CV : {1:<3}, C2V : {2:<3}, kl : {3:<3}, xs : {4:<8} pb, label : {5}".format(i, s.val_CV, s.val_C2V, s.val_kl, s.val_xs, s.label)

        self.build_matrix()
        self.calculatecoeffients()

    def build_matrix(self):    
        """ create the matrix M in this object """

        if len(self.sample_list) != 6:
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

        for isample, sample in enumerate(self.sample_list):
            # print isample, " CV, C2V, kl = ", sample.val_CV, sample.val_C2V, sample.val_kl
            
            ## implement the 6 scalings
            M_tofill[isample][0] = sample.val_CV**2 * sample.val_kl**2
            M_tofill[isample][1] = sample.val_CV**4
            M_tofill[isample][2] = sample.val_C2V**2
            M_tofill[isample][3] = sample.val_CV**3 * sample.val_kl
            M_tofill[isample][4] = sample.val_CV    * sample.val_C2V    * sample.val_kl
            M_tofill[isample][5] = sample.val_CV**2 * sample.val_C2V
        
        # print M_tofill
        self.M = Matrix(M_tofill)


    def calculatecoeffients(self):
        """ create the function sigma and the six coefficients in this object """

        try: self.M
        except AttributeError: self.build_matrix()
        
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
        Minv   = self.M.inv()
        self.coeffs = c.transpose() * Minv # coeffs * s is the sigma, accessing per component gives each sample scaling
        self.sigma  = self.coeffs*s

    def doParametersOfInterest(self):
        POIs = "r,CV,C2V,kl"
        self.modelBuilder.doVar("r[1,0,10]")
        self.modelBuilder.doVar("CV[1,-10,10]")
        self.modelBuilder.doVar("C2V[1,-10,10]")
        self.modelBuilder.doVar("kl[1,-30,30]")
        self.modelBuilder.doSet("POI",POIs)

        self.modelBuilder.out.var("CV").setConstant(True)
        self.modelBuilder.out.var("C2V").setConstant(True)
        self.modelBuilder.out.var("kl").setConstant(True)

        self.create_scalings()

    def create_scalings(self):
        """ create the functions that scale the six components """

        self.f_r_names = [] # the RooFormulae that scale the components

        for i, s in enumerate(self.sample_list):
            f_name = 'f_vbfhhscale_sample_{0}'.format(i)
            f_expr = self.coeffs[i] # the function that multiplies each sample

            # print f_expr
            # for ROOFit, this will convert expressions as a**2 to a*a
            def pow_to_mul_string(expr):
                """ Convert integer powers in an expression to Muls, like a**2 => a*a. Returns a string """
                pows = list(expr.atoms(Pow))
                if any(not e.is_Integer for b, e in (i.as_base_exp() for i in pows)):
                    raise ValueError("A power contains a non-integer exponent")
                s = str(expr)
                repl = zip(pows, (Mul(*[b]*e,evaluate=False) for b,e in (i.as_base_exp() for i in pows)))
                for fr, to in repl:
                    s = s.replace(str(fr), str(to))
                return s
            s_expr = pow_to_mul_string(f_expr)

            couplings_in_expr = []
            if 'CV'  in s_expr: couplings_in_expr.append('CV')
            if 'C2V' in s_expr: couplings_in_expr.append('C2V')
            if 'kl'  in s_expr: couplings_in_expr.append('kl')

            # no constant expressions are expected
            if len(couplings_in_expr) == 0:
                raise RuntimeError('VBF HH : scaling expression has no coefficients')
            
            for idx, ce in enumerate(couplings_in_expr):
                # print '..replacing', ce
                symb = '@{}'.format(idx)
                s_expr = s_expr.replace(ce, symb)

            arglist = ','.join(couplings_in_expr)
            exprname = 'expr::{}(\"{}\" , {})'.format(f_name, s_expr, arglist)
            # print exprname
            self.modelBuilder.factory_(exprname) # the function that scales each sample

            f_prod_name = f_name + '_r'
            prodname = 'prod::{}(r,{})'.format(f_prod_name, f_name)
            self.modelBuilder.factory_(prodname)
            # self.modelBuilder.out.function(f_prod_name).Print("") ## will just print out the values

            self.f_r_names.append(f_prod_name) #bookkeep the scaling that has been created
    
    def getYieldScale(self,bin,process):
		
        if not self.DC.isSignal[process]: return 1
        # match the process name in the datacard to the input sample of the calculation
        # this is the only point where the two things must be matched

        for isample, sample in enumerate(self.sample_list):
            if process.startswith(sample.label):
                return self.f_r_names[isample]

        raise RuntimeError('VBF HH : could not match the process %s to the expected sample list' % process)

## create the objects that will be called in combine

## the expected default - NOTE : the field "label" identifies the way the sample is expected to start in the datacard
sample_list = [
    VBFHHSample(1,1,1,   val_xs = 0.00054, label = 'qqHH_CV_1_C2V_1_C3_1'  ),
    VBFHHSample(1,2,1,   val_xs = 0.00472, label = 'qqHH_CV_1_C2V_2_C3_1'  ),
    VBFHHSample(1,1,2,   val_xs = 0.00044, label = 'qqHH_CV_1_C2V_1_C3_2'  ),
    VBFHHSample(1,1,0,   val_xs = 0.00145, label = 'qqHH_CV_1_C2V_1_C3_0'  ),
    VBFHHSample(0.5,1,1, val_xs = 0.00353, label = 'qqHH_CV_0p5_C2V_1_C3_1'),
    VBFHHSample(1.5,1,1, val_xs = 0.02149, label = 'qqHH_CV_1p5_C2V_1_C3_1'),
]
VBFHHdefault = VBFHHModel(sample_list, 'VBFHHdefault')