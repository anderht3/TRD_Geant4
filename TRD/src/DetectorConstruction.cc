/*
 * Author: Hannah Anderson
 * Start Date: 03/12/2023
 * Most Recent Changes: 05/01/2023
 * 
 */     


#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

namespace TRD
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
:fCheckOverlaps(true)
{
  DefineMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{

  G4NistManager* man = G4NistManager::Instance();

  G4bool isotopes = false;

  G4Element*  Al = man->FindOrBuildElement("Al" , isotopes);
  G4Element*  Si = man->FindOrBuildElement("Si", isotopes);
  G4Element*  N  = man->FindOrBuildElement("N", isotopes);
  G4Element*  O  = man->FindOrBuildElement("O", isotopes);

  //Aluminum gas for TRD
  G4Material* AL_gas = new G4Material("Al_gas", 0.0535*g/cm3, 1);
  AL_gas->AddElement(Al, 1);

  //Silicon for detector layers
  G4Material* Sil = new G4Material("Si_layer8", 2.32*g/cm3, 1);
  Sil->AddElement(Si,1);

  //Vacuum for world volume - G4 does not allow a true vacuum
  G4Material* Vacuum = new G4Material("Vacuum", 1.e-25*g/cm3, 2);
  Vacuum-> AddElement(N, .7);
  Vacuum-> AddElement(O, .3);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{

   //World dimensions
   G4double world_sizeXY = 200 * cm;
   G4double world_sizeZ = 250 * cm;
   
   //Material manager
   G4NistManager *nist = G4NistManager::Instance();
   
   G4Material* world_mat = nist->FindOrBuildMaterial("Vacuum");
   G4Material* trd_mat = nist->FindOrBuildMaterial("Al_gas");
   G4Material* layer_mat = nist->FindOrBuildMaterial("Si_layer8");

   //World volume creation
   G4Box* solidWorld =
      new G4Box("World", 0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);

   G4LogicalVolume* logicWorld =
      new G4LogicalVolume(solidWorld, world_mat, "World");

   G4VPhysicalVolume* physWorld =
      new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, fCheckOverlaps);


   //TRD dimensions
   G4double trd_radius = 80 * cm;
   G4double trd_height = 70 * cm;
   G4double trd_placement = 60*cm;

   //TRD creation and placement
   G4Tubs* solidTRD =
      new G4Tubs("TRD", 0., trd_radius, 0.5* trd_height, 0, twopi);

   G4LogicalVolume* logicTRD =
      new G4LogicalVolume(solidTRD, trd_mat, "TRDLV");

   new G4PVPlacement(0, G4ThreeVector(0,0,trd_placement), logicTRD, "TRD", logicWorld, false, 0, fCheckOverlaps);

   //Silicon layer dimensions
   G4double layer_radius = 70 * cm;
   G4double layer_height = 2 *mm;
   G4double layer_placement = -85 * cm;


   //Silicon layer creation and placement
   G4Tubs* solidLayer =
      new G4Tubs("Layer", 0., layer_radius, 0.5 * layer_height, 0, twopi);

   G4LogicalVolume* logicLayer =
      new G4LogicalVolume(solidLayer, layer_mat, "LayerLV");

   new G4PVPlacement(0, G4ThreeVector(0,0,layer_placement), logicLayer, "Layer", logicWorld, false, 0, fCheckOverlaps);

   //check material composition
   G4cout << *(G4Material::GetMaterialTable()) << G4endl;

   //return full geometry
   return physWorld;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // declare TRD as a MultiFunctionalDetector scorer - useful to look at edep
  auto trd = new G4MultiFunctionalDetector("trd");
  G4SDManager::GetSDMpointer()->AddNewDetector(trd);
  G4VPrimitiveScorer* primitiv1 = new G4PSEnergyDeposit("edep");
  trd->RegisterPrimitive(primitiv1);
  SetSensitiveDetector("TRDLV",trd);

  // declare layer as a MultiFunctionalDetector scorer
  auto layer = new G4MultiFunctionalDetector("layer");
  G4SDManager::GetSDMpointer()->AddNewDetector(layer);
  G4VPrimitiveScorer* primitiv2 = new G4PSDoseDeposit("edep");
  layer->RegisterPrimitive(primitiv2);
  SetSensitiveDetector("LayerLV",layer);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

