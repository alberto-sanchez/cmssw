import FWCore.ParameterSet.Config as cms

process = cms.Process("convertHepMC2LHE")
process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:gen.root')
)

process.load("GeneratorInterface.LHEInterface.HepMC2LHEConverter_cfi")
process.HepMC2LHEConverter.lhe_outputfilename = cms.untracked.string('out.lhe')

process.out = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('out.root'),
    outputCommands = cms.untracked.vstring('drop *','keep *_HepMC2LHEConverter_*_*'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string('p'),
        dataTier = cms.untracked.string('GEN')
    )
)


process.p = cms.Path(process.HepMC2LHEConverter)
process.e = cms.EndPath(process.out)
