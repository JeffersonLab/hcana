void slipstudy(TString Spec="", Int_t RunNumber = 0, Int_t MaxEvent = 0) {

  char RunFileNamePattern[100]="";
  if(Spec.Length()==0) {
    cout << "Enter \"shms\", \"hms\", or \"coin\": ";
    cin >> RunFileNamePattern;
  } else {
    strcpy(RunFileNamePattern, Spec.Data());
  }
  // Get RunNumber and MaxEvent if not provided.
  if(RunNumber == 0) {
    cout << "\nEnter a Run Number (-1 to exit): ";
    cin >> RunNumber;
    if( RunNumber<=0 ) return;
  }
  if(MaxEvent == 0) {
    cout << "\nNumber of Events to analyze: ";
    cin >> MaxEvent;
    if(MaxEvent == 0) {
      cerr << "...Invalid entry\n";
      exit;
    }
  }

  // Create file name patterns.
  strcat(RunFileNamePattern, "_all_%05d.dat");
  vector<TString> pathList;
  pathList.push_back(".");
  pathList.push_back("./raw");
  pathList.push_back("./raw/../raw.copiedtotape");
  pathList.push_back("./cache");

  cout << RunFileNamePattern << endl;

  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  gHcParms->AddString("g_ctp_database_filename", "DBASE/slip.database");

  gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);

  // g_ctp_parm_filename and g_decode_map_filename should now be defined

  gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));

  // Constants not in ENGINE PARAM files that we want to be
  // configurable
  gHcParms->Load("PARAM/hcana.param");
  system("cp slip_db_cratemap.dat db_cratemap.dat");

  THcTimeSyncEvtHandler* fadcsynccheck = new THcTimeSyncEvtHandler("SYNC","Synchronization test");
  fadcsynccheck->AddExpectedOffset(1, -6);
  fadcsynccheck->AddExpectedOffset(2, -7);
  fadcsynccheck->AddExpectedOffset(4, -6);
  fadcsynccheck->AddEvtType(1);
  fadcsynccheck->AddEvtType(2);
  fadcsynccheck->AddEvtType(3);
  fadcsynccheck->AddEvtType(4);
  fadcsynccheck->AddEvtType(5);
  fadcsynccheck->AddEvtType(6);
  fadcsynccheck->AddEvtType(7);
  gHaEvtHandlers->Add(fadcsynccheck);

  // Set up the analyzer - we use the standard one,
  // but this could be an experiment-specific one as well.
  // The Analyzer controls the reading of the data, executes
  // tests/cuts, loops over Acpparatus's and PhysicsModules,
  // and executes the output routines.
  THcAnalyzer* analyzer = new THcAnalyzer;

  // A simple event class to be output to the resulting tree.
  // Creating your own descendant of THaEvent is one way of
  // defining and controlling the output.
  THaEvent* event = new THaEvent;

  // Define the run(s) that we want to analyze.
  // We just set up one, but this could be many.
  THcRun* run = new THcRun( pathList, Form(RunFileNamePattern, RunNumber) );
  run->SetRunParamClass("THcRunParameters");

run->SetEventRange(1, MaxEvent); // Physics Event number, does not include scaler or control events.
  run->SetNscan(1);
  run->SetDataRequired(0x7);
  run->Print();

  // Define the analysis parameters
  analyzer->SetCountMode(2);  // 0 = counter is # of physics triggers
                              // 1 = counter is # of all decode reads
                              // 2 = counter is event number

  analyzer->SetEvent(event);
  analyzer->SetOutFile("slipstudy.root");
  analyzer->SetOdefFile("slipstudy.def");
  analyzer->Process(run);
  fadcsynccheck->PrintStats();
}
