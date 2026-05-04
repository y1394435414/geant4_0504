#include "generator.hh"
MyPrimaryGenerator::MyPrimaryGenerator(){
    fParticleGun = nullptr;
    particleSource = nullptr;

    if(MyDetectorConstruction::scintillatorArrangement=="SC"){
    fParticleGun = new G4ParticleGun(1); //One particle per event
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    
    G4ParticleDefinition *particle = particleTable->FindParticle(13);//13 corresponds to a muon in Geant4 database
    
    //The particle is generated in the origin with direction z+ and energy of 1 GeV
    G4ThreeVector pos(0.,0.,0.);
    G4ThreeVector mom(0.,0.,1.);
    fParticleGun->SetParticlePosition(pos);
    fParticleGun->SetParticleMomentumDirection(mom);
    fParticleGun->SetParticleMomentum(1.*GeV);
    fParticleGun->SetParticleDefinition(particle);

    } else if(MyDetectorConstruction::scintillatorArrangement=="SCARRAY"){
    particleSource = new G4GeneralParticleSource();
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition *particle = particleTable->FindParticle(13);
    particleSource->SetParticleDefinition(particle);
    particleSource->GetCurrentSource()->GetEneDist()->SetMonoEnergy(1.*GeV);
    particleSource->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0., 0., -50.*cm));
    particleSource->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
    }
}
MyPrimaryGenerator::~MyPrimaryGenerator(){
if(fParticleGun){
    delete fParticleGun;
}
if(particleSource){
    delete particleSource;
}
}
void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent){
    if(MyDetectorConstruction::scintillatorArrangement=="SC")
    {
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }
    if(MyDetectorConstruction::scintillatorArrangement=="SCARRAY")
    {
        particleSource->GeneratePrimaryVertex(anEvent);
    }
}

    
