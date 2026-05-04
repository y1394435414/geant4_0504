#include "trajectory.hh"

MyTrackingAction::MyTrackingAction(){}

MyTrackingAction::~MyTrackingAction(){}

void MyTrackingAction::PreUserTrackingAction(const G4Track* track){

    
    const G4ParticleDefinition* particle = track->GetDefinition();
    G4TrackingManager* trackingManager = G4EventManager::GetEventManager()->GetTrackingManager();

    if (particle->GetParticleName() == "opticalphoton") {
        trackingManager->SetStoreTrajectory(0);
    } else if (MyDetectorConstruction::scintillatorArrangement == "SCARRAY") {
        trackingManager->SetStoreTrajectory(1);
    } else if (track->GetParentID()==0) {
        trackingManager->SetStoreTrajectory(0);
    } else {
        trackingManager->SetStoreTrajectory(1);
    }
}
