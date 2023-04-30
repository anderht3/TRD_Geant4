#ifndef TRDDetectorConstruction_h
#define TRDDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;

namespace TRD
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//  Creates and stores the detectors
//  and contains information about the 
//  materials used as well as methods
//  to get information about the built 
//  layer, TRD, and other volumes

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction() override = default;

  public:
    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;


  public:
    G4VPhysicalVolume* GetPvolWorld()         {return physWorld;};
    G4Material*        GetWorldMaterial()     {return world_mat;};
    G4double           GetWorldSizeX()        {return world_sizeXY;};
    G4double           GetTRDHeight()         {return trd_height;};
    G4double           GetTRDPlacement()      {return trd_placement;};
    G4double           GetLayerHeight()       {return layer_height;};

  private:
    G4Material*            world_mat;
    G4VPhysicalVolume*     physWorld;
    G4double               world_sizeXY;
    G4double               trd_height;
    G4double               trd_placement;
    G4double               layer_height;


  private:
    void DefineMaterials();

    G4bool fCheckOverlaps = true;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

