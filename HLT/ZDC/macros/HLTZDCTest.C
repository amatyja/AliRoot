// $Id: HLTZDCTest.C 43204 2010-08-30 07:47:06Z richterm $
void HLTZDCTest();
/**
 * @file HLTZDCTest.C
 * @brief Example macro to run the AliHLTZDCESDRecoComponent in 
 * AliReconstruction.
 *
 * The component subscribes to DDL raw data published by the
 * AliHLTRawReaderPublisherComponent. The macros requires a raw data file
 * and a corresponding GRP entry.
 *
 * <pre>
 * Usage: aliroot -b -q -l \
 *     HLTZDCTest.C'("rawfile", "cdb", minEvent, maxEvent)'
 *
 * Examples:
 *     HLTZDCTest.C'("raw.root", minEvent, MaxEvent)'
 *     HLTZDCTest.C'("./", minEvent, MaxEvent)'
 *     HLTZDCTest.C'("alien:///alice/data/2010/.../raw/....root")' 
 *
 * Defaults
 *     cdb="local://$ALICE_ROOT/OCDB" -> take local OCDB from distribution
 *     minEvent=-1   -> no lower event selection
 *     maxEvent=-1   -> no upper event selection
 *
 * </pre>
 *
 * The input file can be a local raw.root file but also a file from the
 * GRID. The separate DDL files generated in simulation can be accessed
 * using AliRawReaderFile by speficying "directory/".
 *
 * Since the macro runs AliReconstruction the OCDB needs to be set up, in
 * particular the GRP entry. If testing with a local OCDB you have to
 * simulate some events and run the macro in the folder of the simulation.
 * Also HLT components configure from objects in the OCDB.
 *
 * Note: You need a valid GRID token, if you want to access files directly
 * from the Grid, use 'alien-token-init' of your alien installation.
 *
 * @author Chiara Oppedisano <Chiara.Oppedisano@to.infn.it>
 *         Jochen Thaeder <jochen@thaeder.de>
 * @ingroup alihlt_zdc
 */

// ---------------------------------------------------------------------------- 
void HLTZDCTest(const Char_t *filename, const Char_t *cdbURI,
		  Int_t minEvent=-1, Int_t maxEvent=-1) {
  
  printf (" ============================================= \n\n");
  printf ("        TEST ZDC RECONSTRUCTION              \n\n");
  printf (" ============================================= \n");

  if(!gSystem->AccessPathName("galice.root")){
    cerr << "AliReconstruction on raw data requires to delete galice.root, ";
    cerr << "or run at different place." << endl;
    cerr << "!!! DO NOT DELETE the galice.root of your simulation, ";
    cerr << "but create a subfolder !!!!" << endl;
    return;
  }

  if (gSystem->AccessPathName(filename)) {
    cerr << "can not find file " << filename << endl;
    return;
  }

  // -- connect to the GRID if we use a file or OCDB from the GRID
  TString struri=cdbURI;
  TString strfile=filename;
  if (struri.BeginsWith("raw://") ||
      strfile.Contains("://") && !strfile.Contains("local://")) {
    TGrid::Connect("alien");
  }

  // -- Set the CDB storage location
  AliCDBManager * man = AliCDBManager::Instance();
  man->SetDefaultStorage(cdbURI);
  if (struri.BeginsWith("local://")) {
    // set specific storage for GRP entry
    // search in the working directory and one level above, the latter
    // follows the standard simulation setup like e.g. in test/ppbench
    if (!gSystem->AccessPathName("GRP/GRP/Data")) {
      man->SetSpecificStorage("GRP/GRP/Data", "local://$PWD");
    } else if (!gSystem->AccessPathName("../GRP/GRP/Data")) {
      man->SetSpecificStorage("GRP/GRP/Data", "local://$PWD/..");      
    } else {
      cerr << "can not find a GRP entry, please run the macro in the folder" << endl;
      cerr << "of a simulated data sample, or specify a GRID OCDB" << endl;
      HLTZDCTest();      
      return;
    }
  }

  //////////////////////////////////////////////////////////////////////////////////////
  //
  // Reconstruction settings
  AliReconstruction rec;

  if (minEvent>=0 || maxEvent>minEvent) {
    if (minEvent<0) minEvent=0;
    if (maxEvent<minEvent) maxEvent=minEvent;
    rec.SetEventRange(minEvent,maxEvent);
  }

  rec.SetRunReconstruction("HLT");
  rec.SetLoadAlignFromCDB(kFALSE);
  rec.SetWriteESDfriend(kFALSE);

  // due to bug ...
  // StopOnError needs to be disabled
  rec.SetStopOnError(kFALSE);
  rec.SetRunVertexFinder(kFALSE);
  rec.SetInput(filename);

  // QA options
  rec.SetRunQA(":") ;
  //rec.SetQARefDefaultStorage("local://$ALICE_ROOT/QAref") ;

  //////////////////////////////////////////////////////////////////////////////////////
  //
  // setup the HLT system
  AliHLTSystem* pHLT=AliHLTPluginBase::GetInstance();

  // define a data publisher configuration for ZDC raw data
  AliHLTConfiguration publisher("RAW-Publisher", 
				"AliRawReaderPublisher", 
				"", 
				"-equipmentid 3840 "
				"-datatype 'DDL_RAW ' 'ZDC ' "
				"-dataspec 0x01"
				);

  // define configuration of the ZDCReconstruction component
  AliHLTConfiguration vzeroReco("ZDC-Reconstruction", 
				"ZDCESDReco", 
				"RAW-Publisher",
				""
				);

  // define configuration of the GlobalEsdConverter component
  AliHLTConfiguration esdConverter("GLOBAL-ESD-Converter",
				   "GlobalEsdConverter", 
				   "ZDC-Reconstruction",
				   ""
				   );

  // define configuration for Root file writer of ZDC output
  AliHLTConfiguration rootWriter("RootWriter", 
				 "ROOTFileWriter",
				 "ZDC-Reconstruction GLOBAL-ESD-Converter",
				 "-directory analysis -datafile zdcRec"
				 );



  // set option for the HLT module in AliReconstruction
  // arguments
  //  - ignore-hltout : ignore the HLTOUT payload from the HLT DDLs
  //  - libraries to be used as plugins
  //  - loglevel=0x7c : Important, Info, Warning, Error, Fatal
  rec.SetOption("HLT",
		"ignore-hltout " 
		"libAliHLTUtil.so libAliHLTGlobal.so libAliHLTZDC.so "
		"loglevel=0x7c "
		"chains=GLOBAL-ESD-Converter,RootWriter"
		);

  rec.SetRunPlaneEff(kFALSE);

  // switch off cleanESD
  rec.SetCleanESD(kFALSE);

  AliLog::Flush();
  rec.Run();
}

// ---------------------------------------------------------------------------- 
void HLTZDCTest(const Char_t *filename, Int_t minEvent=-1, Int_t maxEvent=-1){
  HLTZDCTest(filename, "local://$ALICE_ROOT/OCDB", minEvent, maxEvent);
}

// ---------------------------------------------------------------------------- 
void HLTZDCTest() {
  cout << "HLTZDCTest: Run HLT component 'ZDCReconstruction' in AliReconstruction" << endl;
  cout << " Usage: aliroot -b -q -l \\" << endl;
  cout << "     HLTZDCTest.C'(\"file\", \"cdb\", minEvent, maxEvent)'" << endl;
  cout << "" << endl;
  cout << " Examples:" << endl;
  cout << "     HLTZDCTest.C'(\"raw.root\", minEvent, MaxEvent)'" << endl;
  cout << "     HLTZDCTest.C'(\"./\", minEvent, MaxEvent)'" << endl;
  cout << "     HLTZDCTest.C'(\"alien:///alice/data/2010/.../raw/....root\")' " << endl;
  cout << "" << endl;
  cout << " Defaults" << endl;
  cout << "     cdb=\"local://$ALICE_ROOT/OCDB\"  -> take local OCDB" << endl;
  cout << "     minEvent=-1   -> no lower event selection" << endl;
  cout << "     maxEvent=-1   -> no upper event selection" << endl;
}
