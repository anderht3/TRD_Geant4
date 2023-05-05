/*
 * Author: Hannah Anderson
 * Start Date: 03/12/2023
 * Most Recent Changes: 05/01/2023
 * 
 */
 
#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4RunManager.hh"
#include "G4EventManager.hh"

#include "G4Step.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

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

  //G4cout << "start " << posStart[2] << " end " << pos[2] << G4endl;

  // check if particle annihilates in detector (first initial check)
  if(pos[2] < trd_end){
    annihilate = false;
  }

  // charged particle storage
  G4int numPionALL = 0;
  G4int numKaonALL = 0;


  // energy at the beginning and end of step
  G4double beginEnergy = preStepPoint->GetKineticEnergy() ;
  G4double endEnergy = postStepPoint->GetKineticEnergy() / GeV;
  //G4cout << "End energy: " << endEnergy << G4endl;
  G4double totalEnergy = (beginEnergy - endEnergy) ;
  //G4cout << "Begin energy: " << beginEnergy << G4endl;
  G4Event *event = G4EventManager::GetEventManager()->GetNonconstCurrentEvent();

  // determine what particle this step is looking at
  G4Track *track = step->GetTrack();
  G4int parentID = track->GetParentID();
  G4int particle = track->GetDefinition()->GetPDGEncoding();
  G4int trackID = track->GetTrackID();

  G4double fullEnergy = track->GetTotalEnergy();

  /*
  if(parentID == 0){
    G4cout << "Start " << posStart << " and end " << pos << G4endl;
  }
  */
 
  G4double stepNum = track->GetCurrentStepNumber();
  //G4cout << "Step Number " << stepNum << " particle number " << trackID << G4endl;

  // get particle momentum from track
  G4ThreeVector momentum = track->GetMomentum();
  G4double momentumTot = sqrt(momentum[0] * momentum[0] + momentum [1] * momentum[1] + momentum[2] * momentum[2]);
  G4double c = 299792458;
  G4double mass = track->GetDefinition()->GetPDGMass();
  G4double beta = momentumTot/fullEnergy;

  G4double velocity = track->GetVelocity() /(m/s);
  G4double beta2 = velocity/c;
  G4double gamma = sqrt(1-beta2 * beta2);

  //G4cout << "Beta " << bet << G4endl;
  //G4cout << "Velocity: " << velocity << G4endl;
  //G4cout << "gamma " << gamma << G4endl;
  //G4cout << "Momentum: " << momentumTot << G4endl;

  // get the secondaries from the step
  auto secondary = step ->GetSecondaryInCurrentStep();
  G4int numSecondary = (*secondary).size();

  // storage of secondaries for pions and kaons
  G4int numPionSec = 0;   //total pion secondaries
  G4int numKaonSec = 0;   //total kaon secondaries
  G4int numPionLay = 0;   //total pion secondaries that reach the layer
  G4int numKaonLay = 0;   //total kaon secondaries that reach the layer
  G4int numCharged = 0;   //total charged secondaries
  G4int numPionCHLay = 0; //charged pion secondaries that reach the layer
  G4int numKaonCHLay = 0; //charged kaon secondaries that reach the layer
  G4int numChargedLay = 0;//charged particles that reach the layer
  //G4int numPandKsec = 0;  
  G4int numPiP = 0;       //number of pi + secondaries
  G4int numPiM = 0;       //number of pi - secondaries
  G4int numPi0 = 0;       //number of pi 0 secondaries

  //G4cout << "Total momentum: " << momentumTot << " Full energy " << fullEnergy << " kinetic energy " << beginEnergy << " partcle mass " << mass << G4endl;

  // PDG encodings for pions and kaons
  G4int pionPlus = 211;
  G4int pionMin = -211;
  G4int pionN = 111;
  G4int kaonPlus = 321;
  G4int kaonMin = -321;
  G4int kaonL = 130;
  G4int kaonS = 310;

  // get information for all secondary particles
  if(numSecondary && parentID == 0){
    for(G4int i = 0; i < numSecondary; i++){
      auto secstep = (*secondary)[i];
      G4int name = secstep->GetDefinition()->GetPDGEncoding();
      G4double kenergy = secstep->GetKineticEnergy();

      // used to determine particle type in ntuple (not currently in use)
      G4int number = 0;

      // check which secondaries are charged
      if(name == pionPlus){
        number +=2;
        numCharged ++;
        numPionSec ++;
        numPiP ++;
      }

      else if(name == pionMin){
        number +=3;
        numCharged ++;
        numPionSec ++;
        numPiM ++;
      }

      else if(name == 111){
        number +=4;
        numPionSec ++;
        numPi0 ++;
      }
      else if(name == kaonPlus || name == kaonMin){
        numCharged++;
        numKaonSec++;
      }
      else if (name == kaonS || name == kaonL){
        numKaonSec++;
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
    if(particle == pionPlus || particle == pionMin || particle == pionN){
        if(stepNum == 1){
          man->FillH1(3, beginEnergy); // fill energy histogram for pion if step is the first in the volume       
          eventAct->MomentumCheck(momentum[2]); 
        }
    }
    else if(particle == kaonPlus || particle == kaonMin){

        if(stepNum == 1){
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
        numPionCHLay ++;
        numPionLay ++;
        if(stepNum == 1){
          man->FillH1(5, beginEnergy);
          man->FillH1(14, momentumTot);
        }
      }
      else if(particle == pionN){
        numPionLay ++;
      }
      //check if particle is a kaon
      else if(particle == kaonPlus || particle == kaonMin){
        numChargedLay++;
        numKaonLay ++;
        numKaonCHLay ++;
        if(stepNum == 1){
          man->FillH1(6, beginEnergy);
        }
      
      }
      else if(particle == kaonS || particle == kaonL){
        numKaonLay ++;
      } 
    }
    man->FillNtupleIColumn(2,4, layer);
  }

  // check if pions/kaons in general made it to the end of the layer
  if (posStart[2] > -850 && pos[2] < -850){
    //G4cout << "Made it past the layer only once" << G4endl;
    if(particle == pionPlus || particle == pionMin){
        numPionALL +=1;
        eventAct->MomentumStore(momentumTot);
        eventAct->BetaStore(beta);
        if(stepNum == 1){
          man->FillH1(10, beginEnergy);
        }
    }
    else if(particle == kaonPlus || particle == kaonMin){
        numKaonALL +=1;
        if(stepNum == 1){
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
  eventAct->PionCHMultLAY(numPionCHLay);
  eventAct->KaonCHMultLAY(numKaonCHLay);
  eventAct->PionMultALL(numPionALL);
  eventAct->KaonMultALL(numKaonALL);  
  eventAct->PionMultLAY(numPionLay);
  eventAct->KaonMultLAY(numKaonLay);
  eventAct->PionMult(numPionSec);
  eventAct->KaonMult(numKaonSec);
  eventAct->NumPiP(numPiP);
  eventAct->NumPiM(numPiM);
  eventAct->NumPi0(numPi0);

  // final check on annihilation
  if(parentID == 0 && annihilate && endEnergy < 0.01){
    eventAct->SetCollision();
    man->FillNtupleDColumn(0,1,pos[0]);
    man->FillNtupleDColumn(0,2,pos[1]);
    man->FillNtupleDColumn(0,3,pos[2]);
  }


}

}
