#make 1d likelihoods
source make_likelihood_1d.sh run2 KL
source make_likelihood_1d.sh run2 C2V
#make 2d likelihoods
source make_likelihood_2d.sh run2 1 C2V-CV
source make_likelihood_2d.sh run2 2 KL-C2V

