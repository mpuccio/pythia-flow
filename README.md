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
_This is for testing purpose, if you are interested in the coalescence part look below_

### Coalescence generator
To compile and run the generator:
``` 
make coalescence
./coalescence command.cmnd out.txt
```
the output file is a list of all the (anti-)deuteron generated in this format:
```
pdg   pT  pz  eta 
```

## DISCLAIMER
All the code here is released under GPL 2.0. You can try, edit, redistribute etc. etc. all the 
code but beware, the code is highly experimental (i.e. it does not work at the time I am writing this) 
therefore I won't respond of any damage to your hardware, software or psychic state.
