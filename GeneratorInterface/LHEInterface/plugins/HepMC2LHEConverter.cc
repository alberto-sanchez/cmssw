#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
//#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LesHouches.h"

class HepMC2LHEConverter : public edm::one::EDProducer<edm::one::WatchRuns,edm::BeginRunProducer> {
 public:
  HepMC2LHEConverter(const edm::ParameterSet&);
  virtual ~HepMC2LHEConverter();
  virtual void beginRun(const edm::Run &, const edm::EventSetup&) override;
  virtual void beginRunProduce(edm::Run &, edm::EventSetup const&) override;
  virtual void endRun(edm::Run const&, const edm::EventSetup&) override;
//  virtual void endRunProduce(edm::Run&, const edm::EventSetup&) override;
  virtual void produce(edm::Event&, const edm::EventSetup&) override;
 // static void fillDescriptions(edm::ConfigurationDescriptions&);
 private:

  edm::InputTag _fSourceTag;
  lhef::HEPRUP *heprup_;
  int ii;
  bool newRun;
};

HepMC2LHEConverter::HepMC2LHEConverter(const edm::ParameterSet &p) {    

    //... get where is input data
    _fSourceTag = p.getParameter<edm::InputTag>("HepMCProduct");

    //... declare output
    produces<LHERunInfoProduct, edm::InRun>();
    produces<LHEEventProduct>();
    ii = 0;
    newRun = false;
}

HepMC2LHEConverter::~HepMC2LHEConverter() {}

void HepMC2LHEConverter::beginRun(const edm::Run & run, const edm::EventSetup& es ) {
	/*
  //... get the event -- infrmation about beam
  edm::Handle<edm::HepMCProduct> HepMCevent;
  run.getByLabel(_fSourceTag,HepMCevent);

  HepMC::GenEvent *evt = new HepMC::GenEvent(*HepMCevent->GetEvent());
  heprup_ = new lhef::HEPRUP();
  heprup_->NPRUP = 1;     // should only be one process
  int ibeam = 1;
  for (HepMC::GenEvent::particle_const_iterator p = evt->particles_begin(); p != evt->particles_end(); ++p) {
     if ((*p)->status() == 4) {
        if (ibeam == 1) { 
           heprup_->IDBMUP.first = (*p)->pdg_id();
           heprup_->EBMUP.first = (*p)->momentum().e();
        } else {
           heprup_->IDBMUP.second = (*p)->pdg_id();
           heprup_->EBMUP.second = (*p)->momentum().e();
        }   
        ibeam++;
     }
     if (ibeam > 2) break;
  }
  //std::auto_ptr<LHERunInfoProduct> lherun(new LHERunInfoProduct(*heprup_));
  //run.put(lherun);  
  //
  */
  if (ii==0) heprup_ = new lhef::HEPRUP();
  newRun = true;
  std::cout << "beginRun " << ii <<std::endl;
}

void HepMC2LHEConverter::beginRunProduce(edm::Run & run, edm::EventSetup const & es ) {
  if (ii == 0) {
    heprup_->NPRUP = 1;
    heprup_->IDBMUP.first = heprup_->IDBMUP.second = 2212;
    heprup_->EBMUP.first = heprup_->IDBMUP.second = 6500;
  }
  std::auto_ptr<LHERunInfoProduct> lherun(new LHERunInfoProduct(*heprup_));
  run.put(lherun);
  lherun.reset();
  std::cout << "beginRunproduce " << ii << std::endl;
}

void HepMC2LHEConverter::endRun(const edm::Run &run, const edm::EventSetup& es ) {
  std::cout << "endRun " << ii << std::endl;
}

//void HepMC2LHEConverter::endRunProduce(edm::Run &run, const edm::EventSetup& es ) {
 // std::auto_ptr<LHERunInfoProduct> lherun(new LHERunInfoProduct(*heprup_));
 // run.put(lherun);
//}

void HepMC2LHEConverter::produce(edm::Event & iEvent, const edm::EventSetup & es) {

   //... get the event
   edm::Handle<edm::HepMCProduct> HepMCevent;
   iEvent.getByLabel(_fSourceTag,HepMCevent);
 
   //... translate to output
   HepMC::GenEvent *evt = new HepMC::GenEvent(*HepMCevent->GetEvent());
   lhef::HEPEUP hepeup_ = lhef::HEPEUP();
   //heprup_ = new lhef::HEPRUP();
   int nup_ = 0;
   int ibeam = 1;
   heprup_->NPRUP = 1;
   for (HepMC::GenEvent::particle_const_iterator p = evt->particles_begin(); p != evt->particles_end(); ++p) {
      if (newRun && ibeam<=2 && (*p)->status() == 4) {
        if (ibeam == 1) {
           heprup_->IDBMUP.first = (*p)->pdg_id();
           heprup_->EBMUP.first = (*p)->momentum().e();
        } else {
           heprup_->IDBMUP.second = (*p)->pdg_id();
           heprup_->EBMUP.second = (*p)->momentum().e();
        }
        ibeam++;
      } 
      if ((*p)->status() != 1) continue;
      if (ii == 0) std::cout << (*p)->status()  << " " << (*p)->pdg_id() << " " << (*p)->momentum().px() << " " << (*p)->momentum().py() << " " << (*p)->momentum().pz() << " " << (*p)->momentum().e() << " " << (*p)->generated_mass()  << std::endl;
      nup_++;
      hepeup_.IDUP.push_back((*p)->pdg_id());
      hepeup_.ISTUP.push_back((*p)->status());
      lhef::HEPEUP::FiveVector pup_ = {{(*p)->momentum().px(),(*p)->momentum().py(),(*p)->momentum().pz(),(*p)->momentum().e(),(*p)->generated_mass()}};
      hepeup_.PUP.push_back(pup_);     
   }
   hepeup_.NUP = nup_;
   std::cout << nup_ << " " << hepeup_.IDUP.size() << std::endl;
   newRun = false;
   //... store output
   std::auto_ptr<LHEEventProduct> lheevt(new LHEEventProduct(hepeup_));
   iEvent.put(lheevt);
   ii ++;
}

/*
void HepMC2LHEConverter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}*/

//}

DEFINE_FWK_MODULE(HepMC2LHEConverter);
