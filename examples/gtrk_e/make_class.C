#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#include <iostream>

using namespace std;

void make_class(string root_file="hodtest_52949.root",
		string tree_name="T;1",
		string class_name="hcana_class") {

  TFile* r_file = new TFile(root_file.c_str());
  //  TTree *T = (TTree*)gROOT->FindObject("T;1");
  TTree *T = (TTree*)gROOT->FindObject(tree_name.c_str());
  T->MakeClass(class_name.c_str());

}
