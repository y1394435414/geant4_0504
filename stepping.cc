#include "stepping.hh"
#include "G4VPhysicalVolume.hh"
MySteppingAction::MySteppingAction(MyEventAction *eventAction){
    fEventAction=eventAction;
}

MySteppingAction::~MySteppingAction(){

}
void MySteppingAction::UserSteppingAction(const G4Step *step){
    if(MyDetectorConstruction::scintillatorArrangement == "SC"){
        G4LogicalVolume *volume=step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
        const MyDetectorConstruction *detectorConstruction = static_cast<const MyDetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        G4LogicalVolume *fScoringVolume= detectorConstruction->GetScoringVolume();
        if (volume != fScoringVolume)
        { 
            return;
        }
        
        //Acumulates the energy in the step using the method created in event.hh
        G4double edep = step->GetTotalEnergyDeposit();
        fEventAction->AddEdep(edep);
    
    
    }
    if(MyDetectorConstruction::scintillatorArrangement == "SCARRAY"){
        G4VPhysicalVolume* physicalVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
        G4int copyNumber = physicalVolume->GetCopyNo();

        if (copyNumber < 2 || (copyNumber - 2) % 4 != 0) {
            return;
        }

        G4double edep = step->GetTotalEnergyDeposit();
        if (edep <= 0.) {
            return;
        }

        G4Track* track = step->GetTrack();
        G4ParticleDefinition* particleDefinition = track->GetDefinition();
        G4String particleName = particleDefinition->GetParticleName();
        if (particleName == "opticalphoton") {
            return;
        }

        G4int scintillatorIndex = (copyNumber - 2) / 4;
        G4bool isNoise = track->GetParentID() != 0;
        fEventAction->AddEdepSCArray(scintillatorIndex, edep, isNoise);
    }

}
