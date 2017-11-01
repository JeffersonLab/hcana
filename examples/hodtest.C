
{

  //
  //  Steering script to test hodoscope decoding
  //

  Int_t RunNumber=50017;
  char RunFileNamePattern[]="daq04_%d.log.0";

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
  sprintf(command,"./make_cratemap.pl < %s > db_cratemap.dat",gHcParms->GetString("g_decode_map_filename"));
  system(command);

  // Load the Hall C style detector map
  gHcDetectorMap=new THcDetectorMap();
  gHcDetectorMap->Load(gHcParms->GetString("g_decode_map_filename"));

  // Set up the equipment to be analyzed.

  THaApparatus* HMS = new THcHallCSpectrometer("H","HMS");
  gHaApps->Add( HMS );

  // Add hodoscope
  THcHodoscope* hms_hodoscope = new THcHodoscope("hod","Hodoscope");
  HMS->AddDetector( hms_hodoscope );
  HMS->AddDetector( new THcShower("cal", "Shower" ));
  HMS->AddDetector( new THcDC("dc", "Drift Chambers" ));
  THcAerogel* aerogel = new THcAerogel("aero", "Aerogel Cerenkov" );
  HMS->AddDetector( aerogel );
  THcCherenkov* cherenkov = new THcCherenkov("cer", "Gas Cerenkov" );
  HMS->AddDetector( cherenkov );

  THcScalerEvtHandler *hscaler = new THcScalerEvtHandler("HS","HC scaler event type 0");
  hscaler->SetDebugFile("HScaler.txt");
  gHaEvtHandlers->Add (hscaler);


  THaApparatus* SOS = new THcHallCSpectrometer("S","SOS");
  gHaApps->Add( SOS );
  // Add detectors
  THcHodoscope* sos_hodoscope = new THcHodoscope("hod","Hodoscope");
  SOS->AddDetector( sos_hodoscope);
  SOS->AddDetector( new THcShower("cal", "Shower" ));
  SOS->AddDetector( new THcDC("dc", "Drift Chambers" ));

  THaApparatus* BEAM = new THcRasteredBeam("B","Rastered Beamline");
  gHaApps->Add( BEAM );

  // setup physics
  gHaPhysics->Add( new THaGoldenTrack( "H.gold", "HMS Golden Track", "H" ));
  gHaPhysics->Add( new THaGoldenTrack( "S.gold", "SOS Golden Track", "S" ));

  gHaPhysics->Add(new THcHodoEff("hhodeff","HMS Hodoscope Efficiencies","H.hod"));
  gHaPhysics->Add(new THcHodoEff("shodeff","SOS Hodoscope Efficiencies","S.hod"));
  THcPeriodicReport* prep = new THcPeriodicReport("rep","Periodic Report","periodic.template", "periodic.out");
  // Defaults to every 2 seconds.  To do by event count instead:
  //  prep->SetEventPeriod(1000);
  //  prep->SetTimePeriod(0);
  //gSystem->Exec("rm periodic.out");
  //gSystem->Exec("./runstats.tcl periodic.out&"); 
  gHaPhysics->Add(prep);

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
  
  THcRun* run = new THcRun(RunFileName);
  run->SetRunParamClass("THcRunParameters");
  // Perhaps we should make a THcRun that does this

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
