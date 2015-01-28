
{

  //
  //  Steering script to test hodoscope decoding
  //
  
  Int_t RunNumber=50017;
  char* RunFileNamePattern="daq04_%d.log.0";
  
  // Load parameters from CCDB

  gHcParms->OpenCCDB(RunNumber);
  gHcParms->LoadCCDBDirectory("hms","h");
  gHcParms->LoadCCDBDirectory("sos","s");
  gHcParms->LoadCCDBDirectory("gen","g");

  // Get these legacy things from the Run database
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  gHcParms->AddString("g_ctp_database_filename", "DBASE/test.database");
  gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);


  // Generate db_cratemap to correspond to map file contents
  char command[100];
  sprintf(command,"./make_cratemap.pl < %s > db_cratemap.dat",gHcParms->GetString("g_decode_map_filename"));
  system(command);

  // Load the Hall C style detector map
  gHcDetectorMap=new THcDetectorMap();
  gHcDetectorMap->Load(gHcParms->GetString("g_decode_map_filename"));

  // Set up the equipment to be analyzed.

  THaApparatus* HMS = new THcHallCSpectrometer("H","HMS");
  gHaApps->Add( HMS );

  // Add hodoscope
  HMS->AddDetector( new THcHodoscope("hod", "Hodoscope" ));
  HMS->AddDetector( new THcShower("cal", "Shower" ));
  HMS->AddDetector( new THcDC("dc", "Drift Chambers" ));
  THcAerogel* aerogel = new THcAerogel("aero", "Aerogel Cerenkov" );
  HMS->AddDetector( aerogel );
  THcCherenkov* cherenkov = new THcCherenkov("cher", "Gas Cerenkov" );
  HMS->AddDetector( cherenkov );

  THaApparatus* SOS = new THcHallCSpectrometer("S","SOS");
  gHaApps->Add( SOS );
  // Add detectors
  SOS->AddDetector( new THcHodoscope("hod", "Hodoscope" ));
  SOS->AddDetector( new THcShower("cal", "Shower" ));
  SOS->AddDetector( new THcDC("dc", "Drift Chambers" ));

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
  char RunFileName[100];
  sprintf(RunFileName,RunFileNamePattern,RunNumber);
  THaRun* run = new THaRun(RunFileName);

  // Eventually need to learn to skip over, or properly analyze
  // the pedestal events
  run->SetEventRange(1,100000);//  Physics Event number, does not
                                // include scaler or control events

  // Define the analysis parameters
  analyzer->SetEvent( event );
  analyzer->SetOutFile( "hodtest.root" );
  analyzer->SetOdefFile("output.def");
  analyzer->SetCutFile("hodtest_cuts.def");        // optional
  analyzer->SetCountMode(2);// Counter event number same as gen_event_ID_number
  
  // File to record cuts accounting information
  //  analyzer->SetSummaryFile("summary_example.log"); // optional
  
  analyzer->Process(run);     // start the actual analysis
  analyzer->PrintReport("report.template","report.out");
}
