/*
 * Author: Hannah Anderson
 * Start Date: 03/12/2023
 * Most Recent Changes: 05/01/2023
 * 
 */ 

#include "EventAction.hh"
#include "RunAction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"


namespace TRD
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction)
 : fRunAction(runAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* /*evt*/)
{
   // Reset all stored variables at the beginning of each run
   fpLayMultALL = 0;   //pion mult
   fkLayMultALL = 0;   //kaon mult
   fPrim = false;      //primary
   fNoACC = false;     //didnt hit acc
   fLayer = false;     //hit layer
   fEdep = 0;          //energy deposition trd
   fSecSize = 0;       //num of secondaries
   fCollision = false; //annihilate
   fLayMult = 0;       //mult at layer
   fpLayMult = 0;      //pion mult at layer
   fkLayMult = 0;      //kaon mult at layer
   fChSecSize = 0;     //charged particle secondaries
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* evt )
{
  //Hits collections
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  if(!HCE) return;

  // Get hits collections IDs
  if (fLayer_ID < 0) {
    G4SDManager* SDMan = G4SDManager::GetSDMpointer();
    fTRD_ID   = SDMan->GetCollectionID("trd/edep");
    fLayer_ID = SDMan->GetCollectionID("layer/edep");
  }

  
  // Choosing events based on edep (optional)
  const G4double threshold = 0*keV;
  G4int nbOfFired = 0;

  // Hits event map
  auto evtMap = (G4THitsMap<G4double>*)(HCE->GetHC(fTRD_ID));

  // Get edep for each hit in map and check events (optional)
  std::map<G4int,G4double*>::iterator itr;
  for (itr = evtMap->GetMap()->begin(); itr != evtMap->GetMap()->end(); itr++) {
    ///G4int copyNb  = (itr->first); //geometry id number for hit
    G4double edep = *(itr->second);
    if (edep > threshold) nbOfFired++;
  }
   
  // Edep optional condition
  if (fEdep > threshold) fRunAction->CountEvent();

  // Total edep deposited in layer (optional)
  G4double totEdep = 0.;

  // Layer hits event map
  evtMap = (G4THitsMap<G4double>*)(HCE->GetHC(fLayer_ID));

  for (itr = evtMap->GetMap()->begin(); itr != evtMap->GetMap()->end(); itr++) {
    ///G4int copyNb  = (itr->first);
    totEdep = *(itr->second);
  }
  
  // Add to stored run variables

  // Increment if something hit the layer
  if (fLayer && fNoACC) fRunAction->CountLayer();

  // Increment if pion hit layer
  if (fpLayMultALL > 0)  fRunAction->CountPion();
  
  // Increment if one pion hit layer
  if (fpLayMultALL == 1) fRunAction->CountOne();

  //Get event ID
  const G4Event *event_man = G4RunManager::GetRunManager()->GetCurrentEvent();
  G4int event = event_man->GetEventID();

  //get root file information
  G4AnalysisManager *man = G4AnalysisManager::Instance();
  

  //fill ntuples and histogram for primary annihilated
  if(fCollision && fPrim){
    //man->FillH1(1,fSecSize);
    man->FillH1(1, fSecSize);
    man->FillH1(2, fChSecSize);
    man->FillH1(7, fLayMult);
    man->FillH1(8, fpLayMult);
    man->FillH1(9, fkLayMult);
    man->FillH1(12, fpLayMultALL);
    man->FillH1(13, fkLayMultALL);
    man->FillNtupleDColumn(0,4,fEdep);
    man->FillNtupleIColumn(0,0,event);
    man->AddNtupleRow(0);

    man->FillNtupleIColumn(2,0,event);
    man->FillNtupleIColumn(2,1, fSecSize);
    man->AddNtupleRow(2);

  }

  //fill for just primary events
  if(fPrim){
    //G4cout << "event number: " << event << G4endl;
    //G4cout << "number of secondaries: " << fSecSize << G4endl;
    //G4cout << "edep: " << fEdep << G4endl;
   
    man->FillH1(0, fSecSize); 
    man->FillNtupleIColumn(1,0, event); 
    man->FillNtupleDColumn(1,1, fEdep);
    man->AddNtupleRow(1);
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

