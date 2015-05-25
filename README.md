# pythia-flow
My first tests to run pythia on a HTCondor facility using Makeflow.

## Usage
Generate your job structure description: 
```
python generateMakeFlow.py
```
then if you are in a HTCondor facility, and provided you already have installed the CCT suite: 
```
condor_submit_makeflow pythia.makeflow
```
_Currently the code does not produce any meaningful results, I am still working on it_

## DISCLAIMER
All the code here is released under GPL 2.0. You can try, edit, redistribute etc. etc. all the 
code but beware, the code is highly experimental (i.e. it does not work at the time I am writing this) 
therefore I won't respond of any damage to your hardware, software or psychic state.
