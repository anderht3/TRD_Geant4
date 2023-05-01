/*
 * Author: Hannah Anderson
 * Start Date: 03/12/2023
 * Most Recent Changes: 05/01/2023
 * 
 * Definition of physics processes necessary for the run
 * includes a pre-built physics list created and used
 * mostly by HEP as well as comments on manual decay process
 * creation
 */
 
#ifndef TRDPhysicsList_h
#define TRDPhysicsList_h 1

#include "G4VModularPhysicsList.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace TRD
{

class PhysicsList: public G4VModularPhysicsList
{
public:
  PhysicsList();
  ~PhysicsList() override = default;

  void SetCuts() override;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

