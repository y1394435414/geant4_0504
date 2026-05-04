#include "photonSD.hh"
#include "G4UIcommand.hh"

G4int MyPhotoDetector::noc;

namespace {
G4int GetSCArraySensorIndex(G4int copyNumber)
{
    if (copyNumber >= 3 && (copyNumber - 3) % 4 == 0) {
        return (copyNumber - 3) / 4;
    }
    if (copyNumber >= 4 && copyNumber % 4 == 0) {
        return (copyNumber - 4) / 4;
    }
    return -1;
}
}

MyPhotoDetector::MyPhotoDetector(G4String name):G4VSensitiveDetector(name)
{   
};

MyPhotoDetector::~MyPhotoDetector(){

}
G4bool MyPhotoDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist){
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    
    if(MyDetectorConstruction::scintillatorArrangement == "SC"){
        
    G4StepPoint *preStepPoint=aStep->GetPreStepPoint();
    G4StepPoint *postStepPoint=aStep->GetPostStepPoint();
    G4ThreeVector posPhoton=preStepPoint->GetPosition();
    G4double posmodule, posmodule2;
    G4Track *track=aStep->GetTrack();
    track->SetTrackStatus(fStopAndKill);
if (track) {
        const G4ParticleDefinition* particleDefinition = track->GetDefinition();
        if (particleDefinition) {
            G4String particleName = particleDefinition->GetParticleName();
            //G4cout << "Nombre de la partícula: " << particleName << G4endl;
            
        }
    }
    
    G4ThreeVector momPhoton=preStepPoint->GetMomentum();
    G4double energy =momPhoton.mag();
    G4double wlen= ((1.239841939*eV*um)/momPhoton.mag());
    
    
    G4double time = preStepPoint->GetLocalTime();
     //Only if Multithreaded mode is not being used

   #ifndef G4MULTITHREADED
    G4cout<<"TOF:  "<<G4BestUnit(time,"Time")<<G4endl;
    G4cout<<"TOF:  "<<time<<G4endl;

    #endif

    
    G4int idEvt =G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    if(idEvt==evt){
        numberOfPhotons++;
        man->GetH1(17)->reset();
        man->FillH1(17,numberOfPhotons);
    } else{
        evt=idEvt;
        numberOfPhotons=1;
        man->GetH1(17)->reset();
        man->FillH1(17,numberOfPhotons);
    }
    
    
    
    
    
    man->FillH1(1,wlen);
    man->FillH1(2,idEvt);
    man->FillH1(3,energy);
    if(idEvt<10){
    man->FillH1(4,idEvt);
    }
    
    if(idEvt<10){
    man->FillH1(idEvt+5,time);
    }
    man->FillH1(15,time);

    
   man->FillH2(0,energy, wlen);
    }
    if(MyDetectorConstruction::scintillatorArrangement == "SCARRAY"){
        G4VPhysicalVolume* physicalVolume = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
        G4int copyNumber = physicalVolume->GetCopyNo();
        G4int scintillatorIndex = GetSCArraySensorIndex(copyNumber);
        if (scintillatorIndex < 0) {
            return true;
        }

        G4Track *track=aStep->GetTrack();
        const G4ParticleDefinition* particleDefinition = track->GetDefinition();
        G4String particleName = particleDefinition->GetParticleName();
        if(particleName != "opticalphoton"){
            return true;
        }

        track->SetTrackStatus(fStopAndKill);
        if (static_cast<G4int>(evtSCArray.size()) <= scintillatorIndex) {
            evtSCArray.resize(scintillatorIndex + 1, 0);
            npSCArray.resize(scintillatorIndex + 1, 0);
        }

        G4String scintillatorName = "SCARRAY_" + G4UIcommand::ConvertToString(scintillatorIndex);
        NtupleFilling(scintillatorName, scintillatorIndex, evtSCArray[scintillatorIndex], npSCArray[scintillatorIndex], aStep);
    }
return true;}

void MyPhotoDetector::NtupleFilling(G4String scintillatorName, G4int scintillatorPosition, G4int& evtNumber, G4int& numberOfPhotons,G4Step *aStep){
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    
    G4Track *track=aStep->GetTrack();
    track->SetTrackStatus(fStopAndKill);
    G4StepPoint *preStepPoint=aStep->GetPreStepPoint();
    G4StepPoint *postStepPoint=aStep->GetPostStepPoint();
    const G4ParticleDefinition* particleDefinition = track->GetDefinition();
    G4String particleName = particleDefinition->GetParticleName();

    
    
    G4ThreeVector momPhoton=preStepPoint->GetMomentum();
    G4double energy =momPhoton.mag();
    G4double wlen= ((1.239841939*eV*um)/momPhoton.mag());
    G4int idEvt =G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

    wlen=wlen/nm;
    energy=energy/eV;
    man->FillNtupleIColumn(scintillatorPosition*3,0,idEvt);
    man->FillNtupleDColumn(scintillatorPosition*3,1,wlen);
    man->FillNtupleDColumn(scintillatorPosition*3,2,energy);
    

    G4double time = preStepPoint->GetLocalTime();
    time = time/ns;
 //   scintillatorPosition = scintillatorPosition*3; //3 es el numero total de histogramas
    man->FillNtupleDColumn(scintillatorPosition*3,3,time);
    man->AddNtupleRow(scintillatorPosition*3);
    man->FillH1(scintillatorPosition*2,time);
    
    if(idEvt==evtNumber){
        numberOfPhotons++;
        
        man->GetH1(scintillatorPosition*2+1)->reset();
        man->FillH1(scintillatorPosition*2+1,numberOfPhotons);
    } else{
        
        evtNumber=idEvt;
        numberOfPhotons=1;
        man->GetH1(scintillatorPosition*2+1)->reset();
        man->FillH1(scintillatorPosition*2+1,numberOfPhotons);
    }


    }
