#pragma once

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <filesystem>

#include <mohses/net.h>

#include <mohses/BaseLogger.h>

// Boost dependencies
#include <boost/algorithm/string.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/exception/all.hpp>


// BioGears core
#include <biogears/cdm/CommonDataModel.h>
#include <biogears/cdm/scenario/SEActionManager.h>
#include <biogears/cdm/scenario/SEPatientActionCollection.h>
#include <biogears/engine/BioGearsPhysiologyEngine.h>
#include <biogears/engine/Controller/BioGears.h>
#include <biogears/engine/Controller/BioGearsEngine.h>

#include <biogears/cdm/compartment/SECompartmentManager.h>
#include <biogears/cdm/compartment/fluid/SEGasCompartment.h>
#include <biogears/cdm/compartment/fluid/SELiquidCompartment.h>
#include <biogears/cdm/patient/SEPatient.h>
#include <biogears/cdm/system/physiology/SEBloodChemistrySystem.h>
#include <biogears/cdm/system/physiology/SECardiovascularSystem.h>
#include <biogears/cdm/system/physiology/SEEnergySystem.h>
#include <biogears/cdm/system/physiology/SERespiratorySystem.h>

#include <biogears/cdm/system/physiology/SERenalSystem.h>

#include <biogears/cdm/Serializer.h>
#include <biogears/cdm/engine/PhysiologyEngineTrack.h>
#include <biogears/cdm/properties/SEScalarTime.h>
#include <biogears/cdm/scenario/SEAdvanceTime.h>
#include <biogears/cdm/scenario/SEScenario.h>
#include <biogears/cdm/scenario/SEScenarioInitialParameters.h>

#include <biogears/cdm/compartment/SECompartmentManager.h>
#include <biogears/cdm/engine/PhysiologyEngineTrack.h>
#include <biogears/cdm/patient/actions/SEAirwayObstruction.h>
#include <biogears/cdm/patient/actions/SEAsthmaAttack.h>
#include <biogears/cdm/patient/actions/SEBrainInjury.h>
#include <biogears/cdm/patient/actions/SEHemorrhage.h>
#include <biogears/cdm/patient/actions/SEPainStimulus.h>
#include <biogears/cdm/patient/actions/SEPupillaryResponse.h>
#include <biogears/cdm/patient/actions/SESubstanceBolus.h>

#include <biogears/cdm/substance/SESubstanceManager.h>
#include <biogears/cdm/system/physiology/SEBloodChemistrySystem.h>
#include <biogears/cdm/system/physiology/SECardiovascularSystem.h>
#include <biogears/cdm/system/physiology/SEDrugSystem.h>
#include <biogears/cdm/system/physiology/SENervousSystem.h>

#include <biogears/cdm/substance/SESubstance.h>
#include <biogears/cdm/substance/SESubstanceManager.h>

#include <biogears/cdm/engine/PhysiologyEngineTrack.h>
#include <biogears/cdm/scenario/SEScenarioExec.h>
#include <biogears/cdm/utils/SEEventHandler.h>

#include <biogears/cdm/properties/SEFunctionVolumeVsTime.h>
#include <biogears/cdm/properties/SEScalarAmountPerVolume.h>
#include <biogears/cdm/properties/SEScalarFlowResistance.h>
#include <biogears/cdm/properties/SEScalarFraction.h>
#include <biogears/cdm/properties/SEScalarFrequency.h>
#include <biogears/cdm/properties/SEScalarLength.h>
#include <biogears/cdm/properties/SEScalarMass.h>
#include <biogears/cdm/properties/SEScalarMassPerVolume.h>
#include <biogears/cdm/properties/SEScalarOsmolality.h>
#include <biogears/cdm/properties/SEScalarOsmolarity.h>
#include <biogears/cdm/properties/SEScalarPressure.h>
#include <biogears/cdm/properties/SEScalarTemperature.h>
#include <biogears/cdm/properties/SEScalarTime.h>
#include <biogears/cdm/properties/SEScalarTypes.h>
#include <biogears/cdm/properties/SEScalarVolume.h>
#include <biogears/cdm/properties/SEScalarVolumePerTime.h>

#include <biogears/cdm/patient/actions/SEPainStimulus.h>
#include <biogears/cdm/patient/actions/SESubstanceBolus.h>
#include <biogears/cdm/patient/actions/SESubstanceCompoundInfusion.h>
#include <biogears/cdm/patient/actions/SESubstanceInfusion.h>
#include <biogears/cdm/patient/actions/SESubstanceNasalDose.h>
#include <biogears/cdm/patient/assessments/SEArterialBloodGasAnalysis.h>
#include <biogears/cdm/patient/assessments/SECompleteBloodCount.h>
#include <biogears/cdm/patient/assessments/SEComprehensiveMetabolicPanel.h>
#include <biogears/cdm/patient/assessments/SEPulmonaryFunctionTest.h>
#include <biogears/cdm/patient/assessments/SEUrinalysis.h>
#include <biogears/cdm/substance/SESubstanceCompound.h>
#include <biogears/cdm/system/physiology/SEDrugSystem.h>
#include <biogears/cdm/system/physiology/SEEnergySystem.h>

#include <biogears/cdm/system/equipment/ElectroCardioGram/SEElectroCardioGram.h>

#include <biogears/cdm/system/equipment/Anesthesia/SEAnesthesiaMachine.h>
#include <biogears/cdm/system/equipment/Anesthesia/SEAnesthesiaMachineOxygenBottle.h>
#include <biogears/cdm/system/equipment/Anesthesia/actions/SEAnesthesiaMachineConfiguration.h>
#include <biogears/cdm/system/equipment/Anesthesia/actions/SEMaskLeak.h>
#include <biogears/cdm/system/equipment/Anesthesia/actions/SEOxygenWallPortPressureLoss.h>

#include <biogears/cdm/scenario/SEAdvanceTime.h>
#include <biogears/cdm/scenario/SEScenario.h>
#include <biogears/cdm/scenario/SEScenarioExec.h>

//#include "PhysiologyEngineManager.h"
#include <mohses/Utility.h>

using namespace biogears;

// Forward declare what we will use in our thread
namespace AMM {
class EventHandler;

class BiogearsThread {
public:
  explicit BiogearsThread(const std::string& stateFile, const std::string& mohses_resource_dir, const std::string& biogears_resource_dir);

  ~BiogearsThread();

  bool LoadScenario(const std::string& scenarioFile);

  bool LoadScenarioFile(const std::string& scenarioFile);

  bool LoadPatient(const std::string& patientFile);

  bool LoadState(const std::string& stateFile, double sec);

  bool SaveState(const std::string& stateFile);

  bool ExecuteXMLCommand(const std::string& cmd);

  bool ExecuteCommand(const std::string& cmd);

  bool Execute(std::function<std::unique_ptr<biogears::PhysiologyEngine>(
                 std::unique_ptr<biogears::PhysiologyEngine>&&)>
                 func);

  bool scenarioLoading = false;

  bool InitializeBioGearsSubstances();

  bool BioGearsLogging();

  void Shutdown();

  void StartSimulation();

  void StopSimulation();

  void AdvanceTimeTick();


  double GetSimulationTime();

  double GetPatientTime();

  std::map<std::string, double (BiogearsThread::*)()>* GetNodePathTable();

  double GetNodePath(const std::string& nodePath);

  void SetVentilator(const std::string& ventilatorSettings);

  void SetBVMMask(const std::string& ventilatorSettings);

  void SetIVPump(const std::string& pumpSettings);

  // AMM Standard patient actions
  void SetAcuteStress(const std::string& actionSettings);

  void SetAirwayObstruction(double severity);

  void SetApnea(const std::string& actionSettings);

  void SetAsthmaAttack(double severity);

  void SetBrainInjury(double severity, const std::string& type);

  void SetBronchoconstriction(const std::string& actionSettings);

  void SetBurnWound(const std::string& actionSettings);

  void SetCardiacArrest(const std::string& actionSettings);

  void SetChestCompression(const std::string& actionSettings);

  void SetChestOcclusiveDressing(const std::string& state, const std::string& side);

  void SetConsciousRespiration(const std::string& actionSettings);

  void SetBreathHold(const std::string& actionSettings);

  void SetForcedExhale(const std::string& actionSettings);

  void SetForcedInhale(const std::string& actionSettings);

  void SetConsciousRespirationCommand(const std::string& actionSettings);

  void SetConsumeNutrients(const std::string& actionSettings);

  void SetExercise(const std::string& actionSettings);

  void SetHemorrhage(const std::string& location, double flow);

  void SetInfection(const std::string& actionSettings);

  void SetIntubation(const std::string& actionSettings);

  void SetMechanicalVentilation(const std::string& actionSettings);

  void SetNeedleDecompression(const std::string& state, const std::string& side);

  void SetPain(const std::string& location, double severity);

  void SetPatientAction(const std::string& actionSettings);

  void SetPatientAssessmentRequest(const std::string& actionSettings);

  void SetPericardialEffusion(const std::string& actionSettings);

  void SetPupillaryResponse(const std::string& actionSettings);

  void SetSepsis(const std::string& location, double severity);

  void SetSubstanceAdministration(const std::string& actionSettings);

  void
  SetSubstanceBolus(const std::string& substance, double concentration, const std::string& concUnit, double dose,
                    const std::string& doseUnit, const std::string& adminRoute);

  void SetSubstanceCompoundInfusion(const std::string& substance, double bagVolume, const std::string& bvUnit,
                                    double rate,
                                    const std::string& rUnit);

  void
  SetSubstanceInfusion(const std::string& substance, double bagVolume, const std::string& bvUnit, double rate,
                       const std::string& rUnit);

  void SetSubstanceNasalDose(const std::string& substance, double dose, const std::string& doseUnit);

  void SetSubstanceOralDose(const std::string& actionSettings);

  void SetTensionPneumothorax(const std::string& type, const std::string& side, double severity);

  void SetUrinate(const std::string& actionSettings);

  void SetUseInhaler(const std::string& actionSettings);

  void SetLogging(bool log);

  void SetLastFrame(int lastFrame);

  void Status();

  bool running = false;

  static std::map<std::string, double (BiogearsThread::*)()> nodePathTable;
  static std::vector<std::string> highFrequencyNodes;

  bool paralyzed = false;
  bool paralyzedSent = false;
  bool irreversible = false;
  bool irreversibleSent = false;
  bool startOfExhale = false;
  bool startOfInhale = false;
  bool pneumothoraxLClosed = false;
  bool pneumothoraxLClosedSent = false;
  bool pneumothoraxRClosed = false;
  bool pneumothoraxRClosedSent = false;
  bool pneumothoraxLOpen = false;
  bool pneumothoraxLOpenSent = false;
  bool pneumothoraxROpen = false;
  bool pneumothoraxROpenSent = false;
  bool hemorrhage = false;
  bool hemorrhageSent = false;
  bool acuteStress = false;
  bool acuteStressSent = false;
  bool asthmaAttack = false;
  bool asthmaAttackSent = false;
  bool brainInjury = false; 
  bool brainInjurySent = false;
  EventHandler* myEventHandler;

private:
  void PopulateNodePathTable();

  double GetLoggingStatus();

  double GetShutdownMessage();

  double GetPatientAge();

  double GetPatientWeight();

  double GetPatientGender();

  double GetPatientHeight();

  double GetPatient_BodyFatFraction();

  double GetGCSValue();

  double GetCerebralPerfusionPressure();

  double GetCerebralBloodFlow();

  double GetBloodLossPercentage();

  double GetHeartRate();

  double GetBloodVolume();

  double GetArterialSystolicPressure();

  double GetArterialDiastolicPressure();

  double GetMeanArterialPressure();

  double GetArterialPressure();

  double GetMeanCentralVenousPressure();

  double GetInspiratoryFlow();

  double GetRespiratoryTotalPressure();

  double GetEndTidalCarbonDioxideFraction();

  double GetEndTidalCarbonDioxidePressure();

  double GetOxygenSaturation();

  double GetCarbonMonoxideSaturation();

  double GetRawRespirationRate();

  double GetRespirationRate();

  double GetPulmonaryResistance();

  double GetCoreTemperature();

  double GetECGWaveform();

  double GetSodiumConcentration();

  double GetSodium();

  double GetGlucoseConcentration();

  double GetBUN();

  double GetIntracranialPressure();

  double GetCreatinineConcentration();

  double GetWhiteBloodCellCount();

  double GetRedBloodCellCount();

  double GetHemoglobinConcentration();

  double GetOxyhemoglobinConcentration();

  double GetCarbaminohemoglobinConcentration();

  double GetOxyCarbaminohemoglobinConcentration();

  double GetCarboxyhemoglobinConcentration();

  double GetIonizedCalcium();

  double GetAnionGap();

  double GetHematocrit();

  double GetRawBloodPH();

  double GetModBloodPH();

  double GetBloodPH();

  double GetArterialCarbonDioxidePressure();

  double GetArterialOxygenPressure();

  double GetVenousOxygenPressure();

  double GetVenousCarbonDioxidePressure();

  double GetBicarbonateConcentration();

  double GetBicarbonate();

  double GetBicarbonateRaw();

  double GetBaseExcess();

  double GetBaseExcessRaw();

  double GetCO2();

  double GetPotassium();

  double GetChloride();

  double GetPlateletCount();

  double GetExhaledCO2();

  double GetExhaledO2();

  double GetTidalVolume();

  double GetTotalLungVolume();

  double GetLeftLungVolume();

  double GetRightLungVolume();

  double GetLeftLungTidalVolume();

  double GetRightLungTidalVolume();

  double GetLeftPleuralCavityVolume();

  double GetRightPleuralCavityVolume();

  double GetLeftAlveoliBaselineCompliance();

  double GetRightAlveoliBaselineCompliance();

  double GetCardiacOutput();

  double GetCalciumConcentration();

  double GetAlbuminConcentration();

  double GetLactateConcentration();

  double GetLactateConcentrationMMOL();

  double GetTotalBilirubin();

  double GetTotalProtein();

  double GetPainVisualAnalogueScale();

  double GetUrineSpecificGravity();

  double GetUrineProductionRate();

  double GetUrineOsmolality();

  double GetUrineOsmolarity();

  double GetBladderGlucose();

  double GetShuntFraction();

  int GlasgowEstimator(double cbf);

  biogears::SESubstance* sodium;
  biogears::SESubstance* glucose;
  biogears::SESubstance* creatinine;
  biogears::SESubstance* calcium;
  biogears::SESubstance* albumin;
  biogears::SESubstance* hemoglobin;
  biogears::SESubstance* bicarbonate;
  biogears::SESubstance* CO2;
  biogears::SESubstance* N2;
  biogears::SESubstance* O2;
  biogears::SESubstance* CO;
  biogears::SESubstance* Hb;
  biogears::SESubstance* HbO2;
  biogears::SESubstance* HbCO2;
  biogears::SESubstance* HbCO;
  biogears::SESubstance* HbO2CO2;
  biogears::SESubstance* potassium;
  biogears::SESubstance* chloride;
  biogears::SESubstance* lactate;

  const biogears::SEGasCompartment* carina;
  const biogears::SEGasCompartment* leftLung;
  const biogears::SEGasCompartment* rightLung;
  const biogears::SELiquidCompartment* bladder;

protected:
  std::mutex m_mutex;
  std::unique_ptr<biogears::BioGearsEngine> m_pe;
  biogears::BioGears* bg;

  // AMM
  AMM::UUID m_uuid;
  std::string moduleName = "AMM_BioGearsThread";
  AMM::DDSManager<BiogearsThread>* m_mgr = nullptr;

  double thresh = 1.0;

  bool falling_L;
  double lung_vol_L, new_min_L, new_max_L, min_lung_vol_L, max_lung_vol_L;
  double chestrise_pct_L;
  double leftLungTidalVol;

  bool falling_R;
  double lung_vol_R, new_min_R, new_max_R, min_lung_vol_R, max_lung_vol_R;
  double chestrise_pct_R;
  double rightLungTidalVol;

  bool eventHandlerAttached = false;

  double bloodPH = 0.0;
  double rawBloodPH = 0.0;
  double lactateConcentration = 0.0;
  double lactateMMOL = 0.0;
  double startingBloodVolume = 5423.53;
  double currentBloodVolume = 0.0;
  double rawRespirationRate = 0.0;

  int lastFrame = 0;

  // Log every 50th frame
  int loggingFrequency = 50;

  bool logging_enabled = false;
};
}
