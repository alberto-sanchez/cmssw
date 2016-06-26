#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Provenance/interface/EventID.h"

#include "GeneratorInterface/ExternalDecays/interface/ExternalDecayDriver.h"

#include <CLHEP/Random/RandomEngine.h>
#include <CLHEP/Random/JamesRandom.h>

#include "ExternalDecaysStandalone.h"
#include "HepMC/GenEvent.h"

#include <iostream>
#include <memory>
#include <vector>

ExternalDecaysStandalone::ExternalDecaysStandalone(edm::ParameterSet const & p) {    

    //... get where is input data
    fSourceToken = consumes<edm::HepMCProduct>(edm::InputTag(p.getUntrackedParameter("moduleLabel",std::string("generator")),"unsmeared"));

    //... create the decayer, configure by user
    edm::ParameterSet decayer_params = p.getParameter<edm::ParameterSet>("ExternalDecays");
    fDecayer = new gen::ExternalDecayDriver(decayer_params);

    edm::Service<edm::RandomNumberGenerator> rng;
    if ( !rng.isAvailable() )
      throw cms::Exception("Configuration")
        << "The ExternalDecaysStandalone module requires the RandomNumberGeneratorService\n"
        << "which appears to be absent. Please add that service to your configuration\n"
        << "or remove the modules that require it.\n";

    //CLHEP::HepRandomEngine *engine = new CLHEP::HepJamesRandom;
    //engine->setSeed(123456789, 0);
    //fDecayer->setRandomEngine(engine);

    //... declare output
    produces<edm::HepMCProduct>("unsmeared");

}

ExternalDecaysStandalone::~ExternalDecaysStandalone() {}

void ExternalDecaysStandalone::beginRun(edm::Run const& run, const edm::EventSetup & es) {
   //... Should be initialized only after setting Random Engine 
   //fDecayer->init(es);
}

void ExternalDecaysStandalone::produce(edm::Event & iEvent, const edm::EventSetup & es) {

   //... set Random Engine. To be done for each event due to multithreading (?)
   edm::Service<edm::RandomNumberGenerator> rng;
/*   if ( !rng.isAvailable() )
     throw cms::Exception("Configuration")
       << "The ExternalDecaysStandalone module requires the RandomNumberGeneratorService\n"
       << "which appears to be absent. Please add that service to your configuration\n"
       << "or remove the modules that require it.\n";*/

   CLHEP::HepRandomEngine *decayRandomEngine = &rng->getEngine(iEvent.streamID());
   fDecayer->setRandomEngine(decayRandomEngine);

   //... we should initialize the external decayer. It is protected against multiple init calls
   fDecayer->init(es);

   //... get the event
   edm::Handle<edm::HepMCProduct> HepMCUndecayed;
   iEvent.getByToken(fSourceToken,HepMCUndecayed);
 
   //... call decayer
   HepMC::GenEvent *evtUndecayed = new HepMC::GenEvent(*HepMCUndecayed->GetEvent());
   HepMC::GenEvent *evtDecayed   = fDecayer->decay(evtUndecayed);
 
   //... store output
   std::unique_ptr<edm::HepMCProduct> pevt(new edm::HepMCProduct(evtDecayed));
   iEvent.put(std::move(pevt),"unsmeared");

}

DEFINE_FWK_MODULE(ExternalDecaysStandalone);
