#include <iostream>
#include <sstream>

#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TMath.h"
#include "TLegend.h"
#include "TEllipse.h"
#include "TGraph.h"
#include "TRandom3.h"
#include "TF1.h"

#include "RAT/DSReader.hh"
#include "RAT/DS/MC.hh"

int main( int nargs, char** argv ) {
 
  double innerTankRadius = 172.0;    //cm including scintillator and acrylic 170 + 2 cm
  int numRings = 7;
  int pmtsPerRing = 14;

  bool usePMTQuantumEff = true;

  TRandom3 rand(0);

  double wavelenVals[41] = {280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 600, 610, 620, 630, 640, 650, 660, 670, 800};
  double QEVals[41] = {.002, .02, .08, .13, .17, .21, .22, .23, .235, .24, .24, .24, .235, .23, .225, .22, .21, .20, .19, .17, .15, .13, .11, .1, .09, .08, .07, .06, .05, .04, .03, .02, .015, .01, .006, .003, .001, .0005, .0001, 0.0, 0.0};

  double xOffset = innerTankRadius*(TMath::Pi()/2.0);  	//Make sure PMTs line up right in event display

  if ( nargs<4 ) {
    std::cout << "usage: analyze_data <input RAT root file> <output rootfile> <pmt info file>" << std::endl;
    return 0;
  }

  std::string inputfile = argv[1];
  std::string outfile = argv[2];
  std::stringstream PMTInfoFile;
  PMTInfoFile << argv[3];

  RAT::DSReader* ds = new RAT::DSReader( inputfile.c_str() ); 

  TFile* out = new TFile(outfile.c_str(), "RECREATE" );
  TH1D* totPEVsTime = new TH1D("totPEVsTime", "", 200, -50., 1000.);
  TH1D* cherenkovTotPEVsTime = new TH1D("cherenkovTotPEVsTime", "", 200, -50., 1000.);
  TH1D* scintTotPEVsTime = new TH1D("scintTotPEVsTime", "", 200, -50., 1000.);
  TH1D* reScatTotPEVsTime = new TH1D("reScatTotPEVsTime", "", 200, -50., 1000.);
  TH1D* cherenkovWavelen = new TH1D("cherenkovWavelen", "", 200, 0, 1000);
  TH1D* scintWavelen = new TH1D("scintWavelen", "", 200, 0, 1000);
  TH1D* reemitWavelen = new TH1D("reemitWavelen", "", 200, 0, 1000);
  TH1D* energyDepHist = new TH1D("energyDepHist", "", 100, 20, 60);
  TH1D* energyDepNoQuenchHist = new TH1D("energyDepNoQuenchHist", "", 100, 20, 60);
  TH1D* numScintPhotonHist = new TH1D("numScintPhotons", "", 200, 0, 20000);
  TH1D* numReemitPhotonHist = new TH1D("numReemitPhotons", "", 200, 0, 20000);
  TGraph* QEGraph = new TGraph(41, wavelenVals, QEVals);

  TFile* PMTINFO = new TFile(PMTInfoFile.str().c_str(), "READ");

  // variables we want
  int numPE;
  int numPMT;
  double positionV[3]; // true vertex
  double primaryDirV[3]; // true direction
  double primaryEndR;
  double primaryEndV[3]; // projected muon endpoint
  double primaryMom;  // truth momentum (0 if no muon! NC event)
  double radius, zPosition;  // from truth
  
  TTree* tree = new TTree( "mcdata", "MC Data" );
  // recon
  tree->Branch( "numPE", &numPE, "numPE/I" );
  tree->Branch( "numPMT", &numPMT, "numPMT/I" );
  // truth
  tree->Branch( "positionV", positionV, "positionV[3]/D" );
  tree->Branch( "primaryDirV", primaryDirV, "primaryDirV[3]/D" );
  tree->Branch( "primaryEndV", primaryEndV, "primaryEndV[3]/D" );
  tree->Branch( "primaryEndR", &primaryEndR, "primaryEndR/D" );
  tree->Branch( "primaryMom", &primaryMom, "primaryMom/D" );
  tree->Branch( "radius", &radius, "radius/D" );  
  tree->Branch( "zPosition", &zPosition, "zPosition/D" );

  int ievent = 0;
  int nevents = ds->GetTotal();

  std::cout << "Number of events: " << nevents << std::endl;

  //Get PMT Positions
  TTree* PMTTree = (TTree*)PMTINFO->Get("pmtinfo");
  Float_t x = 0.0, y = 0.0, z = 0.0;
  int opdetid = 0;
  PMTTree->SetBranchAddress("x", &x);
  PMTTree->SetBranchAddress("y", &y);
  PMTTree->SetBranchAddress("z", &z);
  PMTTree->SetBranchAddress("opdetid", &opdetid);

  int numEntries = PMTTree->GetEntries();
  std::cout << numEntries << " PMTs found." << std::endl;
  double xPos[numEntries], yPos[numEntries], zPos[numEntries], phiPos[numEntries];
  for (int i = 0; i < numEntries; i++) {
    PMTTree->GetEntry(i);
    xPos[opdetid] = x; yPos[opdetid] = y; zPos[opdetid] = z;\
    phiPos[opdetid] = TMath::ATan(y/x);
    if (x < 0) phiPos[opdetid] += TMath::Pi();
    else if (y < 0 && x > 0) phiPos[opdetid] += TMath::TwoPi();
    //std::cout << "PMT " << opdetid << " is at " << x << " " << y << " " << z << std::endl;
    //std::cout << "The phi coordinate is: " << phiPos[opdetid] << std::endl;
  }

  out->mkdir("SingleEventPlots");
  out->cd("SingleEventPlots");
  while (ievent < nevents) {
    int numBadTiming = 0;
    RAT::DS::Root* root = ds->NextEvent();

    std::stringstream timeHistName, cherenkovHistName, scintHistName, reScatHistName;
    std::stringstream hitName, cherenkovHitName, scintHitName, reScatHitName;
    timeHistName << "PEVsTime" << ievent;
    cherenkovHistName << "cherenkovPEVsTime" << ievent;
    scintHistName << "scintPEVsTime" << ievent;
    reScatHistName << "reScatPEVsTime" << ievent;
    hitName << "hitsBarrel" << ievent;
    cherenkovHitName << "cherenkovHitsBarrel" << ievent;
    scintHitName << "scintHitsBarrel" << ievent;
    reScatHitName << "reScatHitsBarrel" << ievent;
    
    TH1D* PEVsTime = new TH1D(timeHistName.str().c_str(), "", 200, -50., 1000.);
    TH1D* cherenkovPEVsTime = new TH1D(cherenkovHistName.str().c_str(), "", 200, -50., 1000.);
    TH1D* scintPEVsTime = new TH1D(scintHistName.str().c_str(), "", 200, -50., 1000.);
    TH1D* reScatPEVsTime = new TH1D(reScatHistName.str().c_str(), "", 200, -50., 1000.);

    int numBinsPhi = 2*pmtsPerRing + 1;
    int numBinsZ = 2*numRings + 1;
    TH2D* hitMapBarrel = new TH2D("", "", numBinsPhi, 0., TMath::TwoPi(), numBinsZ, -innerTankRadius, innerTankRadius);
    TH2D* cherenkovHitMapBarrel = new TH2D("", "", numBinsPhi, 0., TMath::TwoPi(), numBinsZ, -innerTankRadius, innerTankRadius);
    TH2D* scintHitMapBarrel = new TH2D("", "", numBinsPhi, 0., TMath::TwoPi(), numBinsZ, -innerTankRadius, innerTankRadius);
    TH2D* reScatHitMapBarrel = new TH2D("", "", numBinsPhi, 0., TMath::TwoPi(), numBinsZ, -innerTankRadius, innerTankRadius);
    TH2D* hitMapTop = new TH2D("", "", numBinsPhi, -TMath::Pi()*innerTankRadius, TMath::Pi()*innerTankRadius, numBinsZ, -innerTankRadius, innerTankRadius);
    TH2D* cherenkovHitMapTop = new TH2D("", "", numBinsPhi, -TMath::Pi()*innerTankRadius, TMath::Pi()*innerTankRadius, numBinsZ, -innerTankRadius, innerTankRadius);
    TH2D* scintHitMapTop = new TH2D("", "", numBinsPhi, -TMath::Pi()*innerTankRadius, TMath::Pi()*innerTankRadius, numBinsZ, -innerTankRadius, innerTankRadius);
    TH2D* reScatHitMapTop = new TH2D("", "", numBinsPhi, -TMath::Pi()*innerTankRadius, TMath::Pi()*innerTankRadius, numBinsZ, -innerTankRadius, innerTankRadius);
    TH2D* hitMapBottom = new TH2D("", "", numBinsPhi, -TMath::Pi()*innerTankRadius, TMath::Pi()*innerTankRadius, numBinsZ, -innerTankRadius, innerTankRadius);
    TH2D* cherenkovHitMapBottom = new TH2D("", "", numBinsPhi, -TMath::Pi()*innerTankRadius, TMath::Pi()*innerTankRadius, numBinsZ, -innerTankRadius, innerTankRadius);
    TH2D* scintHitMapBottom = new TH2D("", "", numBinsPhi, -TMath::Pi()*innerTankRadius, TMath::Pi()*innerTankRadius, numBinsZ, -innerTankRadius, innerTankRadius);
    TH2D* reScatHitMapBottom = new TH2D("", "", numBinsPhi, -TMath::Pi()*innerTankRadius, TMath::Pi()*innerTankRadius, numBinsZ, -innerTankRadius, innerTankRadius);

    // --------------------------------
    // Clear Variables
    numPE = 0;
    numPMT = 0;
    positionV[0] = positionV[1] = positionV[2] = 0.0;
    radius = zPosition = 0.0;
    primaryMom = 0;
    primaryEndV[0] = primaryEndV[1] = primaryEndV[2] = 0.0;
    primaryDirV[0] = primaryDirV[1] = primaryDirV[2] = 0.0;
    primaryEndR = 0;
    // --------------------------------

    RAT::DS::MC* mc = root->GetMC();
    if ( mc==NULL ) {
      std::cout << "mc==NULL" << std::endl;
      break;
    }

    if ( mc->GetMCParticleCount()==0 ) {
      std::cout << "ParticleCount==0" << std::endl;
      tree->Fill();
      ievent++;
      continue;
    }

    // true vertex of primary particle
    positionV[0] = mc->GetMCParticle(0)->GetPosition().X()/10.0; //change to cm
    positionV[1] = mc->GetMCParticle(0)->GetPosition().Y()/10.0; //change to cm
    positionV[2] = mc->GetMCParticle(0)->GetPosition().Z()/10.0; //change to cm
    radius = sqrt(positionV[0]*positionV[0] + positionV[1]*positionV[1]);
    zPosition = positionV[2];

    // true positron momentum
    for (int ipart=0; ipart<mc->GetMCParticleCount(); ipart++) {
      if ( mc->GetMCParticle(ipart)->GetPDGCode()==2212 ) {
	TVector3 mom( mc->GetMCParticle(ipart)->GetMomentum() );
	primaryMom = sqrt( mom.X()*mom.X() + mom.Y()*mom.Y() + mom.Z()*mom.Z() );
	primaryDirV[0] = mom.X()/primaryMom;
	primaryDirV[1] = mom.Y()/primaryMom;
	primaryDirV[2] = mom.Z()/primaryMom;
	double primaryKE = mc->GetMCParticle(ipart)->GetKE();
   	std::cout << "Primary Particle KE: " << primaryKE << std::endl;
	
	for (int i=0; i<3; i++) primaryEndV[i] = positionV[i] + primaryDirV[i]*(primaryKE/1.7);
	primaryEndR = sqrt( primaryEndV[0]*primaryEndV[0] + primaryEndV[1]*primaryEndV[1] );
      }
    }

    numPE = mc->GetNumPE();
    numPMT = mc->GetMCPMTCount();

    // count stuff
    for ( int iPMT = 0; iPMT < numPMT; iPMT++ ) {
      RAT::DS::MCPMT* PMT = mc->GetMCPMT( iPMT );
      int numHits = PMT->GetMCPhotonCount();
      int PMTID = PMT->GetID();


      for (int iHit = 0; iHit < numHits; iHit++) {
	RAT::DS::MCPhoton* hit = PMT->GetMCPhoton( iHit );
	double wavelen = hit->GetLambda() * 1.0e6; //Convert mm to nm

	//Determine the Quantum Efficiency using linear interpolation
	double QuantumEff = 0.0;
	for (int i = 0; i < 41; ++i) {
	  if (wavelenVals[i] > wavelen) {
   	    double upperWavelen = wavelenVals[i];
   	    double lowerWavelen = wavelenVals[i-1];
	    double upperQE = QEVals[i];
	    double lowerQE = QEVals[i-1];
 	    double slope = (upperQE - lowerQE)/(upperWavelen - lowerWavelen);
	    QuantumEff = slope*(wavelen - lowerWavelen) + lowerQE;
	    break;
	  }
	}
	
	//Cut events based on quantum efficiency
	if (rand.Rndm() > QuantumEff && usePMTQuantumEff) continue;

	//Get the track of the photon that generated the PE we are considering
	int trackID = hit->GetTrackID();
	int numTracks = mc->GetMCTrackCount();
	if (trackID == numTracks) continue;  //Some sort of RAT bug allows this once in a while

	//Track Step 0 has 0 length and it's endpoint is the start of the track
	TVector3 photonOrigin = mc->GetMCTrack(trackID)->GetMCTrackStep(0)->GetEndpoint();
	double distX = xPos[PMTID] - photonOrigin.X()*1e-1;  //Convert from mm to cm
	double distY = yPos[PMTID] - photonOrigin.Y()*1e-1;  //Convert from mm to cm
	double distZ = zPos[PMTID] - photonOrigin.Z()*1e-1;  //Convert from mm to cm
	double dist = TMath::Sqrt(distX*distX + distY*distY + distZ*distZ);
	double rIndex = 1.47;
	double lightSpeed = 3e10/rIndex;  	       //In cm/s
	double travelTime = (dist/lightSpeed)*1e9;     //In ns
	
	//RAT::DS::MCTrack *originTrack = mc->GetMCTrack(trackID);
	//RAT::DS::MCTrackStep *firstStep = originTrack->GetMCTrackStep(0);
	//std::cout << firstStep->GetLength() << std::endl;
	
	double tHit = hit->GetHitTime();
	double correctedTime = tHit - travelTime;

	if (correctedTime < 0.0) numBadTiming++;

	totPEVsTime->Fill(correctedTime);
   	PEVsTime->Fill(correctedTime);
	if (hit->GetOriginFlag() == 0 ) {
	  cherenkovTotPEVsTime->Fill(correctedTime);
   	  cherenkovPEVsTime->Fill(correctedTime);
  	  cherenkovWavelen->Fill(wavelen);
	  if (PMTID < numRings*pmtsPerRing) {
	    cherenkovHitMapBarrel->Fill(phiPos[PMTID], zPos[PMTID]);
  	    hitMapBarrel->Fill(phiPos[PMTID], zPos[PMTID]);
	  }
	  else if (PMTID % 2 == 0) {
	    cherenkovHitMapTop->Fill(xPos[PMTID] + xOffset, yPos[PMTID]);
	    hitMapTop->Fill(xPos[PMTID] + xOffset, yPos[PMTID]);
	  }
	  else {
	    cherenkovHitMapBottom->Fill(xPos[PMTID] + xOffset, -yPos[PMTID]);
	    hitMapBottom->Fill(xPos[PMTID] + xOffset, -yPos[PMTID]);
	  }
 	}
	else if (hit->GetOriginFlag() == 1){
	  scintTotPEVsTime->Fill(correctedTime);
   	  scintPEVsTime->Fill(correctedTime);
  	  scintWavelen->Fill(wavelen);
	  if (PMTID < numRings*pmtsPerRing) {
	    scintHitMapBarrel->Fill(phiPos[PMTID], zPos[PMTID]);
  	    hitMapBarrel->Fill(phiPos[PMTID], zPos[PMTID]);
	  }
	  else if (PMTID % 2 == 0) {
	    scintHitMapTop->Fill(xPos[PMTID] + xOffset, yPos[PMTID]);
	    hitMapTop->Fill(xPos[PMTID] + xOffset, yPos[PMTID]);
	  }
	  else {
	    scintHitMapBottom->Fill(xPos[PMTID] + xOffset, -yPos[PMTID]);
	    hitMapBottom->Fill(xPos[PMTID] + xOffset, -yPos[PMTID]);
	  }
	}
        else {
	  reScatTotPEVsTime->Fill(correctedTime);
	  reScatPEVsTime->Fill(correctedTime);
  	  reemitWavelen->Fill(wavelen);
	  if (PMTID < numRings*pmtsPerRing) {
	    reScatHitMapBarrel->Fill(phiPos[PMTID], zPos[PMTID]);
  	    hitMapBarrel->Fill(phiPos[PMTID], zPos[PMTID]);
	  }
	  else if (PMTID % 2 == 0) {
	    reScatHitMapTop->Fill(xPos[PMTID] + xOffset, yPos[PMTID]);
	    hitMapTop->Fill(xPos[PMTID] + xOffset, yPos[PMTID]);
	  }
	  else {
	    reScatHitMapBottom->Fill(xPos[PMTID] + xOffset, -yPos[PMTID]);
	    hitMapBottom->Fill(xPos[PMTID] + xOffset, -yPos[PMTID]);
	  }
	}
      }
    }


    double energyDep = mc->GetMCSummary()->GetTotalScintEdep();
    double energyDepNoQuench = mc->GetMCSummary()->GetTotalScintEdepQuenched();
    double numScintPhotons = mc->GetMCSummary()->GetNumScintPhoton();
    double numReemitPhotons = mc->GetMCSummary()->GetNumReemitPhoton();
    energyDepHist->Fill(energyDep);
    energyDepNoQuenchHist->Fill(energyDepNoQuench);
    numScintPhotonHist->Fill(numScintPhotons);
    numReemitPhotonHist->Fill(numReemitPhotons);
    
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "EVENT " << ievent << std::endl;
    std::cout << " PEs: " << numPE << " PMTs: " << numPMT << std::endl;
    std::cout << " PEs with Corrected Time < 0: " << numBadTiming << std::endl;
    std::cout << "Position: " << positionV[0] << ", " << positionV[1] << ", " << positionV[2] << std::endl;
    std::cout << "Total Energy Deposited: " << energyDep << " MeV" << std::endl;
    std::cout << "Energy Deposited with Quenched Energy Removed: " << energyDepNoQuench << " MeV" << std::endl;
    std::cout << "Number of Scint Photons Generated: " << numScintPhotons << std::endl;
    std::cout << "Number of Reemit Photons Generated: " << numReemitPhotons << std::endl;
    ievent++; 
    tree->Fill();

    TCanvas* canEvent = new TCanvas(timeHistName.str().c_str(), "", 800, 600);
    PEVsTime->SetStats(0);
    PEVsTime->SetLineColor(1);
    PEVsTime->GetXaxis()->SetTitle("Time (ns)");
    PEVsTime->GetYaxis()->SetTitle("PEs");
    PEVsTime->Draw();
    cherenkovPEVsTime->SetStats(0);
    cherenkovPEVsTime->SetLineColor(4);
    cherenkovPEVsTime->Draw("same");
    scintPEVsTime->SetStats(0);
    scintPEVsTime->SetLineColor(2);
    scintPEVsTime->Draw("same");
    reScatPEVsTime->SetStats(0);
    reScatPEVsTime->SetLineColor(8);
    reScatPEVsTime->Draw("same");
    double fracCherenkov = 0, fracScint = 0, fracReScat = 0;
    fracCherenkov = 100*((double)cherenkovPEVsTime->GetEntries()/(double)PEVsTime->GetEntries());
    fracScint = 100*((double)scintPEVsTime->GetEntries()/(double)PEVsTime->GetEntries());
    fracReScat = 100*((double)reScatPEVsTime->GetEntries()/(double)PEVsTime->GetEntries());
    std::stringstream entry1, entry2, entry3;
    entry1 << "Cherenkov Light ~ " << fracCherenkov << " %";
    entry2 << "Scintillation Light ~ " << fracScint << " %";
    entry3 << "Re-emitted Light ~ " << fracReScat << " %";
    TLegend* legendEvent = new TLegend(0.6, 0.85, 0.9, 0.65); 
    legendEvent->AddEntry(PEVsTime, "Total Light", "l");
    legendEvent->AddEntry(cherenkovPEVsTime, entry1.str().c_str(), "l");
    legendEvent->AddEntry(scintPEVsTime, entry2.str().c_str(), "l");
    legendEvent->AddEntry(reScatPEVsTime, entry3.str().c_str(), "l");
    legendEvent->Draw("same");
    canEvent->SetLogy();
    canEvent->Write();
    
    TCanvas* canHitMap = new TCanvas(hitName.str().c_str(), "", 1200, 1200);
    canHitMap->Divide(1, 3, .01, .01);
    
    double maxHits = TMath::Max(hitMapBarrel->GetMaximum(), TMath::Max(hitMapTop->GetMaximum(), hitMapBottom->GetMaximum()));
    
    canHitMap->cd(1);
    hitMapTop->SetStats(0);
    hitMapTop->SetTitle("Total PMT Hits");
    hitMapTop->GetZaxis()->SetRangeUser(0, maxHits);
    hitMapTop->Draw("COLZ");
    TEllipse* endCap = new TEllipse(xOffset, 0, 170., 170.);
    endCap->SetFillStyle(0);
    endCap->SetLineWidth(2);
    endCap->Draw("same");
    
    canHitMap->cd(2);
    hitMapBarrel->SetStats(0);
    hitMapBarrel->GetZaxis()->SetRangeUser(0, maxHits);
    hitMapBarrel->Draw("COLZ");

    canHitMap->cd(3);
    hitMapBottom->SetStats(0);
    hitMapBottom->GetZaxis()->SetRangeUser(0, maxHits);
    hitMapBottom->Draw("COLZ");
    endCap->Draw("same");
   
    canHitMap->Write();
   
    maxHits = TMath::Max(cherenkovHitMapBarrel->GetMaximum(), TMath::Max(cherenkovHitMapTop->GetMaximum(), cherenkovHitMapBottom->GetMaximum()));
 
    TCanvas* canCherenkovHitMap = new TCanvas(cherenkovHitName.str().c_str(), "", 1200, 1200);
    canCherenkovHitMap->Divide(1, 3, .01, .01);
    
    canCherenkovHitMap->cd(1);
    cherenkovHitMapTop->SetStats(0);
    cherenkovHitMapTop->SetTitle("Total PMT Hits");
    cherenkovHitMapTop->GetZaxis()->SetRangeUser(0, maxHits);
    cherenkovHitMapTop->Draw("COLZ");
    endCap->Draw("same");
    
    canCherenkovHitMap->cd(2);
    cherenkovHitMapBarrel->SetStats(0);
    cherenkovHitMapBarrel->GetZaxis()->SetRangeUser(0, maxHits);
    cherenkovHitMapBarrel->Draw("COLZ");

    canCherenkovHitMap->cd(3);
    cherenkovHitMapBottom->SetStats(0);
    cherenkovHitMapBottom->GetZaxis()->SetRangeUser(0, maxHits);
    cherenkovHitMapBottom->Draw("COLZ");
    endCap->Draw("same");
   
    canCherenkovHitMap->Write();
    
    maxHits = TMath::Max(scintHitMapBarrel->GetMaximum(), TMath::Max(scintHitMapTop->GetMaximum(), scintHitMapBottom->GetMaximum()));
    
    TCanvas* canScintHitMap = new TCanvas(scintHitName.str().c_str(), "", 1200, 1200);
    canScintHitMap->Divide(1, 3, .01, .01);
    
    canScintHitMap->cd(1);
    scintHitMapTop->SetStats(0);
    scintHitMapTop->SetTitle("Total PMT Hits");
    scintHitMapTop->GetZaxis()->SetRangeUser(0, maxHits);
    scintHitMapTop->Draw("COLZ");
    endCap->Draw("same");
    
    canScintHitMap->cd(2);
    scintHitMapBarrel->SetStats(0);
    scintHitMapBarrel->GetZaxis()->SetRangeUser(0, maxHits);
    scintHitMapBarrel->Draw("COLZ");

    canScintHitMap->cd(3);
    scintHitMapBottom->SetStats(0);
    scintHitMapBottom->GetZaxis()->SetRangeUser(0, maxHits);
    scintHitMapBottom->Draw("COLZ");
    endCap->Draw("same");
   
    canScintHitMap->Write();

    maxHits = TMath::Max(reScatHitMapBarrel->GetMaximum(), TMath::Max(reScatHitMapTop->GetMaximum(), reScatHitMapBottom->GetMaximum()));
    
    TCanvas* canReScatHitMap = new TCanvas(reScatHitName.str().c_str(), "", 1200, 1200);
    canReScatHitMap->Divide(1, 3, .01, .01);
    
    canReScatHitMap->cd(1);
    reScatHitMapTop->SetStats(0);
    reScatHitMapTop->SetTitle("Total PMT Hits");
    reScatHitMapTop->GetZaxis()->SetRangeUser(0, maxHits);
    reScatHitMapTop->Draw("COLZ");
    endCap->Draw("same");
    
    canReScatHitMap->cd(2);
    reScatHitMapBarrel->SetStats(0);
    reScatHitMapBarrel->GetZaxis()->SetRangeUser(0, maxHits);
    reScatHitMapBarrel->Draw("COLZ");

    canReScatHitMap->cd(3);
    reScatHitMapBottom->SetStats(0);
    reScatHitMapBottom->GetZaxis()->SetRangeUser(0, maxHits);
    reScatHitMapBottom->Draw("COLZ");
    endCap->Draw("same");
   
    canReScatHitMap->Write();
    
    canHitMap->Clear();
    PEVsTime->Reset();
    cherenkovPEVsTime->Reset();
    scintPEVsTime->Reset();
    reScatPEVsTime->Reset();
    hitMapBarrel->Reset();
    cherenkovHitMapBarrel->Reset();
    scintHitMapBarrel->Reset();
    reScatHitMapBarrel->Reset();
    hitMapTop->Reset();
    cherenkovHitMapTop->Reset();
    scintHitMapTop->Reset();
    reScatHitMapTop->Reset();
    hitMapBottom->Reset();
    cherenkovHitMapBottom->Reset();
    scintHitMapBottom->Reset();
    reScatHitMapBottom->Reset();
  } //end of while loop

  std::cout << "Writing." << std::endl;


  out->cd();
  tree->Write();
  totPEVsTime->Write();
  TCanvas* canTotal = new TCanvas("totPEVsTime", "", 800, 600);
  totPEVsTime->SetStats(0);
  totPEVsTime->SetLineColor(1);
  totPEVsTime->Scale(1./(double)totPEVsTime->GetEntries());
  totPEVsTime->GetXaxis()->SetTitle("Time (ns)");
  totPEVsTime->GetYaxis()->SetTitle("Normalized Likelihood");
  totPEVsTime->GetYaxis()->SetTitleOffset(1.5);
  totPEVsTime->Draw();
  cherenkovTotPEVsTime->SetStats(0);
  cherenkovTotPEVsTime->SetLineColor(4);
  cherenkovTotPEVsTime->Scale(1./(double)totPEVsTime->GetEntries());
  cherenkovTotPEVsTime->Draw("same");
  scintTotPEVsTime->SetStats(0);
  scintTotPEVsTime->SetLineColor(2);
  scintTotPEVsTime->Scale(1./(double)totPEVsTime->GetEntries());
  scintTotPEVsTime->Draw("same");  
  reScatTotPEVsTime->SetLineColor(8);
  reScatTotPEVsTime->Scale(1./(double)totPEVsTime->GetEntries());
  reScatTotPEVsTime->Draw("same");  
  double fracCherenkovTot = 0, fracScintTot = 0, fracReScatTot = 0;
  fracCherenkovTot = 100*((double)cherenkovTotPEVsTime->GetEntries()/(double)totPEVsTime->GetEntries());
  fracScintTot = 100*((double)scintTotPEVsTime->GetEntries()/(double)totPEVsTime->GetEntries());
  fracReScatTot = 100*((double)reScatTotPEVsTime->GetEntries()/(double)totPEVsTime->GetEntries());
  std::stringstream entry1Tot, entry2Tot, entry3Tot;
  entry1Tot << "Cherenkov Light ~ " << fracCherenkovTot << " %";
  entry2Tot << "Scintillation Light ~ " << fracScintTot << " %";
  entry3Tot << "Re-emitted Light ~ " << fracReScatTot << " %";
  TLegend* legendTot = new TLegend(0.6, 0.85, 0.9, 0.65); 
  legendTot->AddEntry(totPEVsTime, "Total Light", "l");
  legendTot->AddEntry(cherenkovTotPEVsTime, entry1Tot.str().c_str(), "l");
  legendTot->AddEntry(scintTotPEVsTime, entry2Tot.str().c_str(), "l");
  legendTot->AddEntry(reScatTotPEVsTime, entry3Tot.str().c_str(), "l");
  legendTot->Draw("same");
  canTotal->SetLogy();

  canTotal->Write();

  TCanvas* wavelenCan = new TCanvas("wavelengthPlot", "", 800, 600);
  cherenkovWavelen->SetStats(0);
  cherenkovWavelen->SetLineColor(4); 
  cherenkovWavelen->GetYaxis()->SetRangeUser(0, 1.1*scintWavelen->GetMaximum());
  cherenkovWavelen->GetXaxis()->SetTitle("Wavelength (nm)");
  cherenkovWavelen->Draw();
  scintWavelen->SetStats(0);
  scintWavelen->SetLineColor(2); 
  scintWavelen->Draw("same");
  reemitWavelen->SetStats(0);
  reemitWavelen->SetLineColor(8); 
  reemitWavelen->Draw("same");
  TLegend* legendWavelen = new TLegend(0.6, 0.85, 0.9, 0.65); 
  legendWavelen->AddEntry(cherenkovWavelen, entry1Tot.str().c_str(), "l");
  legendWavelen->AddEntry(scintWavelen, entry2Tot.str().c_str(), "l");
  legendWavelen->AddEntry(reemitWavelen, entry3Tot.str().c_str(), "l");
  legendWavelen->Draw("same");
  wavelenCan->Write();

  TCanvas* QECanvas = new TCanvas("QEPlot", "", 563, 741);
  QEGraph->SetMarkerColor(0);
  QEGraph->SetMarkerStyle(1);
  QEGraph->SetLineStyle(2); 
  QEGraph->SetLineWidth(2);
  QEGraph->SetTitle("QE of Hamamatsu R5912 PMT"); 
  QEGraph->Draw("ACP");
  QEGraph->GetXaxis()->SetTitle("Wavelength");
  QEGraph->GetYaxis()->SetTitle("QE");
  QEGraph->GetXaxis()->SetLimits(100, 800);
  QEGraph->GetHistogram()->SetMinimum(0.0001);
  QEGraph->GetHistogram()->SetMaximum(1.0);
  QECanvas->SetLogy();
  QECanvas->Write(); 

  energyDepHist->Write();
  energyDepNoQuenchHist->Write();
  numScintPhotonHist->Write();
  numReemitPhotonHist->Write();

  std::cout << "Finished." << std::endl;

  return 0;
}
