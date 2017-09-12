#include "TCanvas.h"
#include "THcShowerCalib.h"

//
// A steering Root script for the HMS calorimeter calibration.
//

void hcal_calib(Int_t RunNumber) {
 
 cout << "Calibrating run " << RunNumber << endl;

 THcShowerCalib theShowerCalib(RunNumber);

 theShowerCalib.Init();            // Initialize constants and variables
 theShowerCalib.CalcThresholds();  // Thresholds on the uncalibrated Edep/P
 theShowerCalib.ComposeVMs();      // Compute vectors amd matrices for calib.
 theShowerCalib.SolveAlphas();     // Solve for the calibration constants
 theShowerCalib.SaveAlphas();      // Save the constants
 theShowerCalib.SaveRawData();  // Save raw data into file for debug purposes
 theShowerCalib.FillHEcal();       // Fill histograms

 // Plot histograms

 TCanvas* Canvas =
   new TCanvas("Canvas", "HMS Shower Counter calibration", 1000, 667);
 Canvas->Divide(2,2);

 Canvas->cd(1);

 // Normalized uncalibrated energy deposition.

 theShowerCalib.hEunc->DrawCopy();
  
 theShowerCalib.hEuncSel->SetFillColor(kGreen);
 theShowerCalib.hEuncSel->DrawCopy("same");

 // Normalized energy deposition after calibration.

 Canvas->cd(3);
 theShowerCalib.hEcal->Fit("gaus");

 // HMS delta(P) versus the calibrated energy deposition.

 Canvas->cd(4);
 theShowerCalib.hDPvsEcal->Draw();

}
