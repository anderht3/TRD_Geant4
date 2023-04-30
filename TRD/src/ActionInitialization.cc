#include "ActionInitialization.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"

namespace TRD
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::BuildForMaster() const
{
  SetUserAction(new RunAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::Build() const
{
  // The breakdown of the action hierarchy is: the runAction is the highest structure
  // and each run has a certain number of events (particle beam activations). Each
  // event includes new particles which are labeled as tracks. The events also occur
  // in steps (ionization, transportation, etc). Tracks and steps can both be called
  // in an event and can be called from each other as well - steps and tracks work 
  // together
 
  auto runAction = new RunAction();

  SetUserAction(runAction);
  EventAction* eventAction = new EventAction(runAction); //the next highest structure must be callable (runAction)

  SetUserAction(eventAction);
  SetUserAction(new PrimaryGeneratorAction);
  SetUserAction(new SteppingAction(eventAction)); //the next highest structure must be callable (eventaction)
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

