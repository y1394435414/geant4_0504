#include "event.hh"
#include "G4UIcommand.hh"

namespace {
G4int GetSCArrayTotalCount()
{
    G4int total = 0;
    for (const auto& group : MyDetectorConstruction::fSCArrayGroups) {
        total += group.count;
    }
    return total;
}
}

MyEventAction::MyEventAction(MyRunAction*){
    fEdep=0;
    fnumber=0;

}
MyEventAction::~MyEventAction(){}
void MyEventAction::BeginOfEventAction(const G4Event*){
fEdep=0;
fnumber=0;
if(MyDetectorConstruction::scintillatorArrangement=="SCARRAY"){
    const G4int totalScintillators = GetSCArrayTotalCount();
    fSCArrayEdep.assign(totalScintillators, 0.);
    fSCArrayNoiseEdep.assign(totalScintillators, 0.);
}
}
void MyEventAction::EndOfEventAction(const G4Event* event){

    if(MyDetectorConstruction::scintillatorArrangement=="SC"){
        G4int evt =G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();


        G4cout<<"Evento: "<< evt<< G4endl;
        G4cout<<"Energy deposition: "<<G4BestUnit(fEdep,"Energy")<<G4endl;


        //Passes the energy Deposition  to a histogram 
        G4AnalysisManager *man=G4AnalysisManager::Instance();
        man->FillH1(0,fEdep);
        meanTF= man->GetH1(15)->mean();
        G4cout<<"Average TOF:"<<meanTF<<G4endl;
        G4cout<<"Average TOF:"<<G4BestUnit(meanTF,"Time")<<G4endl;
        man->FillH1(16,meanTF);
        man->GetH1(15)->reset();
        //G4cout<<"mean TOF:"<<G4BestUnit(man->GetH1(15)->mean(),"Time")<<G4endl;
        //G4cout<<"mean TOF:"<<man->GetH1(15)->mean()<<G4endl;

        
        //G4MUTEXLOCK(&mutex );
        meanNF= man->GetH1(17)->mean();
        //total = meanNF-n;
        /*total2=total2+total;
        man->GetH1(19)->reset();
        man->FillH1(19,total2);*/
        //n=meanNF;
        man->FillH1(18,meanNF);
        
        
        G4cout<<"Number of Photons: "<<meanNF<<G4endl;
    }else if(MyDetectorConstruction::scintillatorArrangement=="SCARRAY"){
        G4int evt = event->GetEventID();
        G4cout<<"Evento: "<< evt<< G4endl;

        const G4int totalScintillators = GetSCArrayTotalCount();
        if (static_cast<G4int>(fSCArrayEdep.size()) != totalScintillators) {
            fSCArrayEdep.assign(totalScintillators, 0.);
            fSCArrayNoiseEdep.assign(totalScintillators, 0.);
        }

        for (G4int i = 0; i < totalScintillators; i++) {
            G4String name = "SCARRAY_" + G4UIcommand::ConvertToString(i);
            fillingNtuples(name, i, fSCArrayEdep[i], fSCArrayNoiseEdep[i], event);
        }
        getSecondariesInfo(event, totalScintillators * 3);
    }
    //G4MUTEXUNLOCK(&mutex );
    


}

void MyEventAction::AddEdepSCArray(G4int scintillatorIndex, G4double edep, G4bool isNoise)
{
    if (scintillatorIndex < 0) {
        return;
    }

    if (static_cast<G4int>(fSCArrayEdep.size()) <= scintillatorIndex) {
        fSCArrayEdep.resize(scintillatorIndex + 1, 0.);
        fSCArrayNoiseEdep.resize(scintillatorIndex + 1, 0.);
    }

    if (isNoise) {
        fSCArrayNoiseEdep[scintillatorIndex] += edep;
    } else {
        fSCArrayEdep[scintillatorIndex] += edep;
    }
}

 void MyEventAction::fillingNtuples(G4String name, G4int scintillatorPosition, G4double fedep, G4double fedepnoise,const G4Event* event){
    G4AnalysisManager *man=G4AnalysisManager::Instance();
    //scintillatorPosition=scintillatorPosition*3;
    G4int eventId=event->GetEventID();
    fedep = fedep/MeV;
    fedepnoise = fedepnoise/MeV;
    man->FillNtupleDColumn(scintillatorPosition*3+1,0, fedep);
    man->FillNtupleDColumn(scintillatorPosition*3+1,3, fedepnoise);

    G4double meanTF = man->GetH1(scintillatorPosition*2)->mean();
    man->GetH1(scintillatorPosition*2)->reset();
    man->FillNtupleDColumn(scintillatorPosition*3+1,1, meanTF);
    
    G4double meanNF = man->GetH1(scintillatorPosition*2+1)->mean();
    man->GetH1(scintillatorPosition*2+1)->reset();
    man->FillNtupleDColumn(scintillatorPosition*3+1,2, meanNF);

    man->FillNtupleIColumn(scintillatorPosition*3+1,4, eventId);
    man->AddNtupleRow(scintillatorPosition*3+1);

    name = "Energy Deposition  "+ name;
    G4cout<<name+": "<<G4BestUnit(fedep,"Energy")<<G4endl;
    G4cout<<name+" by noise: "<<G4BestUnit(fedepnoise,"Energy")<<G4endl;
   
 }

 void getSecondariesInfo(const G4Event* event, G4int ntupleId) {
    G4TrajectoryContainer* trajectoryContainer = event->GetTrajectoryContainer();
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    if (!trajectoryContainer) {
        G4cout<<"No secondaries generated in event  "<<event->GetEventID()<<G4endl;
        return;}

    for (int i = 0; i < trajectoryContainer->entries(); ++i) {
        G4VTrajectory* trajectory = static_cast<G4VTrajectory*>((*trajectoryContainer)[i]);
       // G4cout<<trajectory->GetParticleName()<<"ID: "<<trajectory->GetParentID()<<G4endl;
        // Verificar si la trayectoria es secundaria
        if (trajectory->GetParentID() != 0) {
            G4VTrajectoryPoint* vertex = trajectory->GetPoint(0);
            G4ThreeVector pos = vertex->GetPosition();
            G4int trackID = trajectory->GetTrackID();
            G4String particleName = trajectory->GetParticleName();
            G4double energy = trajectory->GetInitialMomentum().mag(); 
            G4int eventID = event->GetEventID();
            man->FillNtupleIColumn(ntupleId,0,eventID);
            man->FillNtupleIColumn(ntupleId,1,trackID);
            man->FillNtupleDColumn(ntupleId,2,pos.x()/mm);
            man->FillNtupleDColumn(ntupleId,3,pos.y()/mm);
            man->FillNtupleDColumn(ntupleId,4,pos.z()/mm);
            man->FillNtupleDColumn(ntupleId,5,energy/MeV);
            man->FillNtupleSColumn(ntupleId,6,particleName);
            man->AddNtupleRow(ntupleId);
        }
    }
}
