#include "barSD.hh"

MyBarDetector::MyBarDetector(G4String name):G4VSensitiveDetector(name){

}

MyBarDetector::~MyBarDetector(){

}

G4bool MyBarDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist){
    
    

    G4Track *track=aStep->GetTrack();
    const G4ParticleDefinition* particleDefinition = track->GetDefinition();
    G4String particleName = particleDefinition->GetParticleName();
   
    if(particleName == "opticalphoton" || track->GetParentID()==0){
        return true;
    }
    
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    G4VPhysicalVolume* physicalVolume = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
    G4int copyNumber = physicalVolume->GetCopyNo();
    G4StepPoint *preStepPoint=aStep->GetPreStepPoint();
    G4StepPoint *postStepPoint=aStep->GetPostStepPoint();
    G4double time = preStepPoint->GetLocalTime();
    time = time/ns;   

    G4ThreeVector pos = preStepPoint->GetPosition();
    G4int trackID =  aStep->GetTrack()->GetTrackID();
    

    G4int idEvt =G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    if(MyDetectorConstruction::scintillatorArrangement == "SCARRAY"){
        if(copyNumber >= 2 && (copyNumber - 2) % 4 == 0){
            G4int scintillatorIndex = (copyNumber - 2) / 4;
            G4int ntupleId = scintillatorIndex * 3 + 2;
            man->FillNtupleDColumn(ntupleId,0,pos.x()/mm);
            man->FillNtupleDColumn(ntupleId,1,pos.y()/mm);
            man->FillNtupleDColumn(ntupleId,2,pos.z()/mm);
            man->FillNtupleIColumn(ntupleId,3,idEvt);
            man->FillNtupleIColumn(ntupleId,4,trackID);
            man->FillNtupleDColumn(ntupleId,5,time);
            man->AddNtupleRow(ntupleId);
        }
        return true;
    }
    return true;

}
