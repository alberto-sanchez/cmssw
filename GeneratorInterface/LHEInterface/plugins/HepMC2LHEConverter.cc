#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"

//#include "HepMC/GenEvent.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"

#include "SimDataFormats/GeneratorProducts/interface/LesHouches.h"

class HepMC2LHEConverter : public edm::EDProducer {

 public:
  explicit HepMC2LHEConverter(const edm::ParameterSet&);
  virtual ~HepMC2LHEConverter();
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

 private:

  virtual void produce(edm::Event&, const edm::EventSetup&) override;
  virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;

  edm::InputTag _fSourceTag;
  int ii;
};


HepMC2LHEConverter::HepMC2LHEConverter(const edm::ParameterSet &p) {    

    //... get where is input data
    _fSourceTag = p.getParameter<edm::InputTag>("HepMCProduct");

    //... declare output
    //produces<LHERunInfoProduct, edm::InRun>();
    produces<LHEEventProduct>();
    ii = 0;
}

HepMC2LHEConverter::~HepMC2LHEConverter() {}

void HepMC2LHEConverter::beginRun(edm::Run const& run, const edm::EventSetup & es) {
}

void HepMC2LHEConverter::produce(edm::Event & iEvent, const edm::EventSetup & es) {

   //... get the event
   edm::Handle<edm::HepMCProduct> HepMCevent;
   iEvent.getByLabel(_fSourceTag,HepMCevent);
 
   //... call decayer
   HepMC::GenEvent *evt = new HepMC::GenEvent(*HepMCevent->GetEvent());
   lhef::HEPEUP hepeup_ = lhef::HEPEUP();
   int nup_ = 0;
   for (HepMC::GenEvent::particle_const_iterator p = evt->particles_begin(); p != evt->particles_end(); ++p) {
      if ((*p)->status() != 1 && (*p)->status() != 4) continue;
      if (ii == 0) std::cout << (*p)->status()  << " " << (*p)->pdg_id() << " " << (*p)->momentum().px() << " " << (*p)->momentum().py() << " " << (*p)->momentum().pz() << " " << (*p)->momentum().e() << " " << (*p)->generated_mass()  << std::endl;
      nup_++;
      hepeup_.IDUP.push_back((*p)->pdg_id());
      hepeup_.ISTUP.push_back((*p)->status());
      lhef::HEPEUP::FiveVector pup_ = {{(*p)->momentum().px(),(*p)->momentum().py(),(*p)->momentum().pz(),(*p)->momentum().e(),(*p)->generated_mass()}};
      hepeup_.PUP.push_back(pup_);     
   }
   hepeup_.NUP = nup_;
   std::cout << nup_ << " " << hepeup_.IDUP.size() << std::endl;

   //... store output
 //  std::auto_ptr<LHERunInfoProduct> lherun(new LHERunInfoProduct());
   std::auto_ptr<LHEEventProduct> lheevt(new LHEEventProduct(hepeup_));
 //  iEvent.put(lherun);
   iEvent.put(lheevt);
   ii ++;
}

void HepMC2LHEConverter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(HepMC2LHEConverter);
