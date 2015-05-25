// Coalescence.cc is a standalone implementation of coalescence models for PYTHIA
// the code is inspired to the AliROOT implementation made by Eulogio Serradilla Rodriguez
// and Arturo Menchaca.

// Author: Maximiliano Puccio <maximiliano.puccio@cern.ch>
// This code is a modified version of the example main42.cc that can be found in the PYTHIA code.
// The original author of the macro is 

#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include "Pythia8/Pythia.h"
#include "Pythia8/Basics.h"
#include "Pythia8/Event.h"

using namespace Pythia8;
using std::vector;

const float kDeuteronMass = 1.87561282;
const float kProtonMass  = 0.938272013f;
const float kNeutronMass = 0.939565378f;
const float kDeltaMass = kProtonMass - kNeutronMass;
const int   kDeuteronPDG = 1000010020;
const float kSpinProbability = 1.;
const float kMaxDeltaP = 0.2f; // 200 MeV / c

#define Sq(x) ((x)*(x))

struct Candidate {
  static bool CompareCandidates(const Candidate &lhs, const Candidate &rhs) {
    return lhs.probability < rhs.probability;
  }

  Particle *p;
  Particle *n;
  float     probability;
};

float CoalescenceProbability(Particle* a, Particle* b) {
  //
  // Coalescence conditions as in
  // A. J. Baltz et al., Phys. lett B 325(1994)7
  //
  // returns < 0 if coalescence is not possible
  // otherwise returns a coalescence probability
  //

  const Vec4 &ap = a->p();
  const Vec4 &bp = b->p();

  const float e1 = sqrt(kProtonMass * kProtonMass + ap.pAbs2());
  const float e2 = sqrt(kNeutronMass * kNeutronMass + bp.pAbs2());

  const float s = Sq(e1 + e2) - (Sq(ap.px() + bp.px()) + Sq(ap.py() + bp.py()) + Sq(ap.pz() + bp.pz()));
  const float deltaP = sqrt(Sq(s - Sq(kDeltaMass)) / (4.f * s)); // relative momentum in CM frame
  if (deltaP >= kMaxDeltaP) 
    return -1.;
  else 
    return 1.f - (deltaP / kMaxDeltaP);
}

void CoalescenceLoop(vector<Particle*> protons, vector<Particle*> neutrons, int pdg, ofstream &of, Rndm &rnd) {
  vector<Candidate> candidates(neutrons.size() * protons.size());
  size_t nCand = 0;
  for (size_t iN = 0; iN < neutrons.size(); ++iN) {
    for (size_t iP = 0; iP < protons.size(); ++iP) {
      float prob = CoalescenceProbability(protons[iP],neutrons[iN]);
      if (prob > 0) {
        candidates[nCand].p = protons[iP]; 
        candidates[nCand].n = neutrons[iN];
        candidates[nCand].probability = prob;
        ++nCand;
      }
    }
  }
  candidates.resize(nCand);
  std::sort(candidates.begin(), candidates.end(),Candidate::CompareCandidates);
  int nDeut = 0;
  for (size_t iC = 0; iC < candidates.size(); ++iC) {
      /// Coalescence check
    if (rnd.flat() <= candidates[iC].probability * kSpinProbability) {
        /// Removal of the duplicates
      for (size_t iR = iC + 1; iR < candidates.size(); ++iR) {
        if (candidates[iR].p == candidates[iC].p || candidates[iR].n == candidates[iC].n) {
          candidates[iR].probability = -1.;
        }
      }
      const Vec4 &p0 = candidates[iC].p->p();
      const Vec4 &p1 = candidates[iC].n->p();
      Vec4 res = p0 + p1;
      res.e(sqrt(res.pAbs2() + kDeuteronMass * kDeuteronMass));
      nDeut++;
      of << pdg << "\t" << res.pT() << "\t" << res.pz() << "\t" << res.eta() << "\n";
    }
  }
  // of << neutrons.size() << "\t" << protons.size() << "\t" << nDeut << endl;
}

int main(int argc, char* argv[]) {

  // Check that correct number of command-line arguments
  if (argc != 3) {
    cerr << " Unexpected number of command-line arguments. \n You are"
    << " expected to provide one input and one output file name. \n"
    << " Program stopped! " << endl;
    return 1;
  }

  // Check that the provided input name corresponds to an existing file.
  ifstream is(argv[1]);
  if (!is) {
    cerr << " Command-line file " << argv[1] << " was not found. \n"
    << " Program stopped! " << endl;
    return 1;
  }

  // Confirm that external files will be used for input and output.
  cout << "\n >>> PYTHIA settings will be read from file " << argv[1]
  << " <<< \n >>> HepMC events will be written to file "
  << argv[2] << " <<< \n" << endl;

  // Interface for conversion from Pythia8::Event to HepMC event.

  // Specify file where HepMC events will be stored.
  std::ofstream ascii_io(argv[2], std::ios::out);

  // Generator.
  Pythia pythia;
  
  // Read in commands from external file.
  pythia.readFile(argv[1]);

  // Extract settings to be used in the main program.
  int    nEvent    = pythia.mode("Main:numberOfEvents");
  int    nAbort    = pythia.mode("Main:timesAllowErrors");

  // Initialization.
  pythia.init();

  // Begin event loop.
  int iAbort = 0;
  int totEv = 0;
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    // Generate event.
    if (!pythia.next()) {
      // If failure because reached end of file then exit event loop.
      if (pythia.info.atEndOfFile()) {
        cout << " Aborted since reached end of Les Houches Event File\n";
        break;
      }

      // First few failures write off as "acceptable" errors, then quit.
      if (++iAbort < nAbort) 
        continue;
      cout << " Event generation aborted prematurely, owing to error!\n";
      break;
    }
    ++totEv;
    vector<Particle*> neutrons;
    vector<Particle*> protons;
    vector<Particle*> antineutrons;
    vector<Particle*> antiprotons;
    for (int i = 0; i < pythia.event.size(); ++i) {
      Particle &current = pythia.event[i];
      if (current.isFinal()) { 
        switch (current.id()) {
          case 2112:
          neutrons.push_back(&current);
          break;
          case -2112:
          antineutrons.push_back(&current);
          break;
          case 2212:
          protons.push_back(&current);
          break;
          case -2212:
          antiprotons.push_back(&current);
          break;
        }
      }
    }

    /// Coalescence loop for deuterons
    CoalescenceLoop(protons,neutrons,kDeuteronPDG,ascii_io,pythia.rndm);
    /// Coalescence loop for anti-deuterons
    CoalescenceLoop(antiprotons,antineutrons,kDeuteronPDG,ascii_io,pythia.rndm);
    
  }
  ascii_io << "#" << totEv << endl;
  // Done.
  return 0;
}
