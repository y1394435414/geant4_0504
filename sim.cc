#include <iostream>
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4MTRunManager.hh"
#include "G4UIExecutive.hh"
#include "construction.hh"
#include "physics.hh"
#include "action.hh"
#include "G4UIcommand.hh"
#include "G4RunManagerFactory.hh"
#include "G4String.hh"
#include "QGSP_BERT.hh"
#include "G4DecayPhysics.hh"
#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
//#include "G4Random.hh"
#include <ctime>



int main(int argc,char** argv){

    G4long seed = static_cast<G4long>(time(0));

    // Establecer la semilla aleatoria
    G4Random::setTheSeed(seed);
    G4cout<<"The seed is: "<< seed<<G4endl;
    G4UIExecutive *ui=nullptr;
    
    #ifdef G4MULTITHREADED
        G4MTRunManager *runManager=new G4MTRunManager();
    #else
        G4RunManager *runManager=new G4RunManager();
    #endif
    MyDetectorConstruction scintillatorProperties;
    if(G4String(argv[1])=="SC"){
        G4int thickness = G4UIcommand::ConvertToInt(argv[4]);
        scintillatorProperties.ScintillatorProperties(argv[2],argv[3],thickness,argv[5],argv[1]); 
    
        runManager->SetUserInitialization(new MyDetectorConstruction());
        G4cout<<MyDetectorConstruction::scintillatorGeometry<<MyDetectorConstruction::scintillatorType<<MyDetectorConstruction::scintillatorThickness<<MyDetectorConstruction::scintillatorNumberOfSensors<<MyDetectorConstruction::scintillatorArrangement<<G4endl;
        runManager->SetUserInitialization(new MyPhysicsList());
        runManager->SetUserInitialization(new MyActionInitialization());
        G4VModularPhysicsList * physics = new QGSP_BERT();
        //physics->RegisterPhysics(new G4DecayPhysics());
        //physics->RegisterPhysics(new G4EmStandardPhysics());
        physics->RegisterPhysics(new G4OpticalPhysics());
        runManager->SetUserInitialization(physics);
    
        if(argc==6){
            ui = new G4UIExecutive(argc,argv);
            
        }
        

        G4VisManager *visManager = new G4VisExecutive();
        visManager->Initialize();
        G4UImanager *UImanager = G4UImanager::GetUIpointer();
        if(ui){
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();
        }else{
            G4String command="/control/execute ";
            G4String fileName = argv[6];
            UImanager->ApplyCommand(command+fileName);
            delete ui;
        }


        delete visManager;
    }else if(G4String(argv[1])=="SCARRAY"){
        G4int length = G4UIcommand::ConvertToInt(argv[3]);
        G4int width = G4UIcommand::ConvertToInt(argv[4]);
        G4int thickness = G4UIcommand::ConvertToInt(argv[5]);
        G4int gap = G4UIcommand::ConvertToInt(argv[6]);
        G4int groupCount = G4UIcommand::ConvertToInt(argv[7]);

        // Parse group parameters: for each group, read count, orientation, and zPos
        std::vector<G4String> orientations;
        std::vector<G4double> zPositions;
        std::vector<G4int> counts;

        G4int argIndex = 8;
        for (G4int i = 0; i < groupCount; i++) {
            G4int groupScintCount = G4UIcommand::ConvertToInt(argv[argIndex++]);
            G4String orientation = argv[argIndex++];
            G4double zPos = G4UIcommand::ConvertToDouble(argv[argIndex++]);

            counts.push_back(groupScintCount);
            orientations.push_back(orientation);
            zPositions.push_back(zPos);
        }

        scintillatorProperties.ScintillatorArrayGroupProperties(
            argv[2], length, width, thickness, gap, groupCount,
            orientations, zPositions, counts);

        runManager->SetUserInitialization(new MyDetectorConstruction());
        G4cout<<"SCARRAY Mode: Type="<<MyDetectorConstruction::scintillatorType
              <<" Length="<<MyDetectorConstruction::scintillatorLength<<"cm"
              <<" Width="<<MyDetectorConstruction::scintillatorWidth<<"cm"
              <<" Thickness="<<MyDetectorConstruction::scintillatorThickness<<"mm"
              <<" Gap="<<MyDetectorConstruction::scintillatorGap<<"cm"
              <<" Groups="<<MyDetectorConstruction::fGroupCount<<G4endl;
        runManager->SetUserInitialization(new MyPhysicsList());
        runManager->SetUserInitialization(new MyActionInitialization());
        G4VModularPhysicsList * physics = new QGSP_BERT();
        physics->RegisterPhysics(new G4OpticalPhysics());
        runManager->SetUserInitialization(physics);

        // Calculate expected argc: 8 (base) + groupCount*3 (group params)
        G4int expectedArgs = 8 + groupCount * 3;
        if(argc == expectedArgs){
            ui = new G4UIExecutive(argc,argv);
        }

        G4VisManager *visManager = new G4VisExecutive();
        visManager->Initialize();
        G4UImanager *UImanager = G4UImanager::GetUIpointer();
        if(ui){
            UImanager->ApplyCommand("/control/execute vis.mac");
            ui->SessionStart();
        }else if(argc > expectedArgs){
            G4String command="/control/execute ";
            G4String fileName = argv[expectedArgs];
            UImanager->ApplyCommand(command+fileName);
        }
        delete visManager;
    }

    
    
   
    //delete ui;
    
    delete runManager;
    return 0;
}
