#ifndef EVENT_HH
#define EVENT_HH
#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "G4AnalysisManager.hh"
//#include "g4root.hh"
#include "run.hh"
#include "G4UnitsTable.hh"
#include "G4RunManager.hh"
#include "G4Threading.hh"
#include "G4AutoLock.hh"
#include "construction.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4VTrajectoryPoint.hh"
#include "G4ParticleDefinition.hh"
#include <vector>
class MyEventAction:public G4UserEventAction{
public:
MyEventAction(MyRunAction*);
~MyEventAction();
G4Mutex mutex; 
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);
    //Creation of a method that acumulates the energy deposition.
    void AddEdep(G4double edep){fEdep +=edep;}
    void AddNumber(){fnumber++;}
    void AddEdepSCArray(G4int scintillatorIndex, G4double edep, G4bool isNoise);
    void fillingNtuples(G4String name, G4int scintillatorPosition, G4double fedep, G4double fedepnoise,const G4Event* event);
private:
G4double fEdep,meanTF,meanNF,hola;
G4double fnumber;
G4int total=0,n=0,total2=0;

std::vector<G4double> fSCArrayEdep;
std::vector<G4double> fSCArrayNoiseEdep;
};

void getSecondariesInfo(const G4Event* event, G4int ntupleId = 63);
#endif
