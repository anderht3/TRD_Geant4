/*
 * Author: Hannah Anderson
 * Start Date: 03/12/2023
 * Most Recent Changes: 05/01/2023
 *
 * Event action class
 * In EndOfEventAction() there is collected information event per event
 * from Hits Collections, and accumulated statistic for
 * RunAction::EndOfRunAction() as well as many methods that will be
 * used in the step class
 */

#ifndef TRDEventAction_h
#define TRDEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

namespace TRD
{

class RunAction;


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
    void PionCHMultLAY(G4int dpLayCHMult) {fpCHLayMult += dpLayCHMult;};               // Add to the number of secondary pions that hit the layer
    void KaonCHMultLAY(G4int dkLayCHMult) {fkCHLayMult += dkLayCHMult;};               // Add to the number of secondary kaons that hit the layer
    void PionMultALL(G4int dpLayMultALL) {fpLayMultALL += dpLayMultALL;};   // Add to the total number of secondary pions produced
    void KaonMultALL(G4int dkLayMultALL) {fkLayMultALL += dkLayMultALL;};   // Add to the total number of secondary kaons produced
    void PionMultLAY(G4int dpLayMult) {fpLayMult += dpLayMult;};
    void PionMult(G4int dpMult) {fpMult += dpMult;};
    void KaonMultLAY(G4int dkLayMult) {fkLayMult += dkLayMult;};
    void KaonMult(G4int dkMult) {fkMult += dkMult;};
    void MomentumStore(G4double dmompion) {fmompion += dmompion;};
    void NumPiP(G4int dPiP) {fPiP += dPiP;};
    void NumPiM(G4int dPiM) {fPiM += dPiM;};
    void NumPi0(G4int dPi0) {fPi0 += dPi0;};
    void BetaStore(G4double dbeta) {fbeta += dbeta;};
    void MomentumCheck(G4double dmomcheck) {fmomcheck += dmomcheck;};

  private:
    RunAction* fRunAction = nullptr;             // Call the run action
    G4int fTRD_ID = -1;                          // Check if the TRD exists
    G4int fLayer_ID = -1;                        // Check if the layer exists
    
    G4double fmomcheck = 0;
    G4double fbeta = 0;
    G4int fPiP = 0;
    G4int fPiM = 0;
    G4int fPi0 = 0;
    G4double fmompion = 0;
    G4int fpLayMult = 0;
    G4int fkLayMult = 0;
    G4int fpMult = 0;
    G4int fkMult = 0;
    G4int fpLayMultALL = 0;                      // Total multiplicity of pions
    G4int fkLayMultALL = 0;                      // Total multiplicity of kaons
    G4int fLayMult = 0;                          // Multiplicity of secondaries at the layer
    G4int fpCHLayMult = 0;                         // Multiplicity of pions at the layer
    G4int fkCHLayMult = 0;                         // Multiplicity of kaons at the layer
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


