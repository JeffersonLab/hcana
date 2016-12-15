
{

  //
  //  Steering script to test raster signal decoding
  //

  Int_t RunNumber=52947;
  char* RunFileNamePattern="/cache/mss/hallc/daq04/raw/daq04_52947.log.0";


  // Open the database
  //
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);
  gHcParms->AddString("g_ctp_database_filename", "DBASE/raster_test.database");
  gHcParms->Load(gHcParms->GetString("g_ctp_database_filename"), RunNumber);


  // Open and load parameter files
  //
  gHcParms->Load(gHcParms->GetString("g_ctp_kinematics_filename"), RunNumber);
  gHcParms->Load(gHcParms->GetString("g_ctp_parm_filename"));
  // parameters not found in usual engine parameter files
  gHcParms->Load("PARAM/hcana.param");


  //  Generate db_cratemap to correspond to map file contents
  //  make_cratemap.pl scripts reads a Hall C style MAP file and output a
  //  Hall A style crate map DB file
  //
  char command[100];
  sprintf(command,"./make_cratemap.pl < %s > db_cratemap.dat",gHcParms->GetString("g_decode_map_filename"));
  system(command);


  // Load the Hall C style detector map
  //
  gHcDetectorMap=new THcDetectorMap();
  gHcDetectorMap->Load(gHcParms->GetString("g_decode_map_filename"));


  // Set up the equipment to be analyzed.
  //
  // HMS and its detectors
  THaApparatus* HMS = new THcHallCSpectrometer("H","HMS");
  gHaApps->Add( HMS );
  // Add hodoscope
  HMS->AddDetector( new THcHodoscope("hod", "Hodoscope" ));
  HMS->AddDetector( new THcShower("cal", "Shower" ));
  HMS->AddDetector( new THcDC("dc", "Drift Chambers" ));
  HMS->AddDetector( new THcAerogel("aero", "Aerogel Cerenkov" ));
  HMS->AddDetector( new THcCherenkov("cher", "Gas Cerenkov" ));


  // Beamline and its detectors
  THaApparatus * BEAM = new THcRasteredBeam("RB","Rastered Beamline");
  gHaApps->Add( BEAM );


  // Set up the analyzer - we use the standard one,
  // but this could be an experiment-specific one as well.
  // The Analyzer controls the reading of the data, executes
  // tests/cuts, loops over Acpparatus's and PhysicsModules,
  // and executes the output routines.
  //
  THcAnalyzer* analyzer = new THcAnalyzer;


  // A simple event class to be output to the resulting tree.
  // Creating your own descendant of THaEvent is one way of
  // defining and controlling the output.
  //
  THaEvent* event = new THaEvent;


  // Define the run(s) that we want to analyze.
  // We just set up one, but this could be many.
  //
  char RunFileName[100];
  sprintf(RunFileName,RunFileNamePattern,RunNumber);
  THaRun* run = new THaRun(RunFileName);


  // Eventually need to learn to skip over, or properly analyze
  // the pedestal events
  //
  run->SetEventRange(1,2000);//  Physics Event number, does not
                           // include scaler or control events

  // Define the analysis parameters
  //
  analyzer->SetEvent(event);
  analyzer->SetOutFile("raster_compare_52947.root");
  analyzer->SetOdefFile("output_bpw.def");
  analyzer->SetCutFile("hodtest_cuts.def");        // optional
  analyzer->SetCountMode(2);// Counter event number same as gen_event_ID_number

  // File to record cuts accounting information
  //  analyzer->SetSummaryFile("summary_example.log"); // optional

  // start the actual analysis
  //
  analyzer->Process(run);
  analyzer->PrintReport("report.template","report.out");
}
