#include "SHarper/HEEPAnalyzer/interface/HEEPAttStatusToPAT.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

HEEPAttStatusToPAT::HEEPAttStatusToPAT(const edm::ParameterSet& iPara):
  cuts_(iPara)
{
  eleLabel_=iPara.getParameter<edm::InputTag>("eleLabel");
  applyRhoCorrToEleIsol_ = iPara.getParameter<bool>("applyRhoCorrToEleIsol");
  eleRhoCorrLabel_=iPara.getParameter<edm::InputTag>("eleRhoCorrLabel");
  produces < pat::ElectronCollection >();
}


void HEEPAttStatusToPAT::produce(edm::Event& iEvent,const edm::EventSetup& iSetup)
{ 
  //load electrons
  edm::Handle<edm::View<pat::Electron> > eleHandle;
  iEvent.getByLabel(eleLabel_,eleHandle);
  const edm::View<pat::Electron>& eles = *(eleHandle.product());

  
  edm::Handle<double> rhoHandle;
  iEvent.getByLabel(eleRhoCorrLabel_,rhoHandle);
  double rho = rhoHandle.isValid() ? *rhoHandle : 0;
  

  //prepare output collection
  std::auto_ptr<pat::ElectronCollection> outEle(new pat::ElectronCollection());

  //clone electrons, reset energy to the ecal energy and add info
  for(size_t eleNr=0;eleNr<eles.size();eleNr++){
    pat::Electron * newEle = eles[eleNr].clone();
    newEle->setP4(newEle->p4()/newEle->energy()*newEle->ecalEnergy());
    if(applyRhoCorrToEleIsol_) newEle->addUserInt("HEEPId",cuts_.getCutCode(rho,eles[eleNr]));
    else newEle->addUserInt("HEEPId",cuts_.getCutCode(eles[eleNr]));
    outEle->push_back(*(newEle));
    delete newEle;
  }

  //store the output
  iEvent.put(outEle);
}



//define this as a plug-in
DEFINE_FWK_MODULE(HEEPAttStatusToPAT);
