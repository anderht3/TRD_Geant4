/*
 * Author: Hannah Anderson
 * Start Date: 03/12/2023
 * Most Recent Changes: 05/01/2023
 * 
 */ 

#include "PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ChargedGeantino.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

namespace TRD
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  //original single particle gun
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);

  // low beta generation
  G4double beta = 0.02;

  //randomize beta values
  //G4double dbeta = 0.1;
  //beta += dbeta *(G4UniformRand()-0.5); 
  G4double velocity = beta * 299792458 * m/s;

  //choose particle for gun
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle
                    = particleTable->FindParticle("anti_deuteron");
  G4double mass = particle->GetPDGMass();

  //choose initial energy
  G4double momentum = velocity * mass / sqrt(1-beta*beta);
  G4double Ekin = sqrt(momentum * momentum + mass * mass) - mass;

  //initialize particle gun
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,99.*cm));
  fParticleGun->SetParticleEnergy(Ekin);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,-1.));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  G4ParticleDefinition* particle = fParticleGun->GetParticleDefinition();

  // randomized position
  G4double x0  = 0*cm, y0  = 0*cm, z0  = 100*cm;
  G4double dx0 = 1*cm, dy0 = 1*cm, dz0 = 1*cm;
  x0 += dx0*(G4UniformRand()-0.5);
  y0 += dy0*(G4UniformRand()-0.5);
  z0 += dz0*(G4UniformRand()-0.5);
  fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));

  //create vertex
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}


