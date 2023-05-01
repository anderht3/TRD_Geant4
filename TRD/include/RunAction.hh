/*
 * Author: Hannah Anderson
 * Start Date: 03/12/2023
 * Most Recent Changes: 05/01/2023
 * 
 * Run action class that counts the number of events 
 * and layer hits
 */ 

#ifndef TRDRunAction_h
#define TRDRunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace TRD
{

class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run*) override;
    void   EndOfRunAction(const G4Run*) override;

    void CountEvent()           { fGoodEvents += 1; };            // Increments the number of events processessed
    void CountLayer()           { fGoodLayer += 1; };             // Increments the number of events with charged particle layer hits
    void CountPion()            { fGoodPion +=1;};                // Increments the number of events with pion layer hits
    void CountOne()             { fGoodOne +=1;};                 // Increments the number of events with one pion layer hit


private:
    G4Accumulable<G4int>    fGoodEvents = 0;            // Number of events
    G4Accumulable<G4int>    fGoodLayer = 0.;            // Number of events with any layer hit
    G4Accumulable<G4int>    fGoodPion = 0.;             // Number of events with pion layer hits
    G4Accumulable<G4int>    fGoodOne = 0.;              // Number of events with only one pion hit

};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

