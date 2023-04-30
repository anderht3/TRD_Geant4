#ifndef TRDEventAction_h
#define TRDEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

namespace TRD
{

class RunAction;

/// Event action class
///
/// In EndOfEventAction() there is collected information event per event
/// from Hits Collections, and accumulated statistic for
/// RunAction::EndOfRunAction() as well as many methods that will be
/// used in the step class

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void  BeginOfEventAction(const G4Event*) override;
    void    EndOfEventAction(const G4Event*) override;
 
    void SumChargedSecSize(G4int dchsize) {fChSecSize += dchsize;};         // Add to the total number of charged secondaries produced
    void SetPrimary() {fPrim = true;};                                      // Set to true if the particle is the primary
    void SumEdep(G4double dEdep) {fEdep += dEdep;};                         // Find the total energy deposited in the TRD
    void SumSecSize(G4int dSize) {fSecSize += dSize;};                      // Add to the total number of secondaries produced
    void SetCollision() {fCollision = true;};                               // Set to true if the particle annihilates in the TRD
    void LayerFound() {fLayer = true;};                                     // Set to true if a secondary particle in the event reaches the layer
    void NoACC() {fNoACC = true;};                                          // Set to true if the ACC is not hit by the primary
    void LayerMult(G4int dLayMult ) {fLayMult += dLayMult;};                // Add to the number of secondaries hit the layer
    void PionMult(G4int dpLayMult) {fpLayMult += dpLayMult;};               // Add to the number of secondary pions that hit the layer
    void KaonMult(G4int dkLayMult) {fkLayMult += dkLayMult;};               // Add to the number of secondary kaons that hit the layer
    void PionMultALL(G4int dpLayMultALL) {fpLayMultALL += dpLayMultALL;};   // Add to the total number of secondary pions produced
    void KaonMultALL(G4int dkLayMultALL) {fkLayMultALL += dkLayMultALL;};   // Add to the total number of secondary kaons produced



  private:
    RunAction* fRunAction = nullptr;             // Call the run action
    G4int fTRD_ID = -1;                          // Check if the TRD exists
    G4int fLayer_ID = -1;                        // Check if the layer exists
    
    G4int fpLayMultALL = 0;                      // Total multiplicity of pions
    G4int fkLayMultALL = 0;                      // Total multiplicity of kaons
    G4int fLayMult = 0;                          // Multiplicity of secondaries at the layer
    G4int fpLayMult = 0;                         // Multiplicity of pions at the layer
    G4int fkLayMult = 0;                         // Multiplicity of kaons at the layer
    G4int fChSecSize = 0;                        // Number of charged secondary particles
    G4bool fLayer = false;                       // Layer hit
    G4bool fNoACC = false;                       // No ACC hit
    G4bool fPrim = false;                        // Primary
    G4double fEdep = 0;                          // TRD energy deposition
    G4int fSecSize = 0;                          // Total number of secondaries produced
    G4bool fCollision = false;                   // Annihilates

};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


