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
  strcat(RunFileNamePattern, "_all_%05d");
  char FilteredFileNamePattern[100];
  strcpy(FilteredFileNamePattern, RunFileNamePattern);
  strcat(RunFileNamePattern, ".dat");
  strcat(FilteredFileNamePattern, "_filtered.dat");
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

  THcTimeSyncEvtHandler* synccheck = new THcTimeSyncEvtHandler("SYNC","Synchronization test");
  synccheck->AddExpectedOffset(1, -6);
  synccheck->AddExpectedOffset(2, -7);
  synccheck->AddExpectedOffset(4, -6);
  synccheck->AddEvtType(1);
  synccheck->AddEvtType(2);
  synccheck->AddEvtType(3);
  synccheck->AddEvtType(4);
  synccheck->AddEvtType(5);
  synccheck->AddEvtType(6);
  synccheck->AddEvtType(7);
  synccheck->SetResync(kTRUE);	// Use kFALSE for runs before April 12, 2018, 11AM
  synccheck->SetBadSyncSizeTrigger(450); // Trigger sync fixing if 1190 bank bigger than this
  synccheck->SetBadROC(3);	// The ROC experiencing sync problems
  synccheck->SetRewriteFile(Form(FilteredFileNamePattern, RunNumber));
  gHaEvtHandlers->Add(synccheck);

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
  synccheck->PrintStats();
}
