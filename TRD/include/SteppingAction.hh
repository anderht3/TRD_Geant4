#ifndef TRDSteppingAction_h
#define TRDSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace TRD
{

// Tracks many variables that change during the different steps
// in the geant4 process and provides access to the current step
// and tracks associated with the step

class EventAction;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(EventAction*);
   ~SteppingAction();

    void UserSteppingAction(const G4Step*);

  private:
    EventAction*          eventAct;       // need the current event to reference 
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
