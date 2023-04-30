#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4RunManager.hh"
#include "G4EventManager.hh"

#include "G4Step.hh"
#include "G4AnalysisManager.hh"

namespace TRD
{

SteppingAction::SteppingAction(EventAction* evt):G4UserSteppingAction(),eventAct(evt)
{ 
}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step )
{

  // open histograms and ntuples to fill
  G4AnalysisManager *man = G4AnalysisManager::Instance();

  // trd dimensions (without calling detector class)
  G4double trd_end = 250.; //35
  G4double trd_begin = 950.; //95
 
  // acc dimensions 
  G4double acc_x = 10000;
  G4double acc_y = 10000;
 
  //define the beginning and end of the current step
  G4bool annihilate = true;
  G4StepPoint *preStepPoint = step->GetPreStepPoint();
  G4StepPoint *postStepPoint = step->GetPostStepPoint();

  // particle in step positions
  G4ThreeVector posStart = preStepPoint->GetPosition();
  G4ThreeVector pos = postStepPoint->GetPosition();

  // check if particle annihilates in detector (first initial check)
  if(pos[2] < trd_end){
    annihilate = false;
  }

  // charged particle storage
  G4int numPionALL = 0;
  G4int numKaonALL = 0;

  // energy at the beginning and end of step
  G4double beginEnergy = preStepPoint->GetKineticEnergy();
  G4double endEnergy = postStepPoint->GetKineticEnergy();
  //G4cout << "End energy: " << endEnergy << G4endl;
  G4double totalEnergy = beginEnergy - endEnergy;
  //G4cout << "total energy: " << totalEnergy << G4endl;
  G4Event *event = G4EventManager::GetEventManager()->GetNonconstCurrentEvent();

  // determine what particle this step is looking at
  G4Track *track = step->GetTrack();
  G4int parentID = track->GetParentID();
  G4int particle = track->GetDefinition()->GetPDGEncoding();

  // get particle momentum from track
  G4ThreeVector momentum = track->GetMomentum();
  G4double momentumTot = sqrt(momentum[0] * momentum[0] + momentum [1] * momentum[1] + momentum[2] * momentum[2]);

  //G4cout << "Momentum: " << momentumTot << G4endl;

  // get the secondaries from the step
  auto secondary = step ->GetSecondaryInCurrentStep();
  G4int numSecondary = (*secondary).size();

  G4int numCharged = 0;
  G4int numPionLay = 0;
  G4int numKaonLay = 0;
  G4int numChargedLay = 0;
    
  // PDG encodings for pions and kaons
  G4int pionPlus = 211;
  G4int pionMin = -211;
  G4int kaonPlus = 321;
  G4int kaonMin = -321;

  // get information for all secondary particles
  if(numSecondary && parentID == 0){
    for(G4int i = 0; i < numSecondary; i++){
      auto secstep = (*secondary)[i];
      G4int name = secstep->GetDefinition()->GetPDGEncoding();
      G4double kenergy = secstep->GetKineticEnergy();
      
      //G4cout << "Position of the secondary: " << secpos[2] << G4endl;
      // used to determine particle type in ntuple (not currently in use)
      G4int number = 0;

      // check which secondaries are charged
      if(name == pionPlus){
        number +=2;
        numCharged ++;
      }

      else if(name == pionMin){
        number +=3;
        numCharged ++;
      }

      else if(name == 111){
        number +=4;
      }
      else if(name == kaonPlus || name == kaonMin){
        numCharged++;
      }
      man->FillNtupleIColumn(1,2, number);

      // fill if annihilated
      if(annihilate && endEnergy < 10.){
        man->FillNtupleIColumn(2,2, number);
        man->FillNtupleDColumn(2,3, kenergy);
      }
    }

  }

  // if the event is primary set variables in eventAction
  if(parentID == 0){
    eventAct->SetPrimary();
    eventAct->SumSecSize(numSecondary);
  }


  // If the particle is a secondary from primary 
  if(parentID == 1){
    G4int layer = 0;
   
    if(particle == pionPlus || particle == pionMin){
        if(step->IsFirstStepInVolume()){
          man->FillH1(3, beginEnergy); // fill energy histogram for pion if step is the first in the volume
        }
    }
    else if(particle == kaonPlus || particle == kaonMin){

        if(step->IsFirstStepInVolume()){
          man->FillH1(4, beginEnergy); // fill energy hist for kaon if step is first
        }

    }

    // check if secondary made it past the layer
    if(pos[2] < -1100){
      
      // check if particle hits the acc
      if(abs(pos[0]) < acc_x && abs(pos[1]) < acc_y){
        eventAct->NoACC();
        //G4cout << "noACC " << G4endl;
      }

      eventAct->LayerFound();
      layer +=1;
      
      //check if particle is a pion
      if(particle == pionPlus || particle == pionMin){
        //G4cout << "pion found" << G4endl;
        numChargedLay ++;
        numPionLay ++;
        if(step->IsFirstStepInVolume()){
          man->FillH1(5, beginEnergy);
          man->FillH1(14, momentumTot);
        }
      }
      //check if particle is a kaon
      else if(particle == kaonPlus || particle == kaonMin){
        numChargedLay++;
        numKaonLay ++;
      
        if(step->IsFirstStepInVolume()){
          man->FillH1(6, beginEnergy);
        }

      } 
    }
    man->FillNtupleIColumn(2,4, layer);
  }

  // check if pions/kaons in general made it to the end of the layer
  if (pos[2] < -1100){
    if(particle == pionPlus || particle == pionMin){
        numPionALL +=1;
        if(step->IsFirstStepInVolume()){
          man->FillH1(10, beginEnergy);
        }
    }
    else if(particle == kaonPlus || particle == kaonMin){
        numKaonALL +=1;
        if(step->IsFirstStepInVolume()){
          man->FillH1(11, beginEnergy);
        }

    }



  }
  
  //sum up the energy deposited from primary in the TRD
  if(pos[2] >= trd_end && parentID == 0 && posStart[2] <= trd_begin){
    eventAct->SumEdep(totalEnergy);
  }
  
  //add information to the eventAction
  eventAct->LayerMult(numChargedLay);
  eventAct->SumChargedSecSize(numCharged);
  eventAct->PionMult(numPionLay);
  eventAct->KaonMult(numKaonLay);
  eventAct->PionMultALL(numPionALL);
  eventAct->KaonMultALL(numKaonALL);  

  // final check on annihilation
  if(parentID == 0 && annihilate && endEnergy < 10.){
    eventAct->SetCollision();
    man->FillNtupleDColumn(0,1,pos[0]);
    man->FillNtupleDColumn(0,2,pos[1]);
    man->FillNtupleDColumn(0,3,pos[2]);
  }


}

}
