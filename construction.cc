#include "construction.hh"
#include <iostream>
#include <tuple>
G4String MyDetectorConstruction::scintillatorType;
G4String MyDetectorConstruction::scintillatorGeometry;
G4String MyDetectorConstruction::scintillatorNumberOfSensors;
G4int MyDetectorConstruction::scintillatorThickness;
G4int MyDetectorConstruction::copyNumberSC;
G4String MyDetectorConstruction::scintillatorArrangement;
G4int MyDetectorConstruction::scintillatorLength;
G4int MyDetectorConstruction::scintillatorWidth;
G4int MyDetectorConstruction::scintillatorGap;
G4int MyDetectorConstruction::scintillatorCount;
std::vector<MyDetectorConstruction::SCArrayGroup> MyDetectorConstruction::fSCArrayGroups;
G4int MyDetectorConstruction::fGroupCount;


MyDetectorConstruction::MyDetectorConstruction(){

   if(scintillatorArrangement=="SC" ){

        if(scintillatorType=="EJ208"){
            DefineMaterialsEJ();
        }else if (scintillatorType=="BC404"){
        DefineMaterialsBC();
    }
    }else if(scintillatorArrangement=="SCARRAY"){
        if(scintillatorType=="EJ208"){
            DefineMaterialsEJ();
        }else if (scintillatorType=="BC404"){
            DefineMaterialsBC();
        }
    }

}
MyDetectorConstruction::~MyDetectorConstruction(){}

void MyDetectorConstruction::DefineMaterialsBC(){
      //Scintillation parameters are defined according to a plastic scintillator bc-404. Check https://www.phys.ufl.edu/courses/phy4803L/group_I/muon/bicron_bc400-416.pdf for more details.
   
    std::ifstream fin("bc404.txt");
    std::vector<G4double> energy;
    std::vector<G4double> fraction;
    std::vector<G4double> rindexSC;
    std::vector<G4double> rindexWorld;
    std::vector<G4double> rindexMylar;
    std::vector<G4double> absSC;
    std::vector<G4double> reflectivity;

    G4double RindexSC=1.58,RindexWorld=1.0,RindexMylar=1.655, AbsSC=160.*cm, Reflectivity=0.9999;
    while(!fin.eof()){
        fin >> wavelength>>lightOutput;
        G4double Energy=1.239841939*eV/((wavelength/1000));
        lightOutput= lightOutput/100;
        energy.push_back(Energy);
        fraction.push_back(lightOutput);
    }
    G4int numberOfEntries=energy.size();
    
    G4int contador = 0;
    while(contador<numberOfEntries){
        rindexSC.push_back(RindexSC);
        rindexWorld.push_back(RindexWorld);
        rindexMylar.push_back(RindexMylar);
        absSC.push_back(AbsSC);
        reflectivity.push_back(Reflectivity);
        contador++;
    }

    G4OpticalParameters::Instance()->SetScintFiniteRiseTime(true);
    G4NistManager *nist = G4NistManager::Instance();

    //Geant4 what objects are detectors.-Predefined materials are invoked.
    worldMat = nist->FindOrBuildMaterial("G4_AIR");
    SCMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    mylarMat = nist->FindOrBuildMaterial("G4_MYLAR");

    mirrorSurface = new G4OpticalSurface("mirrorSurface");
    mirrorSurface->SetType(dielectric_dielectric);
    mirrorSurface->SetFinish(polishedfrontpainted);
    mirrorSurface->SetModel(unified);

    G4MaterialPropertiesTable *mptSC=new G4MaterialPropertiesTable();
    G4MaterialPropertiesTable *mptWorld=new G4MaterialPropertiesTable();
    G4MaterialPropertiesTable *mptMylar=new G4MaterialPropertiesTable();
    G4MaterialPropertiesTable *mptMirror = new G4MaterialPropertiesTable();

    mptSC->AddProperty("RINDEX",energy, rindexSC,numberOfEntries);
    mptWorld->AddProperty("RINDEX",energy, rindexWorld,numberOfEntries);
    mptSC->AddProperty("SCINTILLATIONCOMPONENT1",energy,fraction,numberOfEntries);
    mptSC->AddConstProperty("SCINTILLATIONYIELD", 10./keV);
    mptSC->AddConstProperty("RESOLUTIONSCALE", 1.);
    mptSC->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 1.8*ns);
    mptSC->AddConstProperty("SCINTILLATIONRISETIME1", 0.7*ns);
    mptSC->AddProperty("ABSLENGTH",energy,absSC,numberOfEntries);
    mptMirror->AddProperty("REFLECTIVITY", energy, reflectivity);;
    mptMylar->AddProperty("RINDEX",energy, rindexMylar,numberOfEntries);
    SCMat->SetMaterialPropertiesTable(mptSC);
    worldMat->SetMaterialPropertiesTable(mptWorld);
    mylarMat->SetMaterialPropertiesTable(mptMylar);
    mirrorSurface->SetMaterialPropertiesTable(mptMirror);
}

void MyDetectorConstruction::DefineMaterialsEJ(){
    
    //Scintillation parameters are defined according to a plastic scintillator bc-404. Check https://www.phys.ufl.edu/courses/phy4803L/group_I/muon/bicron_bc400-416.pdf for more details.
   
    std::ifstream fin("ej208.txt");
    std::vector<G4double> energy;
    std::vector<G4double> fraction;
    std::vector<G4double> rindexSC;
    std::vector<G4double> rindexWorld;
    std::vector<G4double> rindexMylar;
    std::vector<G4double> absSC;
    std::vector<G4double> reflectivity;

    G4double RindexSC=1.58,RindexWorld=1.0,RindexMylar=1.655, AbsSC=400.*cm, Reflectivity=0.9999;
    while(!fin.eof()){
        
        fin >> wavelength>>lightOutput;
        G4double Energy=1.239841939*eV/((wavelength/1000));
        lightOutput= lightOutput/100;
        energy.push_back(Energy);
        fraction.push_back(lightOutput);
    }
    G4int numberOfEntries=energy.size();
    
    G4int contador = 0;
    while(contador<numberOfEntries){
        rindexSC.push_back(RindexSC);
        rindexWorld.push_back(RindexWorld);
        rindexMylar.push_back(RindexMylar);
        absSC.push_back(AbsSC);
        reflectivity.push_back(Reflectivity);
        contador++;
    }
    
    G4OpticalParameters::Instance()->SetScintFiniteRiseTime(true);
    G4NistManager *nist = G4NistManager::Instance();

    //Geant4 what objects are detectors.-Predefined materials are invoked.
    worldMat = nist->FindOrBuildMaterial("G4_AIR");
    SCMat = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    mylarMat = nist->FindOrBuildMaterial("G4_MYLAR");

    mirrorSurface = new G4OpticalSurface("mirrorSurface");
    mirrorSurface->SetType(dielectric_dielectric);
    mirrorSurface->SetFinish(polishedfrontpainted);
    mirrorSurface->SetModel(unified);

    G4MaterialPropertiesTable *mptSC=new G4MaterialPropertiesTable();
    G4MaterialPropertiesTable *mptWorld=new G4MaterialPropertiesTable();
    G4MaterialPropertiesTable *mptMylar=new G4MaterialPropertiesTable();
    G4MaterialPropertiesTable *mptMirror = new G4MaterialPropertiesTable();

    mptSC->AddProperty("RINDEX",energy, rindexSC,numberOfEntries);
    mptWorld->AddProperty("RINDEX",energy, rindexWorld,numberOfEntries);
    mptSC->AddProperty("SCINTILLATIONCOMPONENT1",energy,fraction,numberOfEntries);
    mptSC->AddConstProperty("SCINTILLATIONYIELD", 10./keV);
    mptSC->AddConstProperty("RESOLUTIONSCALE", 1.);
    mptSC->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 3.3*ns);
    mptSC->AddConstProperty("SCINTILLATIONRISETIME1", 1.*ns);
    mptSC->AddProperty("ABSLENGTH",energy,absSC,numberOfEntries);
    mptMirror->AddProperty("REFLECTIVITY", energy, reflectivity);;
    mptMylar->AddProperty("RINDEX",energy, rindexMylar,numberOfEntries);
    SCMat->SetMaterialPropertiesTable(mptSC);
    worldMat->SetMaterialPropertiesTable(mptWorld);
    mylarMat->SetMaterialPropertiesTable(mptMylar);
    mirrorSurface->SetMaterialPropertiesTable(mptMirror);
}

G4VPhysicalVolume *MyDetectorConstruction::Construct(){
    
   
    //World volume is created

    G4double xWorld = 1.5*m, yWorld= 1.5*m, zWorld = 8.5*m;
    thickness=scintillatorThickness*mm;
    solidWorld= new G4Box("solidWorld",xWorld,yWorld,zWorld);
    logicWorld = new G4LogicalVolume(solidWorld,worldMat, "logicWorld");
    physWorld =new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),logicWorld,"physWorld",0,false,0,true);
   
    if(scintillatorArrangement=="SC"){
        if(scintillatorGeometry=="R"){
            rectangularGeometry();
        }else if (scintillatorGeometry=="H"){
            hexagonalGeometry();
    }
   } else if(scintillatorArrangement == "SCARRAY"){
        rectangularArrayGeometry();
   }
    fScoringVolume=logicSC;

    return physWorld;
} 



//Predefined method used to tell Geant4 what objects are detectors. 
void MyDetectorConstruction::ConstructSDandField(){
   // MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");

    if(scintillatorArrangement == "SC"){


        MyPhotoDetector *myphotodetector = new MyPhotoDetector("myphotodetector");
        logicDetector->SetSensitiveDetector(myphotodetector);

    } else if(scintillatorArrangement == "SCARRAY"){

        MyBarDetector *mybardetector = new MyBarDetector("mybardetector");
        MyPhotoDetector *myphotodetector = new MyPhotoDetector("myphotodetector");

        // Set scintillator volumes as sensitive (bar detector)
        for (auto scLogic : fArrayScintLogics) {
            scLogic->SetSensitiveDetector(mybardetector);
        }

        // Set sensor volumes as sensitive (photo detector)
        for (auto detLogic : fArrayDetectorLogics) {
            detLogic->SetSensitiveDetector(myphotodetector);
        }

    }


}
void MyDetectorConstruction::ScintillatorProperties(G4String type,G4String geometry,G4int thickness, G4String numberOfSensors, G4String arrangement){
    scintillatorType=type;
    scintillatorGeometry=geometry;
    scintillatorThickness = thickness;
    scintillatorNumberOfSensors=numberOfSensors;
    scintillatorArrangement=arrangement;

}
void MyDetectorConstruction::ScintillatorArrayProperties(G4String type, G4int length, G4int width, G4int thickness, G4int gap, G4int count){
    scintillatorType = type;
    scintillatorLength = length;
    scintillatorWidth = width;
    scintillatorThickness = thickness;
    scintillatorGap = gap;
    scintillatorCount = count;
    scintillatorArrangement = "SCARRAY";
}
void MyDetectorConstruction::ScintillatorArrayGroupProperties(G4String type, G4int length, G4int width, G4int thickness, G4int gap, G4int groupCount, std::vector<G4String> orientations, std::vector<G4double> zPositions, std::vector<G4int> counts){
    scintillatorType = type;
    scintillatorLength = length;
    scintillatorWidth = width;
    scintillatorThickness = thickness;
    scintillatorGap = gap;
    scintillatorArrangement = "SCARRAY";
    fGroupCount = groupCount;
    fSCArrayGroups.clear();

    for (G4int i = 0; i < groupCount; i++) {
        SCArrayGroup group;
        group.count = counts[i];
        group.orientation = orientations[i];
        group.zPos = zPositions[i];
        fSCArrayGroups.push_back(group);
    }
}
void MyDetectorConstruction::rectangularGeometry(){


    if(scintillatorNumberOfSensors=="1"){
        // 3cm x 3cm x 50cm 长方体闪烁体，两端面中心各装一个光子探测器
        G4TwoVector offsetA(0,0), offsetB(0,0);
        G4double scaleA = 1, scaleB = 1;

        yRot->rotateY(0*rad);

        // 闪烁体尺寸: 3cm(x) x 50cm(y) x 3cm(z)
        G4double halfX = 1.5*cm;  // X方向半长度
        G4double halfY = 25.*cm;   // Y方向半长度 (总长50cm)
        G4double halfZ = 1.5*cm;  // Z方向半长度

        // Mylar外壳 (闪烁体 + 1mm包边)
        innerBox = new G4Box("innerBox", halfX, halfY, halfZ);
        externalBox = new G4Box("externalBox", halfX + 1*mm, halfY + 1*mm, halfZ + 1*mm);

        // 传感器盒: 3mm x 3mm x 1mm, 放在两端面中心
        sensorBox = new G4Box("sensorBox", 3.*mm, 3.*mm, 1.*mm);

        G4ThreeVector zTrans(0, 0, halfZ);  // 传感器在+Z侧
        G4ThreeVector yTransUp(0, halfY, 0);   // 上端面中心
        G4ThreeVector yTransDown(0, -halfY, 0); // 下端面中心

        externalInner = new G4SubtractionSolid("External-Inner", externalBox, innerBox);

        // Mylar外壳减第一个传感器槽 (上端面)
        externalSensor1 = new G4SubtractionSolid("External-Inner-Sensor1", externalInner, sensorBox, yRot, yTransUp + zTrans);
        // 再减第二个传感器槽 (下端面)
        externalSensor2 = new G4SubtractionSolid("External-Inner-Sensor2", externalSensor1, sensorBox, yRot, yTransDown + zTrans);

        logicMylar = new G4LogicalVolume(externalSensor2, mylarMat, "logicMylar");
        G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin", logicMylar, mirrorSurface);
        physMylar = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicMylar, "physMylar", logicWorld, false, 1, true);

        // 闪烁体
        G4Box *solidScintillator = new G4Box("solidScintillator", halfX, halfY, halfZ);
        logicSC = new G4LogicalVolume(solidScintillator, SCMat, "logicSC");
        physSC = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicSC, "physSC", logicWorld, false, 2, true);

        // 两个光子探测器: 上端面和下端面中心
        solidDetector = new G4Box("solidDetector", 3.*mm, 3.*mm, 1.0*mm);
        logicDetector = new G4LogicalVolume(solidDetector, worldMat, "logicalDetector");

        // 上端面光子探测器 (Y = +25cm + 1mm)
        G4VPhysicalVolume *physDetectorUp = new G4PVPlacement(0, G4ThreeVector(0., halfY + 1*mm, 0.), logicDetector, "physDetectorUp", logicWorld, false, 3, true);
        // 下端面光子探测器 (Y = -25cm - 1mm)
        G4VPhysicalVolume *physDetectorDown = new G4PVPlacement(0, G4ThreeVector(0., -halfY - 1*mm, 0.), logicDetector, "physDetectorDown", logicWorld, false, 4, true);
    }

    if(scintillatorNumberOfSensors=="2"){
        G4TwoVector offsetA(0,0), offsetB(0,0);
        G4double scaleA = 1, scaleB = 1;
        
        
        
        G4ThreeVector yTrans1(0,50.*cm,0);
        G4ThreeVector yTrans2(0,-50.*cm,0);
        yRot->rotateY(0*rad);
        innerBox = new G4Box("innerBox",2.5*cm,50.*cm,thickness/2);
        externalBox = new G4Box("externalBox",2.6*cm,50.1*cm,(thickness/2)+1*mm);
        sensorBox = new G4Box("sensorBox",3.*mm,1.*mm,3*mm);

        externalInner= new G4SubtractionSolid("External-Inner", externalBox, innerBox);
        externalSensor1=new G4SubtractionSolid("External-Inner-Sensor1", externalInner, sensorBox, yRot, yTrans1);
        externalSensor2=new G4SubtractionSolid("External-Inner-Sensor2", externalSensor1, sensorBox, yRot, yTrans2);
        logicMylar= new G4LogicalVolume(externalSensor2,mylarMat, "logicMylar");
        G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin",logicMylar,mirrorSurface);
        physMylar =new G4PVPlacement(0,G4ThreeVector(0.,0.,10.*cm),logicMylar,"physMylar",logicWorld,false,1,true);

        solidDetector=new G4Box("solidDetector",3.*mm,1.*mm,3.0*mm);
        logicDetector = new G4LogicalVolume(solidDetector,worldMat,"logicalDetector");
        physDetector1=new G4PVPlacement(0,G4ThreeVector(0.,50*cm+1*mm,10*cm),logicDetector,"physDetector1",logicWorld,false,3,true);
        physDetector2=new G4PVPlacement(0,G4ThreeVector(0.,-50*cm-1*mm,10*cm),logicDetector,"physDetector2",logicWorld,false,4,true);

        G4Box *solidScintillator = new G4Box("solidScintillator",2.5*cm,50.*cm,thickness/2);
        logicSC= new G4LogicalVolume(solidScintillator,SCMat, "logicSC");
        physSC =new G4PVPlacement(0,G4ThreeVector(0.,0.,10.*cm),logicSC,"physSC",logicWorld,false,2,true);
    }

}

void MyDetectorConstruction::hexagonalGeometry(){
    //Hexagonal geometry is defined using G4ExtrudedSolid Class
    const G4int nsect = 6;
    std::vector<G4TwoVector> polygon(nsect);
    G4double ang = twopi/nsect;
    G4double rmax = 2.88675*cm;
    for (G4int i = 0; i < nsect; ++i)
    {
    G4double phi = i*ang;
    G4double cosphi = std::cos(phi);
    G4double sinphi = std::sin(phi);
    polygon[i].set(rmax*cosphi, rmax*sinphi);
    }
    G4TwoVector offsetA(0,0), offsetB(0,0);
    G4double scaleA = 1, scaleB = 1;

    //Mylar Volume (External volume)
    std::vector<G4TwoVector> polygonExt(nsect);
    G4double rmaxExt = (26/(std::cos(pi/6)))*mm;
    
    for (G4int i = 0; i < nsect; ++i)
    {
    G4double phi = i*ang;
    G4double cosphi = std::cos(phi);
    G4double sinphi = std::sin(phi);
    polygonExt[i].set(rmaxExt*cosphi, rmaxExt*sinphi);
    }
    
    G4ThreeVector yTrans(0,2.5*cm,0.);
    yRot->rotateY(0*rad);
    //World volume is created
    sensorBox = new G4Box("sensorBox",3.*mm,2.*mm,3.*mm);
    
    
    
    
    xtru = new G4ExtrudedSolid("xtru", polygon, thickness/2, offsetA, scaleA, offsetB, scaleB);
    xtruInner = new G4ExtrudedSolid("xtruInner", polygon, thickness/2, offsetA, scaleA, offsetB, scaleB);

    logicSC= new G4LogicalVolume(xtru,SCMat, "logicSC");
    physSC =new G4PVPlacement(0,G4ThreeVector(0.,0.,10.*cm),logicSC,"physSC",logicWorld,false,0,true);
    xtruExternal= new G4ExtrudedSolid("xtruExternal", polygonExt, (thickness/2)+1*mm, offsetA, scaleA, offsetB, scaleB);
   
    externalInner= new G4SubtractionSolid("External-Inner", xtruExternal, xtruInner);
    externalSensor=new G4SubtractionSolid("external-sensor", externalInner, sensorBox, yRot, yTrans);
    logicMylar= new G4LogicalVolume(externalSensor,mylarMat, "logicMylar");
    G4LogicalSkinSurface *skin = new G4LogicalSkinSurface("skin",logicMylar,mirrorSurface);
    physMylar =new G4PVPlacement(0,G4ThreeVector(0.,0.,10*cm),logicMylar,"physMylar",logicWorld,false,1,true);

    solidDetector=new G4Box("solidDetector",3.*mm,1.*mm,3.0*mm);
    logicDetector = new G4LogicalVolume(solidDetector,worldMat,"logicalDetector");
    G4VPhysicalVolume *physDetector=new G4PVPlacement(0,G4ThreeVector(0.,2.6*cm,10.*cm),logicDetector,"physDetector",logicWorld,false,3,true);
}

void MyDetectorConstruction::rectangularArrayGeometry(){
    // Multi-group rectangular geometry with scintillator arrays
    // Each group can have different orientation (X or Y) and Z height
    // Scintillator size: width (X) x length (Y) x thickness (Z) in cm

    G4double length = scintillatorLength * cm;   // Y direction
    G4double width = scintillatorWidth * cm;   // X direction
    G4double thickness = scintillatorThickness * mm; // Z direction
    G4double gap = scintillatorGap * cm;       // Gap between scintillators

    // Rotation matrices for X and Y orientations
    G4RotationMatrix *rotX = new G4RotationMatrix;  // No rotation for X direction
    G4RotationMatrix *rotY = new G4RotationMatrix;  // 90 degree rotation for Y direction
    rotY->rotateZ(90*deg);

    copyNumberSC = 0;
    fArrayDetectorLogics.clear();
    fArrayScintLogics.clear();

    G4int groupIndex = 0;

    // Iterate through each group
    for (G4int g = 0; g < fGroupCount; g++) {
        SCArrayGroup &group = fSCArrayGroups[g];
        G4int count = group.count;
        G4String orientation = group.orientation;
        G4double zPos = group.zPos * cm;  // Convert cm to Geant4 units

        G4RotationMatrix *rotMat;
        G4String orientStr;
        if (orientation == "X" || orientation == "x") {
            rotMat = rotX;
            orientStr = "X";
        } else {
            rotMat = rotY;
            orientStr = "Y";
        }

        // Calculate total array dimension for centering
        G4double totalDim = count * width + (count - 1) * gap;
        G4double startPos = -totalDim / 2.0 + width / 2.0;

        G4cout << "Group " << g << ": " << count << " scintillators, orientation=" << orientStr
               << ", Z=" << zPos/cm << " cm" << G4endl;

        // Place scintillators in this group
        for (G4int i = 0; i < count; i++) {
            G4double primaryPos = startPos + i * (width + gap);
            G4String name = "SC_G" + std::to_string(g) + "_" + std::to_string(i);

            G4double xPos, yPos;
            if (orientation == "X" || orientation == "x") {
                xPos = primaryPos;
                yPos = 0;
            } else {
                xPos = 0;
                yPos = primaryPos;
            }

            // =========================================
            // 1. Create Mylar shell (open at both ends)
            // =========================================
            G4Box * mylarOuter = new G4Box("mylarOuter",
                width/2 + 1*mm,  // X: wrapped with 1mm on each side
                length/2,         // Y: NOT wrapped, open at ends
                thickness/2 + 1*mm);  // Z: wrapped with 1mm on each side

            G4Box * mylarInner = new G4Box("mylarInner",
                width/2,          // X: inner void
                length/2 + 1*mm,  // Y: slightly larger to leave openings
                thickness/2);     // Z: inner void

            G4SubtractionSolid * mylarShell = new G4SubtractionSolid("mylarShell",
                mylarOuter, mylarInner);

            G4LogicalVolume * logicMylarLocal = new G4LogicalVolume(mylarShell, mylarMat, ("logicMylar_"+name).c_str());
            G4LogicalSkinSurface * skin = new G4LogicalSkinSurface(("skin_"+name).c_str(), logicMylarLocal, mirrorSurface);

            // Place mylar shell with rotation for Y orientation
            new G4PVPlacement(rotMat, G4ThreeVector(xPos, yPos, zPos), logicMylarLocal, ("physMylar_"+name).c_str(),
                             logicWorld, false, copyNumberSC+1, true);

            // =========================================
            // 2. Create scintillator (placed inside mylar shell)
            // =========================================
            G4Box * solidScintillator = new G4Box(("solidScint_"+name).c_str(), width/2, length/2, thickness/2);
            G4LogicalVolume * logicSCLocal = new G4LogicalVolume(solidScintillator, SCMat, ("logicSC_"+name).c_str());
            new G4PVPlacement(rotMat, G4ThreeVector(xPos, yPos, zPos), logicSCLocal, ("physSC_"+name).c_str(),
                             logicWorld, false, copyNumberSC+2, true);
            fArrayScintLogics.push_back(logicSCLocal);

            // =========================================
            // 3. Create sensors (directly on scintillator ends, NO gap)
            // For X orientation: sensor size: width x 1mm x thickness/2
            //   Sensor placed at Y = ±(length/2 + 1mm), 1mm gap is along Y direction
            // For Y orientation (rotated 90deg): sensor size: thickness x width x 1mm
            //   Sensor placed at X = ±(length/2 + 1mm), 1mm gap is along X direction
            //   The sensor thickness (1mm) should point toward world X (scintillator end direction)
            // =========================================
            G4Box * solidDetectorLocal;
            G4double sensorOffset1, sensorOffset2;
            if (orientation == "X" || orientation == "x") {
                // X direction: sensor box: width x 1mm x thickness
                // Covers 3cm x 3cm end face (width x thickness)
                // 1mm thickness points toward end face (Y direction)
                solidDetectorLocal = new G4Box(("solidDetector_"+name).c_str(),
                    width/2,      // X: 1.5cm covers face width
                    1.*mm,        // Y: 1mm thickness (points toward end face)
                    thickness/2); // Z: 1.5cm covers face thickness
                sensorOffset1 = -length/2 - 1.*mm;
                sensorOffset2 = length/2 + 1.*mm;
            } else {
                // Y direction: sensor box: 1mm x width x thickness
                // Covers 3cm x 3cm end face (width x thickness)
                // After 90 deg rotation, 1mm points toward end face (world X)
                solidDetectorLocal = new G4Box(("solidDetector_"+name).c_str(),
                    1.*mm,        // X: 1mm thickness (gap from scintillator end)
                    width/2,      // Y: 1.5cm covers face width
                    thickness/2); // Z: 1.5cm covers face thickness
                sensorOffset1 = -length/2 - 1.*mm;
                sensorOffset2 = length/2 + 1.*mm;
            }
            G4LogicalVolume * logicDetectorLocal = new G4LogicalVolume(solidDetectorLocal, worldMat, ("logicalDetector_"+name).c_str());

            // Place sensors at scintillator ends
            if (orientation == "X" || orientation == "x") {
                // X direction: offset along Y axis, with rotation (identity)
                new G4PVPlacement(rotMat, G4ThreeVector(xPos, sensorOffset1, zPos),
                                 logicDetectorLocal, ("physDetector_L_"+name).c_str(),
                                 logicWorld, false, copyNumberSC+3, true);
                new G4PVPlacement(rotMat, G4ThreeVector(xPos, sensorOffset2, zPos),
                                 logicDetectorLocal, ("physDetector_R_"+name).c_str(),
                                 logicWorld, false, copyNumberSC+4, true);
            } else {
                // Y direction: offset along X axis, no rotation
                // Place at (±offset, yPos, zPos) where offset = length/2 + 1mm
                new G4PVPlacement(0, G4ThreeVector(sensorOffset1, yPos, zPos),
                                 logicDetectorLocal, ("physDetector_L_"+name).c_str(),
                                 logicWorld, false, copyNumberSC+3, true);
                new G4PVPlacement(0, G4ThreeVector(sensorOffset2, yPos, zPos),
                                 logicDetectorLocal, ("physDetector_R_"+name).c_str(),
                                 logicWorld, false, copyNumberSC+4, true);
            }

            // Store the detector logical volume for sensitive detector assignment
            fArrayDetectorLogics.push_back(logicDetectorLocal);

            G4cout << "  " << name << " LeftDetector: " << copyNumberSC+3
                   << " RightDetector: " << copyNumberSC+4
                   << " at (" << xPos/cm << ", " << yPos/cm << ", " << zPos/cm << ") cm" << G4endl;

            copyNumberSC = copyNumberSC + 4;
        }
        groupIndex++;
    }

    // Set the first scintillator volume as scoring volume
    if (!fArrayScintLogics.empty()) {
        fScoringVolume = fArrayScintLogics[0];
    }
}

