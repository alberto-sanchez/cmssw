import FWCore.ParameterSet.Config as cms

HepMC2LHEConverter = cms.EDProducer("HepMC2LHEConverter",
  HepMCProduct = cms.InputTag("generator"),
  lhe_outputfilename = cms.untracked.string('')   #if empty no ascii file is produced
)
