/*
 * Author: Hannah Anderson
 * Start Date: 03/12/2023
 * Most Recent Changes: 05/01/2023
 * 
 */ 

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"


namespace TRD
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
{
  // Register accumulable to the accumulable manager
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->RegisterAccumulable(fGoodEvents);
  accumulableManager->RegisterAccumulable(fGoodLayer);
  accumulableManager->RegisterAccumulable(fGoodPion);
  accumulableManager->RegisterAccumulable(fGoodOne);

}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* run)
{
  G4bool ntuple = false;
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;

  // reset accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();

  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  //create histograms and ntuples
  G4AnalysisManager *man = G4AnalysisManager::Instance();
  man->OpenFile("antideuteron.root");

  man->CreateH1("Mult", "Total Event Multiplicity", 100, 0, 50);
  man->CreateH1("MultC", "Collision Event Multiplicity", 100, 0, 50);
  man->CreateH1("MultCH", "Charged Particle Collision Multiplicity", 100, 0, 20);
  man->CreateH1("Pmom", "Pion Energy", 100, 0, 20);
  man->CreateH1("Kmom", "Kaon Energy", 100, 0, 20);
  man->CreateH1("PmomL", "Pion Energy Layers", 100, 0, 20);
  man->CreateH1("KmomL", "Kaon Energy Layers", 100, 0, 20);  
  man->CreateH1("LayP", "Charged Particles Layer Multiplicity", 100, 0, 20);
  man->CreateH1("MultCHPL", "Charged Pion Multiplicity at Layer", 100,0,20);
  man->CreateH1("MultCHKL", "Charged Kaon Multiplicity at Layer", 100,0,20);
  man->CreateH1("PmomLALL", "Pion Momentum Layers ALL", 100, 0, 20);
  man->CreateH1("KmomLALL", "Kaon Momentum Layers ALL", 100, 0, 20);
  man->CreateH1("PmultLALL", "Pion Multiplicity Layers ALL", 100, 0, 20);
  man->CreateH1("KmultLALL", "Kaon Multiplicity Layers ALL", 100, 0, 20);
  man->CreateH1("PmomTot", "Layer Hits Pion Momentum", 100, 0, 20);
  man->CreateH1("MultPL", "Pion Multiplicity at Layer", 100, 0 ,20);
  man->CreateH1("MultKL", "Kaon Multiplicity at Layer", 100, 0, 20);
  man->CreateH1("MultP", "Pion Secondary Multiplicity", 100,0, 20);
  man->CreateH1("MultK", "Kaon Secondary Multiplicity", 100,0,20);
  man->CreateH1("MomLP", "Pion Layer Momentum", 100, 0, 20);
  man->CreateH1("NumPiP", "Multiplicity of Pion+", 100, 0, 20);
  man->CreateH1("NumPiM", "Multiplicity of Pion-", 100, 0, 20);
  man->CreateH1("NumPi0", "Multiplicity of Pion0", 100, 0, 20);
  man->CreateH1("betaPi", "Beta of Pions at Layer", 100, 0, 1);

  //man->CreateH1("OneHit", "Events with only one layer hit", 100, 0, 20);

  //ntuples for annihilated particles
  man->CreateNtuple("Collision", "Collision");
  man->CreateNtupleIColumn("fEvent");
  man->CreateNtupleDColumn("fX");
  man->CreateNtupleDColumn("fy");
  man->CreateNtupleDColumn("fz");
  man->CreateNtupleDColumn("edep");
  man->FinishNtuple();

  //ntuples for all events
  man->CreateNtuple("Hits", "Hits");
  man->CreateNtupleIColumn("fEvent");
  man->CreateNtupleDColumn("trdedep");
  man->CreateNtupleIColumn("secondaries");
  man->FinishNtuple();

  //ntuples of annihilations to have more info on secondaries
  man->CreateNtuple("Production", "Production");
  man->CreateNtupleIColumn("fEvent");
  man->CreateNtupleIColumn("numSecond");
  man->CreateNtupleIColumn("secondaries");
  man->CreateNtupleDColumn("fEnergy");
  man->CreateNtupleIColumn("layer");
  man->FinishNtuple();
  

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  // Merge accumulables
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Merge();

  // Run conditions
  //  note: There is no primary generator action object for "master"
  //        run manager for multi-threaded mode.
  const auto generatorAction = static_cast<const PrimaryGeneratorAction*>(
    G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String partName;
  if (generatorAction)
  {
    G4ParticleDefinition* particle
      = generatorAction->GetParticleGun()->GetParticleDefinition();
    partName = particle->GetParticleName();
  }
  
  //write and close root file 
  G4AnalysisManager *man = G4AnalysisManager::Instance();
  man->Write();
  man->CloseFile();

  
  // Print results
  if (IsMaster())
  {
    G4cout
     << G4endl
     << "--------------------End of Global Run-----------------------"
     << G4endl
     << "  The run was " << nofEvents << " events ";
  }
  else
  {
    G4cout
     << G4endl
     << "--------------------End of Local Run------------------------"
     << G4endl
     << "  The run was " << nofEvents << " "<< partName;
  }
  G4cout
     << "; Nb of events: " << fGoodEvents.GetValue()  << G4endl
     << " Nb of layer hits : " << fGoodLayer.GetValue() << G4endl
     << " Nb of pion layer hits: " << fGoodPion.GetValue() << G4endl
     << " Nb of events with one layer hit: " << fGoodOne.GetValue()
     << G4endl
     << "------------------------------------------------------------" << G4endl
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

