#ifndef GeneratorInterface_Pythia8Interface_ExternalDecaysStandalone_H
#define GeneratorInterface_Pythia8Interface_ExternalDecaysStandalone_H

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"

class ParameterSet;
class Event;
class EventSetup;

namespace gen {
  class ExternalDecayDriver;
}

class ExternalDecaysStandalone : public edm::stream::EDProducer <>
{

 public:

  explicit ExternalDecaysStandalone(edm::ParameterSet const & p);
  virtual ~ExternalDecaysStandalone();
  virtual void beginRun(edm::Run const& run, const edm::EventSetup & es) override;
  virtual void produce(edm::Event & e, const edm::EventSetup & c) override;

 private:

  gen::ExternalDecayDriver *fDecayer;
  edm::EDGetTokenT<edm::HepMCProduct> fSourceToken;

};

#endif
