#ifndef ALIPHOSDigitizer_H
#define ALIPHOSDigitizer_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//_________________________________________________________________________
//  Task Class for making SDigits in PHOS      
// Class performs digitization of Summable digits (in the PHOS case this is just
// sum of contributions of all primary particles into given cell). 
// In addition it performs mixing of summable digits from different events.
//                  
//*-- Author: Dmitri Peressounko(SUBATECH & KI)


// --- ROOT system ---
#include "TObjString.h"
class TArrayI ;
class TClonesArray ; 
class TFile ; 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliDigitizer.h"
class AliPHOSSDigitizer ;
class AliRunDigitizer ;

class AliPHOSDigitizer: public AliDigitizer {

public:
  AliPHOSDigitizer() ;          // ctor
  AliPHOSDigitizer(const TString alirunFileNameFile, 
		   const TString eventFolderName = AliConfig::fgkDefaultEventFolderName) ; 
  AliPHOSDigitizer(const AliPHOSDigitizer & dtizer) ;
  AliPHOSDigitizer(AliRunDigitizer * manager) ;
  virtual ~AliPHOSDigitizer() ;       

  void    Digitize(const Int_t event) ;            // Make Digits from SDigits 
  void    Exec(Option_t *option);                  // Supervising method

  //CPV parameters
  const Float_t GetCPVNoise()     const { return fCPVNoise ;}
  const Float_t GetCPVThreshold() const { return fCPVDigitThreshold ;}
  const Float_t GetCPVchannel()   const { return fADCchanelCpv; }
  const Float_t GetCPVpedestal()  const { return fADCpedestalCpv; }

  void    SetCPVNoise(Float_t CPVNoise)          {fCPVNoise = CPVNoise;}
  void    SetCPVThreshold(Float_t CPVThreshold)  {fCPVDigitThreshold= CPVThreshold;}
  void    SetNCPVchannels(Int_t n)     { fNADCcpv = n; }
  void    SetCPVchannel(Float_t width) { fADCchanelCpv = width; }
  void    SetCPVpedestal(Float_t ped)  { fADCpedestalCpv = ped; }


  //EMC parameters
  const Float_t GetEMCThreshold() const { return fEMCDigitThreshold;}
  const Float_t GetEMCchannel()   const { return fADCchanelEmc; }
  const Float_t GetEMCpedestal()  const { return fADCpedestalEmc; }  
  const Float_t GetPinNoise()     const { return fPinNoise;}
  const Float_t GetTimeResolution() const { return fTimeResolution ; }

  void   SetEMCThreshold(Float_t EMCThreshold)  {fEMCDigitThreshold = EMCThreshold;}
  void   SetPinNoise(Float_t PinNoise )         {fPinNoise = PinNoise;}
  void   SetNEMCchannels(Int_t n)      { fNADCemc = n; }
  void   SetEMCchannel(Float_t width)  { fADCchanelEmc = width; }
  void   SetEMCpedestal(Float_t ped)   { fADCpedestalEmc = ped ; }  
  void   SetTimeResolution(Float_t res){ fTimeResolution = res ; }  

  //General
  const Int_t   GetDigitsInRun()  const { return fDigitsInRun ;}  
  void    MixWith(const TString alirunFileName, 
		  const TString eventFolderName = AliConfig::fgkDefaultEventFolderName) ; // Add another one file to mix
  void    Print(Option_t* option)const ;
 
  AliPHOSDigitizer & operator = (const AliPHOSDigitizer & rvalue)  {
    // assignement operator requested by coding convention but not needed
    Fatal("operator =", "not implemented") ;
    return *this ; 
  }

private:

  virtual Bool_t Init() ; 
  void    InitParameters() ; 
  void    PrintDigits(Option_t * option) ;
  void    WriteDigits(Int_t evt) ;            // Writes Digits for particular event
  Float_t TimeOfNoise(void) const;            // Calculate time signal generated by noise
  //Calculate the time of crossing of the threshold by front edge
  Float_t FrontEdgeTime(TClonesArray * ticks) const ; 
  //Calculate digitized signal with gived ADC parameters
  Int_t   DigitizeEnergy(Float_t energy, Int_t absId) ;

private:

  Bool_t  fDefaultInit;             //! Says if the task was created by defaut ctor (only parameters are initialized)
  Int_t   fDigitsInRun ;            //! Total number of digits in one run
  Bool_t  fInit ;                   //! To avoid overwriting existing files

  Int_t   fInput ;                  // Number of files to merge
  TString * fInputFileNames ;       //[fInput] List of file names to merge
  TString * fEventNames ;           //[fInput] List of event names to merge

  Int_t   fEmcCrystals ;            // Number of EMC crystalls in the given geometry

  Float_t fPinNoise ;               // Electronics noise in EMC
  Float_t fEMCDigitThreshold  ;     // Threshold for storing digits in EMC

  Float_t fCPVNoise ;               // Noise in CPV
  Float_t fCPVDigitThreshold  ;     // Threshold for storing digits in CPV


  Float_t fTimeResolution ;         // Time resolution of FEE electronics
  Float_t fTimeThreshold ;          // Threshold to start timing for given crystall
  Float_t fTimeSignalLength ;       // Length of the timing signal 

  Float_t fADCchanelEmc ;           // width of one ADC channel in GeV
  Float_t fADCpedestalEmc ;         // value of the EMC ADC pedestal
  Int_t   fNADCemc ;                // number of channels in EMC ADC

  Float_t fADCchanelCpv ;           // width of one ADC channel in CPV units
  Float_t fADCpedestalCpv ;         // value of the CPV ADC pedestal in CPV units
  Int_t   fNADCcpv ;                // number of channels in CPV ADC


  ClassDef(AliPHOSDigitizer,1)  // description 

};


#endif // AliPHOSDigitizer_H
