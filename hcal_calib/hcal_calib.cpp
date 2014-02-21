#include "TCanvas.h"
#include "THcShowerCalib.h"

void hcal_calib(Int_t RunNumber) {
 
 cout << "Calibrating run " << RunNumber << endl;

 THcShowerCalib theShowerCalib(RunNumber);

 theShowerCalib.ExtractData();
 theShowerCalib.Init();
 theShowerCalib.CalcThresholds();
 theShowerCalib.ComposeVMs();
 theShowerCalib.SolveAlphas();
 theShowerCalib.FillHEcal();
 theShowerCalib.SaveAlphas();
 
 TCanvas* Canvas =
   new TCanvas("Canvas", "HMS Shower Counter calibration", 1000, 667);
 Canvas->Divide(2,2);

 Canvas->cd(1);

 theShowerCalib.hEunc->DrawCopy();
  
 theShowerCalib.hEuncSel->SetFillColor(kGreen);
 theShowerCalib.hEuncSel->DrawCopy("same");

 Canvas->cd(3);
 // theShowerCalib.hEcal->Draw();
 theShowerCalib.hEcal->Fit("gaus");

 Canvas->cd(4);
 theShowerCalib.hPvsEcal->Draw();

}
