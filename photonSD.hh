#ifndef PHOTONSD_HH
#define PHOTONSD_HH

#include "G4VSensitiveDetector.hh"
#include "G4AnalysisManager.hh"
//#include "g4root.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "event.hh"
#include "construction.hh"
#include "G4UnitsTable.hh"
#include <vector>

class MyPhotoDetector: public G4VSensitiveDetector{
    public:
    MyPhotoDetector(G4String);
    ~MyPhotoDetector();
    void NtupleFilling(G4String scintillatorName, G4int scintillatorPosition, G4int& evtNumber,G4int& numberOfPhotons,G4Step *aStep);
    
    
    private:
    virtual G4bool ProcessHits(G4Step *,G4TouchableHistory *);
    static G4int noc;
    G4int numberOfPhotons=0,evt=0;
    std::vector<G4int> evtSCArray;
    std::vector<G4int> npSCArray;
};


#endif
