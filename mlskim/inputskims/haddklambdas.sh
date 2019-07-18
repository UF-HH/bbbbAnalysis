#### 2016 reweight klambda
mkdir klambdarew_2016_8Lug2019_v2
rm klambdarew_2016_8Lug2019_v2/*.root

for kl in `seq -f %g -10 1 10`; do
     klname=${kl/./d}
     if [[ $klname == *-* ]]; then
         klname=${klname/-/m_};
     else
         klname=p_${klname}
     fi

     hadd klambdarew_2016_8Lug2019_v2/SKIM_GGHH4B_rew_kl_${klname}.root   `xrdfsls -u /eos/uscms/store/user/lcadamur/bbbb_ntuples/klambdarew_2016_8Lug2019_v2/SKIM_GGHH4B_rew_kl_${klname}/output/ | grep '\.root'`
done

#### 2017 reweight klambda
mkdir klambdarew_2017_8Lug2019_v2
rm klambdarew_2017_8Lug2019_v2/*.root

for kl in `seq -f %g -10 1 10`; do
     klname=${kl/./d}
     if [[ $klname == *-* ]]; then
         klname=${klname/-/m_};
     else
         klname=p_${klname}
     fi

     hadd klambdarew_2017_8Lug2019_v2/SKIM_GGHH4B_rew_kl_${klname}.root   `xrdfsls -u /eos/uscms/store/user/lcadamur/bbbb_ntuples/jobs_klambdarew_2017_8Lug2019_v2/SKIM_GGHH4B_rew_kl_${klname}/output/ | grep '\.root'`
done
