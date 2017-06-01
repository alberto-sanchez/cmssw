import FWCore.ParameterSet.Config as cms

HepMC2LHEConverter = cms.EDProducer("HepMC2LHEConverter",
  HepMCProduct = cms.InputTag("generator")
)
