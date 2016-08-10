#include <iostream>
#include <sstream>

#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TRandom3.h"
#include "TLine.h"

#include "RAT/DSReader.hh"
#include "RAT/DS/MC.hh"

//Compile with:
// g++ -o neutronPositronSep neutronPositronSep.cc `root-config --cflags --glibs` -I/home/jrlowery/jsns2/ratpac-kpipe/include
//

int main( int nargs, char** argv ) {

  double timeCutoff = 30.0;   //Size of time window at beginning of event to collect light
			      //to perform proton gamma separation.

  double cutValue = 150.0;   //Cut on number of PEs to separate protons and positrons
  int numPosUnderCut = 0;
  int numNeutOverCut = 0;

  bool usePMTQuantumEff = true;

  TRandom3 rand(0);

  double wavelenVals[41] = {280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490, 500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 600, 610, 620, 630, 640, 650, 660, 670, 800};
  double QEVals[41] = {.002, .02, .08, .13, .17, .21, .22, .23, .235, .24, .24, .24, .235, .23, .225, .22, .21, .20, .19, .17, .15, .13, .11, .1, .09, .08, .07, .06, .05, .04, .03, .02, .015, .01, .006, .003, .001, .0005, .0001, 0.0, 0.0};

  if ( nargs < 4 ) {
    std::cout << "usage: analyze_data <input RAT root file (neutrons)> <input RAT root file (positrons)> <output rootfile>" << std::endl;
    return 0;
  }

  std::string inputfileN = argv[1];
  std::string inputfileP = argv[2];
  std::string outfile = argv[3];

  RAT::DSReader* dsN = new RAT::DSReader( inputfileN.c_str() );
  RAT::DSReader* dsP = new RAT::DSReader( inputfileP.c_str() );

  TFile* out = new TFile(outfile.c_str(), "RECREATE" );

  TH1D* earlyPEsN = new TH1D("earlyPEsN", "", 300, -200, 1000);
  TH1D* earlyPEsP = new TH1D("earlyPEsP", "", 300, -200, 1000);
  TH1D* wavelenN = new TH1D("wavelenN", "", 500, 200, 1000);
  TH1D* wavelenP = new TH1D("wavelenP", "", 500, 200, 1000);
  TH1D* timeHistN = new TH1D("timeHistN", "", 450, 0, 900);
  TH1D* timeHistP = new TH1D("timeHistP", "", 450, 0, 900);

  
  int numNeutronEvents = dsN->GetTotal();
  int numPositronEvents = dsP->GetTotal();

  std::cout << "Number of neutron events: " << numNeutronEvents << std::endl;
  std::cout << "Number of positron events: " << numPositronEvents << std::endl;

  int numEarlyPE = 0;
  int maxEarlyPE = 0;    //Use to make histo axes appropriate.
  int numPMT = 0;

  for (int i = 0; i < numNeutronEvents; ++i) {

    RAT::DS::Root* root = dsN->NextEvent();
    numEarlyPE = 0;
    numPMT = 0;

    RAT::DS::MC* mc = root->GetMC();
    if ( mc==NULL ) {
      std::cout << "mc==NULL" << std::endl;
      break;
    }

    if ( mc->GetMCParticleCount()==0 ) {
      std::cout << "ParticleCount==0" << std::endl;
      continue;
    }

    numPMT = mc->GetMCPMTCount();

    // count stuff
    for ( int iPMT = 0; iPMT < numPMT; iPMT++ ) {
      RAT::DS::MCPMT* PMT = mc->GetMCPMT( iPMT );
      int numHits = PMT->GetMCPhotonCount();
      int PMTID = PMT->GetID();

      for (int iHit = 0; iHit < numHits; iHit++) {
	RAT::DS::MCPhoton* hit = PMT->GetMCPhoton( iHit );
	double tHit = hit->GetHitTime();
  	timeHistN->Fill(tHit);
 	double wavelen = hit->GetLambda() * 1.0e6; //Convert mm to nm
	wavelenN->Fill(wavelen);
	
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
	if (tHit < timeCutoff) numEarlyPE++;
      }
    }

    if (numEarlyPE > maxEarlyPE) maxEarlyPE = numEarlyPE;
    if (numEarlyPE > cutValue) numNeutOverCut++;
    earlyPEsN->Fill(numEarlyPE);    

//    std::cout << "------------------------------------------" << std::endl;
//    std::cout << "NEUTRON EVENT " << i << std::endl;
//    std::cout << "PMTs: " << numPMT << std::endl;
//    std::cout << "PEs in first " << timeCutoff << " ns: " << numEarlyPE << std::endl;

  } //end of neutron for loop

  for (int i = 0; i < numPositronEvents; ++i) {

    RAT::DS::Root* root = dsP->NextEvent();
    numEarlyPE = 0;
    numPMT = 0;

    RAT::DS::MC* mc = root->GetMC();
    if ( mc==NULL ) {
      std::cout << "mc==NULL" << std::endl;
      break;
    }

    if ( mc->GetMCParticleCount()==0 ) {
      std::cout << "ParticleCount==0" << std::endl;
      continue;
    }

    numPMT = mc->GetMCPMTCount();

    // count stuff
    for ( int iPMT = 0; iPMT < numPMT; iPMT++ ) {
      RAT::DS::MCPMT* PMT = mc->GetMCPMT( iPMT );
      int numHits = PMT->GetMCPhotonCount();
      int PMTID = PMT->GetID();


      for (int iHit = 0; iHit < numHits; iHit++) {
	RAT::DS::MCPhoton* hit = PMT->GetMCPhoton( iHit );
	double tHit = hit->GetHitTime();
  	timeHistP->Fill(tHit);
 	double wavelen = hit->GetLambda() * 1.0e6; //Convert mm to nm
	wavelenP->Fill(wavelen);
         
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
	if (tHit < timeCutoff) numEarlyPE++;
      }
    }

    if (numEarlyPE > maxEarlyPE) maxEarlyPE = numEarlyPE;
    if (numEarlyPE < cutValue) numPosUnderCut++;
    earlyPEsP->Fill(numEarlyPE);    

//    std::cout << "------------------------------------------" << std::endl;
//    std::cout << "POSITRON EVENT " << i << std::endl;
//    std::cout << "PMTs: " << numPMT << std::endl;
//    std::cout << "PEs in first " << timeCutoff << " ns: " << numEarlyPE << std::endl;

  } //end of positron for loop
  
  std::cout << "Writing." << std::endl;
  std::cout << "============================================" << std::endl;
  std::cout << "Rejection Factor: " << (double)numNeutronEvents/(double)numNeutOverCut  << std::endl;
  std::cout << "Efficiency: " << (double)(numPositronEvents - numPosUnderCut)/(double)numPositronEvents << std::endl;

  double maxEntries = TMath::Max(earlyPEsP->GetMaximum(), earlyPEsN->GetMaximum());

  TCanvas* can = new TCanvas("neutronPositronSeparation", "", 1200, 900);
  earlyPEsP->GetXaxis()->SetRangeUser(-200, 1.1*maxEarlyPE);
  earlyPEsP->GetYaxis()->SetRangeUser(0, 1.1*maxEntries);
  std::stringstream axisTitle;
  axisTitle << "Number of PEs in First " << timeCutoff << " ns";
  earlyPEsP->GetXaxis()->SetTitle(axisTitle.str().c_str());
  earlyPEsP->SetTitle(axisTitle.str().c_str());
  earlyPEsP->SetLineColor(2);
  earlyPEsP->SetLineWidth(2);
  earlyPEsP->SetStats(0);
  earlyPEsP->Draw();
  earlyPEsN->SetLineColor(1);
  earlyPEsN->SetLineWidth(2);
  earlyPEsN->GetXaxis()->SetRangeUser(0, 1.1*maxEarlyPE);
  earlyPEsP->GetYaxis()->SetRangeUser(0, 1.1*maxEntries);
  earlyPEsN->SetStats(0);
  earlyPEsN->Draw("same");

  TLegend* legend = new TLegend(.3, .7, .7, .9);
  legend->AddEntry(earlyPEsN, "50 MeV Proton Events", "l");
  legend->AddEntry(earlyPEsP, "50 MeV Positron Events", "l");
  legend->Draw("same");

  TLine* cutLine = new TLine(cutValue, 0, cutValue, 1.1*maxEntries);
  cutLine->SetLineWidth(2);
  cutLine->SetLineColor(4);
  cutLine->Draw("same");

  can->Write();
  wavelenN->Write();
  wavelenP->Write();
  timeHistN->Write();
  timeHistP->Write();

  out->Close();

  std::cout << "Finished." << std::endl;

  return 0;
}
