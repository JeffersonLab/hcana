void hcal_replay(Int_t RunNumber=52949, Int_t MaxEventToReplay=11000) {

  char* RunFileNamePattern="daq04_%d.log.0";

  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  gHcParms->AddString("g_ctp_database_filename", "DBASE/test.database");
  
  gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);

  // g_ctp_parm_filename and g_decode_map_filename should now be defined

  gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));

  // Constants not in ENGINE PARAM files that we want to be
  // configurable
  gHcParms->Load("PARAM/hcana.param");

  // Generate db_cratemap to correspond to map file contents
  char command[100];
  sprintf(command,"./make_cratemap.pl < %s > db_cratemap.dat",
	  gHcParms->GetString("g_decode_map_filename"));
  system(command);

  // Load the Hall C style detector map
  gHcDetectorMap=new THcDetectorMap();
  gHcDetectorMap->Load(gHcParms->GetString("g_decode_map_filename"));

  // Set up the equipment to be analyzed.

  THaApparatus* HMS = new THcHallCSpectrometer("H","HMS");
  gHaApps->Add( HMS );

  // Add HMS detectors
  HMS->AddDetector( new THcHodoscope("hod", "Hodoscope" ));
  HMS->AddDetector( new THcShower("cal", "Shower" ));
  HMS->AddDetector( new THcDC("dc", "Drift Chambers" ));
  HMS->AddDetector( new THcAerogel("aero", "Aerogel Cerenkov" ));
  HMS->AddDetector( new THcCherenkov("cer", "Gas Cerenkov" ));

  // Set up the analyzer - we use the standard one,
  // but this could be an experiment-specific one as well.
  // The Analyzer controls the reading of the data, executes
  // tests/cuts, loops over Acpparatus's and PhysicsModules,
  // and executes the output routines.
  THaAnalyzer* analyzer = new THcAnalyzer;

  // A simple event class to be output to the resulting tree.
  // Creating your own descendant of THaEvent is one way of
  // defining and controlling the output.
  THaEvent* event = new THaEvent;
  
  // Define the run(s) that we want to analyze.
  // We just set up one, but this could be many.
  char RunFileName[100];
  sprintf(RunFileName,RunFileNamePattern,RunNumber);
  THaRun* run = new THaRun(RunFileName);

  // Eventually need to learn to skip over, or properly analyze
  // the pedestal events
  run->SetEventRange(1,MaxEventToReplay);  //  Physics Event number, does not
                                           //  include scaler or control events

  // Define the analysis parameters
  analyzer->SetCountMode( 0 );   //Mark's modification
  analyzer->SetEvent( event );
  analyzer->SetOutFile(Form("Root_files/hcal_calib_%05d.root",RunNumber));
  analyzer->SetOdefFile("output_hcal_replay.def");
  analyzer->SetCutFile("hcal_replay_cuts.def");        // optional
  
  // File to record cuts accounting information
  //  analyzer->SetSummaryFile("summary_example.log"); // optional
  
  analyzer->Process(run);     // start the actual analysis
}
