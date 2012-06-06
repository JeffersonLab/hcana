
{

  // Load the Hall C style detector map
  gHcDetectorMap=new THcDetectorMap();
  gHcDetectorMap->Load("july04.map");

  gHcParms->Load("PARAM/general.param");
  //
  //  Steering script to test hodoscope decoding
  //
  
  // Set up the equipment to be analyzed.
  
  THaApparatus* HMS = new THcHallCSpectrometer("H","HMS");
  gHaApps->Add( HMS );

  // Add hodoscope
  HMS->AddDetector( new THcHodoscope("hod", "Hodoscope" ));
  HMS->AddDetector( new THcShower("Cal", "Shower" ));
  //  HMS->AddDetector( new THcDriftChamber("dc", "Drift Chambers" ));

  // Set up the analyzer - we use the standard one,
  // but this could be an experiment-specific one as well.
  // The Analyzer controls the reading of the data, executes
  // tests/cuts, loops over Apparatus's and PhysicsModules,
  // and executes the output routines.
  THcAnalyzer* analyzer = new THcAnalyzer;
  

  // A simple event class to be output to the resulting tree.
  // Creating your own descendant of THaEvent is one way of
  // defining and controlling the output.
  THaEvent* event = new THaEvent;
  
  // Define the run(s) that we want to analyze.
  // We just set up one, but this could be many.
  THaRun* run = new THaRun( "daq04_50017.log.0" );
  run->SetEventRange(2000,100000);
  
  // Define the analysis parameters
  analyzer->SetEvent( event );
  analyzer->SetOutFile( "hodtest.root" );
  analyzer->SetOdefFile("output.def");
  //  analyzer->SetCutFile("cuts_example.def");        // optional
  
  // File to record cuts accounting information
  //  analyzer->SetSummaryFile("summary_example.log"); // optional
  
  analyzer->Process(run);     // start the actual analysis
}
