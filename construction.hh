#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH


#include "G4SystemOfUnits.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include <assert.h>
#include <cmath>
#include "G4TwoVector.hh"
#include "G4PhysicalConstants.hh"
#include "G4ExtrudedSolid.hh"
#include "G4Polyhedra.hh"
#include "G4TessellatedSolid.hh"
#include "G4OpticalPhysics.hh"
#include "G4SubtractionSolid.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4String.hh"
#include <tuple>
#include <vector>
#include "G4SDManager.hh"
#include "barSD.hh"
#include "photonSD.hh"

class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    MyDetectorConstruction();
    ~MyDetectorConstruction();
    G4LogicalVolume *GetScoringVolume() const{return fScoringVolume;}
    void ScintillatorProperties(G4String type,G4String geometry,G4int thickness, G4String numberOfSensors, G4String arrangement);
    void ScintillatorArrayProperties(G4String type, G4int length, G4int width, G4int thickness, G4int gap, G4int count);
    void ScintillatorArrayGroupProperties(G4String type, G4int length, G4int width, G4int thickness, G4int gap, G4int groupCount, std::vector<G4String> orientations, std::vector<G4double> zPositions, std::vector<G4int> counts);

    // SCArray group structure for multi-group configuration
    struct SCArrayGroup {
        G4int count;           // Number of scintillators in this group
        G4String orientation;  // "X" or "Y" direction
        G4double zPos;         // Z-axis height (cm)
    };

    virtual G4VPhysicalVolume *Construct();
    static G4String scintillatorType;
    static G4String scintillatorGeometry;
    static G4int scintillatorThickness;
    static G4int copyNumberSC;
    static G4String scintillatorNumberOfSensors;
    static G4String scintillatorArrangement;
    static G4int scintillatorLength;
    static G4int scintillatorWidth;
    static G4int scintillatorGap;
    static G4int scintillatorCount;
    static std::vector<SCArrayGroup> fSCArrayGroups;
    static G4int fGroupCount;

   
    G4double wavelength, lightOutput;
    void hexagonalGeometry();
    void rectangularArrayGeometry();
    void rectangularGeometry();
   // void CreateRectangularShell(G4SubtractionSolid *shell,G4double length, G4double width, G4double thickness, G4double shellThickness);
private: 
    G4LogicalVolume *logicDetector;
   

    G4LogicalVolume *fScoringVolume;
    G4double thickness;
    G4int nCols,nRows;

        std::vector<G4LogicalVolume*> fArrayDetectorLogics;
        std::vector<G4LogicalVolume*> fArrayScintLogics;
    virtual void ConstructSDandField();
    
    
    G4Box *solidWorld,*solidDetector, *innerBox,*externalBox,*sensorBox,*solidDetector2;
    G4LogicalVolume *logicWorld,*logicSC, *logicMylar;
    G4VPhysicalVolume *physWorld,*physSC,*physDetector,*physMylar,*physDetector1,*physDetector2, *upperDetector, *lowerDetector;
    G4VSolid* xtru, *xtruExternal, *xtruInner;
    G4SubtractionSolid  *externalInner, *externalSensor1, *externalSensor2,*externalSensor;

    G4RotationMatrix* yRot = new G4RotationMatrix;

    G4Material *worldMat,*SCMat, *mylarMat,*air;
    G4Material *poly;
    G4OpticalSurface *mirrorSurface;
    void DefineMaterialsBC();
    void DefineMaterialsEJ(); 
};
#endif
