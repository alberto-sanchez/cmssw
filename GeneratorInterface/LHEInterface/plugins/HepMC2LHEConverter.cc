#include <memory>
#include <fstream>
#include <string>

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
  LHERunInfoProduct::Header custom_slha();

  edm::InputTag _fSourceTag;
  lhef::HEPRUP *heprup_;
  int ii;
  bool newRun;
  std::ofstream file;
  bool write_lheout;
};

HepMC2LHEConverter::HepMC2LHEConverter(const edm::ParameterSet &p) {    

    //... get where is input data
    _fSourceTag = p.getParameter<edm::InputTag>("HepMCProduct");

    //... declare output
    produces<LHERunInfoProduct, edm::InRun>();
    produces<LHEEventProduct>();
    ii = 0;
    newRun = false;
    write_lheout=false;
    std::string lhe_ouputfile = "test.lhe";
    if (lhe_ouputfile !=""){
     write_lheout=true;
     file.open(lhe_ouputfile, std::fstream::out | std::fstream::trunc);
    }
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
    heprup_->resize(1);
    heprup_->NPRUP = 1;
    /*
    heprup_->IDBMUP.first = 2212
    heprup_->IDBMUP.second = 2212;
    heprup_->EBMUP.first = 6500;
    heprup_->IDBMUP.second = 6500;*/
    heprup_->IDWTUP = 3;
    heprup_->XSECUP[0] = 1.;
    heprup_->XERRUP[0] = 0;
    heprup_->XMAXUP[0] = 1;
    heprup_->LPRUP[0]= 1;
  }
  std::auto_ptr<LHERunInfoProduct> lherun(new LHERunInfoProduct(*heprup_));
  lherun->addHeader(custom_slha());
  if (write_lheout) std::copy(lherun->begin(), lherun->end(), std::ostream_iterator<std::string>(file));
  run.put(lherun);
  lherun.reset();
  std::cout << "beginRunproduce " << ii << std::endl;
}

void HepMC2LHEConverter::endRun(const edm::Run &run, const edm::EventSetup& es ) {
  if (write_lheout) {
      file << LHERunInfoProduct::endOfFile();
      file.close();
  }
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
   lhef::HEPEUP hepeup_;
   int nup_ = 0;
   int ibeam = 1;
   hepeup_.IDPRUP = 0;
   hepeup_.XWGTUP = 1;
   hepeup_.SCALUP = -1;
   hepeup_.AQEDUP = -1;
   hepeup_.AQCDUP = -1;

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
      int particleindex = hepeup_.NUP;
      hepeup_.resize(hepeup_.NUP+1);
      hepeup_.PUP[particleindex][0] = (*p)->momentum().px();
      hepeup_.PUP[particleindex][1] = (*p)->momentum().py();
      hepeup_.PUP[particleindex][2] = (*p)->momentum().pz();
      hepeup_.PUP[particleindex][3] = (*p)->momentum().e();
      hepeup_.PUP[particleindex][4] = (*p)->generated_mass();
      hepeup_.IDUP[particleindex]   = (*p)->pdg_id();
      hepeup_.SPINUP[particleindex] = 0;
      hepeup_.VTIMUP[particleindex] = 0.;
      hepeup_.ICOLUP[particleindex].first = 0;
      hepeup_.ICOLUP[particleindex].second = 0;
 
      /*hepeup_.IDUP.push_back((*p)->pdg_id());
      hepeup_.ISTUP.push_back((*p)->status());
      lhef::HEPEUP::FiveVector pup_ = {{(*p)->momentum().px(),(*p)->momentum().py(),(*p)->momentum().pz(),(*p)->momentum().e(),(*p)->generated_mass()}};
      hepeup_.PUP.push_back(pup_);     */
   }
   //hepeup_.NUP = nup_;
   //std::cout << nup_ << " " << hepeup_.IDUP.size() << std::endl;
   newRun = false;
   //... store output
   std::auto_ptr<LHEEventProduct> lheevt(new LHEEventProduct(hepeup_,1.));
   if (write_lheout) std::copy(lheevt->begin(), lheevt->end(), std::ostream_iterator<std::string>(file));
   iEvent.put(lheevt);
   ii ++;
}

LHERunInfoProduct::Header HepMC2LHEConverter::custom_slha() {
   LHERunInfoProduct::Header slhah("slha");
   slhah.addLine("\n");
   slhah.addLine("######################################################################\n");
   slhah.addLine("## Custom HepMC to LHE Converter                                  ####\n");
   slhah.addLine("######################################################################\n");
   return slhah;
}

/*
void HepMC2LHEConverter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}*/

//}

DEFINE_FWK_MODULE(HepMC2LHEConverter);
