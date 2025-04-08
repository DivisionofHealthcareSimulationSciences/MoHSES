#include <mutex>
#include <chrono>
#include <thread>
#include <iostream>
#include <functional>
#include <condition_variable>
#include <stdexcept>

#include <mohses/net.h>

#include <mohses/BaseLogger.h>

#include <mohses/Utility.h>

#include <pistache/endpoint.h>
#include <pistache/router.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include <filesystem>
#include <boost/algorithm/string/join.hpp>
#include <boost/process.hpp>
#include <boost/program_options.hpp>

#include "thirdparty/sqlite_modern_cpp.h"

#include <constants.h>

using namespace AMM;
using namespace std::chrono;
using namespace rapidjson;
using namespace Pistache;
using namespace eprosima;
using namespace eprosima::fastrtps;
using namespace sqlite;

struct Configuration {
    bool daemonize = true;
    bool discovery = true;

    /// REST adapter port.
    int portNumber = 9080;
    
    /// REST threads.
    int thread_pool_size = 2;
    
    /// Hostname to connect to.
    char hostname[HOST_NAME_MAX];

    std::string action_path = "Actions/";
    std::string state_path = "./states/";
    std::string patient_path = "./patients/";
    std::string scenario_path = "./scenarios/";

    std::string runtime_directory{MOHSES_RUNTIME_DIR};
    std::string resource_path{MOHSES_RESOURCE_DIR};
} config;

std::mutex nds_mutex;
std::map<std::string, double> nodeDataStorage;

std::map<std::string, std::string> statusStorage = {{"STATUS",         "NOT RUNNING"},
                                                    {"TICK",           "0"},
                                                    {"TIME",           "0"},
                                                    {"SCENARIO",       ""},
                                                    {"STATE",          ""},
                                                    {"AIR_SUPPLY",     ""},
                                                    {"CLEAR_SUPPLY",   ""},
                                                    {"BLOOD_SUPPLY",   ""},
                                                    {"FLUIDICS_STATE", ""},
                                                    {"BATTERY1",       ""},
                                                    {"BATTERY2",       ""},
                                                    {"EXT_POWER",      ""},
                                                    {"IVARM_STATE",    ""},
                                                    {"MONITOR_ECG",     ""},
                                                    {"MONITOR_PULSEOX", ""},
                                                    {"MONITOR_NIBP",    ""},
                                                    {"MONITOR_TEMP",    ""},
                                                    {"MONITOR_ARTLINE", ""},
                                                    {"MONITOR_ETCO2",   ""},
                                                   };
std::vector<std::string> labsStorage;

bool m_runThread = false;
int64_t lastTick = 0;

const std::string sysPrefix = "[SYS]";
const std::string actPrefix = "[ACT]";
const std::string loadScenarioPrefix = "LOAD_SCENARIO:";
const std::string loadPrefix = "LOAD_STATE:";
const std::string loadPatientPrefix = "LOAD_PATIENT:";

void SendReset();

/// Resets database tables for labs.
void ResetLabs() {
    labsStorage.clear();
    std::ostringstream labRow;

    labRow << "Time,";

// POCT
    labRow << "POCT,";
    labRow << "Sodium (Na),";
    labRow << "Potassium (K),";
    labRow << "Chloride (Cl),";
    labRow << "TCO2,";
    labRow << "Anion Gap,"; // Anion Gap
    labRow << "Ionized Calcium (iCa),"; // Ionized Calcium (iCa)
    labRow << "Glucose (Glu),";
    labRow << "Urea Nitrogen (BUN)/Urea,";
    labRow << "Creatinine (Crea),";

// Hematology
    labRow << "Hematology,";
    labRow << "Hematocrit (Hct),";
    labRow << "Hemoglobin (Hgb),";

//ABG
    labRow << "ABG,";
    labRow << "Lactate,";
    labRow << "pH,";
    labRow << "modified_pH,";
    labRow << "PCO2,";
    labRow << "PO2,";
    labRow << "TCO2,";
    labRow << "HCO3,";
    labRow << "Base Excess (BE),";
    labRow << "SpO2,";
    labRow << "COHb,";

// VBG
    labRow << "VBG,";
    labRow << "Lactate,";
    labRow << "pH,";
    labRow << "PCO2,";
    labRow << "TCO2,";
    labRow << "HCO3,";
    labRow << "Base Excess (BE),";
    labRow << "COHb,";


    // BMP
    labRow << "BMP,";
    labRow << "Sodium (Na),";
    labRow << "Potassium (K),";
    labRow << "Chloride (Cl),";
    labRow << "TCO2,";
    labRow << "Anion Gap,"; // Anion Gap
    labRow << "Ionized Calcium (iCa),"; // Ionized Calcium (iCa)
    labRow << "Glucose (Glu),";
    labRow << "Urea Nitrogen (BUN)/Urea,";
    labRow << "Creatinine (Crea),";


    // CBC
    labRow << "CBC,";
    labRow << "WBC,";
    labRow << "RBC,";
    labRow << "Hgb,";
    labRow << "Hct,";
    labRow << "Plt,";

// CMP
    labRow << "CMP,";
    labRow << "Albumin,";
    labRow << "ALP,"; // ALP
    labRow << "ALT,"; // ALT
    labRow << "AST,"; // AST
    labRow << "BUN,";
    labRow << "Calcium,";
    labRow << "Chloride,";
    labRow << "CO2,";
    labRow << "Creatinine (men),";
    labRow << "Creatinine (women),";
    labRow << "Glucose,";
    labRow << "Potassium,";
    labRow << "Sodium,";
    labRow << "Total bilirubin,";
    labRow << "Total protein";
    labsStorage.push_back(labRow.str());
}

/// Add to database tables for labs.
void AppendLabRow() {
    std::ostringstream labRow;

    labRow << nodeDataStorage["SIM_TIME"] << ",";

// POCT
    labRow << "POCT,";
    labRow << nodeDataStorage["Substance_Sodium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Potassium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Chloride"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << ","; // Anion Gap
    labRow << ","; // Ionized Calcium (iCa)
    labRow << nodeDataStorage["Substance_Glucose_Concentration"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodUreaNitrogen_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Creatinine_Concentration"] << ",";

// Hematology
    labRow << "Hematology,";
    labRow << nodeDataStorage["BloodChemistry_Hemaocrit"] << ",";
    labRow << nodeDataStorage["Substance_Hemoglobin_Concentration"] << ",";

//ABG
    labRow << "ABG,";
    labRow << nodeDataStorage["Substance_Lactate_Concentration_mmol"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodPH"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodPH_MOD"] << ",";
    labRow << nodeDataStorage["BloodChemistry_Arterial_CarbonDioxide_Pressure"] << ",";
    labRow << nodeDataStorage["BloodChemistry_Arterial_Oxygen_Pressure"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << nodeDataStorage["Substance_Bicarbonate"] << ",";
    labRow << nodeDataStorage["Substance_BaseExcess"] << ",";
    labRow << nodeDataStorage["BloodChemistry_Oxygen_Saturation"] << ",";
    labRow << nodeDataStorage["Substance_Carboxyhemoglobin_Concentration"] << ",";

// VBG
    labRow << "VBG,";
    labRow << nodeDataStorage["Substance_Lactate_Concentration_mmol"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodPH"] << ",";
    labRow << nodeDataStorage["BloodChemistry_VenousCarbonDioxidePressure"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << nodeDataStorage["Substance_Bicarbonate"] << ",";
    labRow << nodeDataStorage["Substance_BaseExcess"] << ",";
    labRow << nodeDataStorage["Substance_Carboxyhemoglobin_Concentration"] << ",";


    // BMP
    labRow << "BMP,";
    labRow << nodeDataStorage["Substance_Sodium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Potassium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Chloride"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << ","; // Anion Gap
    labRow << ","; // Ionized Calcium (iCa)
    labRow << nodeDataStorage["Substance_Glucose_Concentration"] << ",";
    labRow << nodeDataStorage["BloodChemistry_BloodUreaNitrogen_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Creatinine_Concentration"] << ",";


    // CBC
    labRow << "CBC,";
    labRow << nodeDataStorage["BloodChemistry_WhiteBloodCell_Count"] << ",";
    labRow << nodeDataStorage["BloodChemistry_RedBloodCell_Count"] << ",";
    labRow << nodeDataStorage["Substance_Hemoglobin_Concentration"] << ",";
    labRow << nodeDataStorage["BloodChemistry_Hemaocrit"] << ",";
    labRow << nodeDataStorage["CompleteBloodCount_Platelet"] << ",";

// CMP
    labRow << "CMP,";
    labRow << nodeDataStorage["Substance_Albumin_Concentration"] << ",";
    labRow << ","; // ALP
    labRow << ","; // ALT
    labRow << ","; // AST
    labRow << nodeDataStorage["BloodChemistry_BloodUreaNitrogen_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Calcium_Concentration"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Chloride"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_CarbonDioxide"] << ",";
    labRow << nodeDataStorage["Substance_Creatinine_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Creatinine_Concentration"] << ",";
    labRow << nodeDataStorage["Substance_Glucose_Concentration"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Potassium"] << ",";
    labRow << nodeDataStorage["Substance_Sodium"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Bilirubin"] << ",";
    labRow << nodeDataStorage["MetabolicPanel_Protein"];
    labsStorage.push_back(labRow.str());
}

std::string ExtractTypeFromRenderMod(std::string payload) {
    std::size_t pos = payload.find("type=");
    if (pos != std::string::npos) {
        std::string p1 = payload.substr(pos + 6);
        std::size_t pos2 = p1.find("\"");
        if (pos2 != std::string::npos) {
            std::string p2 = p1.substr(0, pos2);
            return p2;
        }
    }
    return {};
};

std::string ExtractManikinIDFromString(std::string in) {
    std::size_t pos = in.find("mid=");
    if (pos != std::string::npos) {
        std::string mid1 = in.substr(pos + 4);
        std::size_t pos1 = mid1.find(";");
        if (pos1 != std::string::npos) {
            std::string mid2 = mid1.substr(0, pos1);
            return mid2;
        }
        return mid1;
    }
    return {};
}

/// Core logic container for DDS Manager functions.
class RESTListener : public ListenerInterface {
public:

    void onNewStatus(AMM::Status &st, SampleInfo_t *info) {
        std::ostringstream statusValue;
        statusValue << AMM::Utility::EStatusValueStr(st.value());

        LOG_DEBUG << "[" << st.module_id().id() << "][" << st.module_name() << "]["
                  << st.capability() << "] Status = " << statusValue.str() << " (" << st.value() << ")";
        // Message = " << st.message();

        if ( st.module_name() == "AMM_FluidManager" || st.module_name() == "Torso_Control" )
        {
            if ( st.capability() == "fluidics" ) {
                statusStorage["FLUIDICS_STATE"] = statusValue.str();
            } else if ( st.capability() == "clear_supply" ) {
                statusStorage["CLEAR_SUPPLY"] = statusValue.str();
            } else if ( st.capability() == "blood_supply") {
                statusStorage["BLOOD_SUPPLY"] = statusValue.str();
            } else if ( st.capability() == "air_supply") {
                statusStorage["AIR_SUPPLY"] = statusValue.str();
                // parse st.message() to double p; [p] = psi
                try {
                    double p = std::stod(st.message());
                    nodeDataStorage["Air_Pressure"] = p;
                } catch (const std::invalid_argument &) {
                    nodeDataStorage["Air_Pressure"] = 0.0;
                } catch (const std::out_of_range &) {
                }
            }
        }

        if ( st.module_name() == "AJAMS_Services" )
        {
            if ( st.capability() == "battery-1" ) {
                statusStorage["BATTERY1"] = statusValue.str();
                // parse st.message() to double soc; [soc] = %
                try {
                    double soc = std::stod(st.message());
                    nodeDataStorage["Battery1_SOC"] = soc;
                } catch (const std::invalid_argument &) {
                    nodeDataStorage["Battery1_SOC"] = 0.0;
                } catch (const std::out_of_range &) {
                }
            } else if ( st.capability() == "battery-2" ) {
                statusStorage["BATTERY2"] = statusValue.str();
                // parse st.message() to double soc; [soc] = %
                try {
                    double soc = std::stod(st.message());
                    nodeDataStorage["Battery2_SOC"] = soc;
                } catch (const std::invalid_argument &) {
                    nodeDataStorage["Battery2_SOC"] = 0.0;
                } catch (const std::out_of_range &) {
                }
            } else if ( st.capability() == "ext_power" ) {
                statusStorage["EXT_POWER"] = statusValue.str();
            }
        }

        if (st.capability() == "iv_detection") {
            statusStorage["IVARM_STATE"] = statusValue.str();
        }
    }

    void onNewTick(AMM::Tick &t, SampleInfo_t *info) {
        if (statusStorage["STATUS"].compare("NOT RUNNING") == 0 &&
            t.frame() > lastTick) {
            statusStorage["STATUS"] = "RUNNING";
        }
        lastTick = t.frame();
        statusStorage["TICK"] = std::to_string(t.frame());
        statusStorage["TIME"] = std::to_string(t.time());
    }

    void onNewSimulationControl(AMM::SimulationControl &simControl, SampleInfo_t *info) {
        switch (simControl.type()) {
            case AMM::ControlType::RUN: {
                statusStorage["STATUS"] = "RUNNING";
                LOG_DEBUG << "SimControl received: Run sim.";
                break;
            }

            case AMM::ControlType::HALT: {
                statusStorage["STATUS"] = "PAUSED";
                break;
            }

            case AMM::ControlType::RESET: {
                LOG_DEBUG
                        << "SimControl received: Reset simulation, clean up and prepare for next run.";
                statusStorage["STATUS"] = "NOT RUNNING";
                statusStorage["TICK"] = "0";
                statusStorage["TIME"] = "0";
                nodeDataStorage.clear();
                ResetLabs();
                break;
            }

        }
    }

    void onNewCommand(AMM::Command &c, SampleInfo_t *info) {
        std::string manikin_id = ExtractManikinIDFromString(c.message());
        LOG_INFO << "Got a command: " << c.message() << " for manikin " << manikin_id;
        if (!c.message().compare(0, sysPrefix.size(), sysPrefix)) {
            std::string value = c.message().substr(sysPrefix.size());
            if (value.find("START_SIM") != std::string::npos) {
                statusStorage["STATUS"] = "RUNNING";
            } else if (value.find("STOP_SIM") != std::string::npos) {
                statusStorage["STATUS"] = "STOPPED";
            } else if (value.find("PAUSE_SIM") != std::string::npos) {
                statusStorage["STATUS"] = "PAUSED";
            } else if (value.find("RESET_SIM") != std::string::npos) {
                statusStorage["STATUS"] = "NOT RUNNING";
                statusStorage["TICK"] = "0";
                statusStorage["TIME"] = "0";
                nodeDataStorage.clear();
                ResetLabs();
            } else if (value.find("END_SIMULATION") != std::string::npos) {
                statusStorage["STATUS"] = "STOPPED";
                statusStorage["TICK"] = "0";
                statusStorage["TIME"] = "0";
                nodeDataStorage.clear();
                ResetLabs();
            } else if (value.find("APPEND_LABS") != std::string::npos) {
                AppendLabRow();
            } else if (value.find("CLEAR_LOG") != std::string::npos) {

            } else if (value.find("RESTART_SERVICE") != std::string::npos) {
		LOG_INFO << "Command: RESTART_SERVICE" << c.message();
            } else if (!value.compare(0, loadPrefix.size(), loadPrefix)) {
                statusStorage["STATE"] = value.substr(loadPrefix.size());
                SendReset();
            } else if (!value.compare(0, loadScenarioPrefix.size(),
                                      loadScenarioPrefix)) {
                statusStorage["SCENARIO"] = value.substr(loadScenarioPrefix.size());
            } else if (!value.compare(0, loadPatientPrefix.size(),
                                      loadPatientPrefix)) {
                statusStorage["PATIENT"] = value.substr(loadPatientPrefix.size());
            }
        } else {
            //LOG_TRACE << "Unknown AMM Command: " << c.message();
        }
    }

    void onNewPhysiologyValue(AMM::PhysiologyValue &n, SampleInfo_t *info) {
        const std::lock_guard<std::mutex> lock(nds_mutex);
        if (!isnan(n.value())) {
            nodeDataStorage[n.name()] = n.value();
        }
    }

    void onNewRenderModification(AMM::RenderModification &rendMod, SampleInfo_t *info) {
        std::ostringstream messageOut;
        messageOut << "[AMM_Render_Modification]"
                   << "type=" << rendMod.type() << ";"
                   << "payload=" << rendMod.data();
        std::string stringOut = messageOut.str();
        //LOG_DEBUG << "Render modification received from AMM: " << stringOut;

        if ( rendMod.type().compare("CONNECT_ECG") == 0 ) {
            statusStorage["MONITOR_ECG"] = "ON";
        } else if ( rendMod.type().compare("DETACH_ECG") == 0 ) {
            statusStorage["MONITOR_ECG"] = "OFF";
        } else if ( rendMod.type().compare("CONNECT_PULSE_OX") == 0 ) {
            statusStorage["MONITOR_PULSEOX"] = "ON";
        } else if ( rendMod.type().compare("DETACH_PULSE_OX") == 0 ) {
            statusStorage["MONITOR_PULSEOX"] = "OFF";
        } else if ( rendMod.type().compare("CONNECT_NIBP") == 0 ) {
            statusStorage["MONITOR_NIBP"] = "ON";
        } else if ( rendMod.type().compare("DETACH_NIBP") == 0 ) {
            statusStorage["MONITOR_NIBP"] = "OFF";
        } else if ( rendMod.type().compare("CONNECT_TEMP_PROBE") == 0 ) {
            statusStorage["MONITOR_TEMP"] = "ON";
        } else if ( rendMod.type().compare("DETACH_TEMP_PROBE") == 0 ) {
            statusStorage["MONITOR_TEMP"] = "OFF";
        } else if ( rendMod.type().compare("CONNECT_ART_LINE") == 0 ) {
            statusStorage["MONITOR_ARTLINE"] = "ON";
        } else if ( rendMod.type().compare("DETACH_ART_LINE") == 0 ) {
            statusStorage["MONITOR_ARTLINE"] = "OFF";
        } else if ( rendMod.type().compare("CONNECT_ETCO2") == 0 ) {
            statusStorage["MONITOR_ETCO2"] = "ON";
        } else if ( rendMod.type().compare("DETACH_ETCO2") == 0 ) {
            statusStorage["MONITOR_ETCO2"] = "OFF";
        } else if ( rendMod.type().compare("ATTACH_TO_PATIENT") == 0 ) {
            statusStorage["MONITOR_ECG"] = "ON";
            statusStorage["MONITOR_PULSEOX"] = "ON";
            statusStorage["MONITOR_NIBP"] = "ON";
            statusStorage["MONITOR_TEMP"] = "ON";
            statusStorage["MONITOR_ARTLINE"] = "ON";
            statusStorage["MONITOR_ETCO2"] = "ON";
        } else if ( rendMod.type().compare("DETACH_FROM_PATIENT") == 0 ) {
            statusStorage["MONITOR_ECG"] = "OFF";
            statusStorage["MONITOR_PULSEOX"] = "OFF";
            statusStorage["MONITOR_NIBP"] = "OFF";
            statusStorage["MONITOR_TEMP"] = "OFF";
            statusStorage["MONITOR_ARTLINE"] = "OFF";
            statusStorage["MONITOR_ETCO2"] = "OFF";
        }
    }
};

const std::string moduleName = "AMM_REST_Adapter";

const std::string rest_adapter_amm= "/rest_adapter_amm.xml";
const std::string rest_adapter_configuration= "/rest_adapter_configuration.xml";
const std::string rest_adapter_capabilities= "/rest_adapter_capabilities.xml";

DDSManager<RESTListener> *mgr;
AMM::UUID m_uuid;

database db("amm.db");

std::string file_time_to_string(const std::filesystem::file_time_type& ftime)
{
#if __cpp_lib_format
    return std::format("{:%c}", ftime);
#else
    std::time_t cftime = std::chrono::system_clock::to_time_t(
        std::chrono::file_clock::to_sys(ftime));
    std::string str = std::asctime(std::localtime(&cftime));
    str.pop_back(); // rm the trailing '\n' put by `asctime`
    return str;
#endif
}

void SendReset() {
    AMM::SimulationControl simControl;
    auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    simControl.timestamp(ms);
    simControl.type(AMM::ControlType::RESET);
    mgr->WriteSimulationControl(simControl);
}

void PublishOperationalDescription() {
    AMM::OperationalDescription od;
    od.name(moduleName);
    od.model("REST Adapter");
    od.manufacturer("Vcom3D");
    od.serial_number("1.0.0");
    od.module_id(m_uuid);
    od.module_version("1.0.0");
    const std::string capabilities = AMM::Utility::read_file_to_string(config.resource_path + rest_adapter_capabilities);
    od.capabilities_schema(capabilities);
    od.description();
    mgr->WriteOperationalDescription(od);
}

AMM::UUID SendEventRecord(
        const std::string &location,
        const std::string &practitioner,
        const std::string &type
) {
    LOG_DEBUG << "Publishing an event record: " << type;
    AMM::EventRecord er;
    AMM::FMA_Location fmaL;
    fmaL.name(location);

    AMM::UUID eventUUID;
    eventUUID.id(mgr->GenerateUuidString());

    er.type(type);
    er.location(fmaL);
    er.id(eventUUID);
    mgr->WriteEventRecord(er);
    return eventUUID;
}

void SendPhysiologyModification(
        AMM::UUID &er_id,
        const std::string &type,
        const std::string &payload) {
    LOG_DEBUG << "Publishing a phys mod: " << type;
    AMM::PhysiologyModification modInstance;
    AMM::UUID instUUID;
    instUUID.id(mgr->GenerateUuidString());
    modInstance.id(instUUID);
    modInstance.type(type);
    modInstance.event_id(er_id);
    modInstance.data(payload);
    mgr->WritePhysiologyModification(modInstance);
}

void SendRenderModification(AMM::UUID &er_id,
                            const std::string &type,
                            const std::string &payload) {
    LOG_DEBUG << "Publishing a render mod: " << type;
    AMM::RenderModification modInstance;
    AMM::UUID instUUID;
    instUUID.id(mgr->GenerateUuidString());
    modInstance.id(instUUID);
    modInstance.type(type);
    modInstance.event_id(er_id);
    modInstance.data(payload);
    mgr->WriteRenderModification(modInstance);
}

void SendPerformanceAssessment(
        AMM::UUID &er_id,
        const std::string &assessment_type,
        const std::string &assessment_info,
        const std::string &step,
        const std::string &comment) {
    LOG_INFO << "Publishing an assessment: " << assessment_type;
    AMM::Assessment assessInstance;
    AMM::UUID instUUID;
    instUUID.id(mgr->GenerateUuidString());
    assessInstance.id(instUUID);
    assessInstance.event_id(er_id);
    assessInstance.comment(comment);
    mgr->WriteAssessment(assessInstance);
}

void SendCommand(const std::string &command) {
    LOG_INFO << "Publishing a command:" << command;
    if (!command.compare(0, sysPrefix.size(), sysPrefix)) {
        std::string value = command.substr(sysPrefix.size());
        if (value.compare("START_SIM") == 0) {
            AMM::SimulationControl simControl;
            auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            simControl.timestamp(ms);
            simControl.type(AMM::ControlType::RUN);
            mgr->WriteSimulationControl(simControl);
            statusStorage["STATUS"] = "RUNNING";
        } else if (value.compare("STOP_SIM") == 0) {
            AMM::SimulationControl simControl;
            auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            simControl.timestamp(ms);
            simControl.type(AMM::ControlType::HALT);
            mgr->WriteSimulationControl(simControl);
            statusStorage["STATUS"] = "STOPPED";
        } else if (value.compare("PAUSE_SIM") == 0) {
            AMM::SimulationControl simControl;
            auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            simControl.timestamp(ms);
            simControl.type(AMM::ControlType::HALT);
            mgr->WriteSimulationControl(simControl);
            statusStorage["STATUS"] = "PAUSED";
        } else if (value.compare("RESET_SIM") == 0) {
            statusStorage["STATUS"] = "NOT RUNNING";
            statusStorage["TICK"] = "0";
            statusStorage["TIME"] = "0";
            nodeDataStorage.clear();
            ResetLabs();
            AMM::SimulationControl simControl;
            auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            simControl.timestamp(ms);
            simControl.type(AMM::ControlType::RESET);
            mgr->WriteSimulationControl(simControl);
        } else if (value.compare("SAVE_STATE") == 0) {
            AMM::SimulationControl simControl;
            auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            simControl.timestamp(ms);
            simControl.type(AMM::ControlType::SAVE);
            mgr->WriteSimulationControl(simControl);
        } else if (value.compare("RESTART_SERVICES") == 0) {
            // Execute restart request here. Do not forward.
            // TODO: this functionality might be more appropriate for the module manager to handle 
            //       the else statement below will forward. Module manager would receive the command and process
            std::string command = "supervisorctl start amm_startup";
            int result = boost::process::system(command);
        } else {
            // Publish a SYS Command
            AMM::Command cmdInstance;
            cmdInstance.message(command);
            mgr->WriteCommand(cmdInstance);
        }
    } else {
        // Publish some other command?
        AMM::Command cmdInstance;
        cmdInstance.message(command);
        mgr->WriteCommand(cmdInstance);
    }
}

void printCookies(const Http::Request &req) {
    auto cookies = req.cookies();
    std::cout << "Cookies: [" << std::endl;
    const std::string indent(4, ' ');
    for (const auto &c : cookies) {
        std::cout << indent << c.name << " = " << c.value << std::endl;
    }
    std::cout << "]" << std::endl;
}

namespace Generic {

    void handleReady(const Rest::Request &request, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "1");
    }
}

class DDSEndpoint {
    static constexpr size_t DefaultMaxPayload = 1024102410;

public:
    explicit DDSEndpoint(Address addr)
            : httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {}

    void init(int thread_pool_size = 2) {
        auto opts = Http::Endpoint::options()
                .threads(thread_pool_size)
                .flags(Tcp::Options::ReuseAddr);
        httpEndpoint->init(opts);
        setupRoutes();
    }

    void start() {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serveThreaded();
    }

    void shutdown() { httpEndpoint->shutdown(); }

private:
    void setupRoutes() {
        using namespace Rest;

        Routes::Get(router, "/instance", Routes::bind(&DDSEndpoint::getInstance, this));
        Routes::Get(router, "/node/:name", Routes::bind(&DDSEndpoint::getNode, this));
        Routes::Get(router, "/nodes", Routes::bind(&DDSEndpoint::getNodes, this));
        Routes::Get(router, "/command/:name",
                    Routes::bind(&DDSEndpoint::issueCommand, this));
        Routes::Get(router, "/ready", Routes::bind(&Generic::handleReady));
        Routes::Get(router, "/debug", Routes::bind(&DDSEndpoint::doDebug, this));

        Routes::Get(router, "/labs", Routes::bind(&DDSEndpoint::getLabsReport, this));

        Routes::Get(router, "/events",
                    Routes::bind(&DDSEndpoint::getEventLog, this));
        Routes::Get(router, "/events/csv",
                    Routes::bind(&DDSEndpoint::getEventLogCSV, this));

        Routes::Get(router, "/logs",
                    Routes::bind(&DDSEndpoint::getDiagnosticLog, this));

        Routes::Get(router, "/logs/csv",
                    Routes::bind(&DDSEndpoint::getDiagnosticLogCSV, this));

        Routes::Get(router, "/modules/count",
                    Routes::bind(&DDSEndpoint::getModuleCount, this));
        Routes::Get(router, "/modules",
                    Routes::bind(&DDSEndpoint::getModules, this));
        Routes::Get(router, "/module/id/:id",
                    Routes::bind(&DDSEndpoint::getModuleById, this));
        Routes::Get(router, "/module/guid/:guid",
                    Routes::bind(&DDSEndpoint::getModuleByGuid, this));

        Routes::Get(router, "/modules/other",
                    Routes::bind(&DDSEndpoint::getOtherModules, this));

        Routes::Get(router, "/shutdown",
                    Routes::bind(&DDSEndpoint::doShutdown, this));

        Routes::Get(router, "/actions",
                    Routes::bind(&DDSEndpoint::getActions, this));
        Routes::Get(router, "/action/:name",
                    Routes::bind(&DDSEndpoint::getAction, this));
        Routes::Post(router, "/action",
                     Routes::bind(&DDSEndpoint::createAction, this));
        Routes::Put(router, "/action/:name",
                    Routes::bind(&DDSEndpoint::updateAction, this));
        Routes::Delete(router, "/action/:name",
                       Routes::bind(&DDSEndpoint::deleteAction, this));

        Routes::Get(router, "/assessments",
                    Routes::bind(&DDSEndpoint::getAssessments, this));
        Routes::Get(router, "/assessment/:name",
                    Routes::bind(&DDSEndpoint::getAssessment, this));
        Routes::Post(router, "/assessment/:name",
                     Routes::bind(&DDSEndpoint::createAssessment, this));
        Routes::Put(router, "/assessment/:name",
                    Routes::bind(&DDSEndpoint::createAssessment, this));
        Routes::Delete(router, "/assessment/:name",
                       Routes::bind(&DDSEndpoint::deleteAssessment, this));

        Routes::Post(router, "/execute",
                     Routes::bind(&DDSEndpoint::executeCommand, this));
        Routes::Options(router, "/execute",
                        Routes::bind(&DDSEndpoint::executeOptions, this));

        Routes::Post(
                router, "/topic/physiology_modification",
                Routes::bind(&DDSEndpoint::executePhysiologyModification, this));
        Routes::Post(router, "/topic/render_modification",
                     Routes::bind(&DDSEndpoint::executeRenderModification, this));
        Routes::Post(
                router, "/topic/performance_assessment",
                Routes::bind(&DDSEndpoint::executePerformanceAssessment, this));
        Routes::Options(router, "/topic/:mod_type",
                        Routes::bind(&DDSEndpoint::executeOptions, this));

        Routes::Get(router, "/patients",
                    Routes::bind(&DDSEndpoint::getPatients, this));

        Routes::Get(router, "/scenarios",
                    Routes::bind(&DDSEndpoint::getScenarios, this));

        Routes::Get(router, "/states", Routes::bind(&DDSEndpoint::getStates, this));
        Routes::Get(router, "/states/:name/delete",
                    Routes::bind(&DDSEndpoint::deleteState, this));
    }

    void getInstance(const Rest::Request &request,
                     Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);

        std::ifstream t("static/current_scenario.txt");
        std::string scenario((std::istreambuf_iterator<char>(t)),
                             std::istreambuf_iterator<char>());
        t.close();

        writer.StartObject();
        writer.Key("name");
        writer.String(config.hostname);
        writer.Key("scenario");
        writer.String(scenario.c_str());
        writer.EndObject();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getStates(const Rest::Request &request, Http::ResponseWriter response) {

        StringBuffer s;
        Writer<StringBuffer> writer(s);

        writer.StartArray();
        if (std::filesystem::exists(config.state_path) && std::filesystem::is_directory(config.state_path)) {
            std::filesystem::path p(config.state_path);
            if (std::filesystem::is_directory(p)) {
                std::vector<std::filesystem::path> paths(
                        std::filesystem::directory_iterator{config.state_path}, std::filesystem::directory_iterator{}
                );
                std::sort(paths.begin(), paths.end());
                for (auto const &this_path : paths) {
                    writer.StartObject();
                    writer.Key("name");
                    writer.String(this_path.filename().c_str());
                    writer.Key("last_updated");
                    auto lwt = std::filesystem::last_write_time(this_path.wstring());
                    writer.String(file_time_to_string(lwt).c_str());
                    writer.EndObject();
                }
            }
        }
        writer.EndArray();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void deleteState(const Rest::Request &request,
                     Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        if (name != "StandardMale@0s.xml") {
            std::ostringstream deleteFile;
            deleteFile << config.state_path << "/" << name;
            std::filesystem::path deletePath(deleteFile.str().c_str());
            if (std::filesystem::exists(deletePath) && is_regular_file(deletePath)) {
                LOG_INFO << "Deleting " << deletePath.string();
                std::filesystem::remove(deletePath);
                response.send(Pistache::Http::Code::Ok, "Deleted",
                              MIME(Application, Json));
            } else {
                response.send(Pistache::Http::Code::Forbidden,
                              "Unable to delete state file", MIME(Application, Json));
            }
        } else {
            response.send(Pistache::Http::Code::Forbidden,
                          "Can not delete default state file",
                          MIME(Application, Json));
        }
    }

    void getScenarios(const Rest::Request &request,
                      Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);

        writer.StartArray();
        if (std::filesystem::exists(config.scenario_path) && std::filesystem::is_directory(config.scenario_path)) {
            std::filesystem::path p(config.scenario_path);
            if (std::filesystem::is_directory(p)) {
                std::filesystem::directory_iterator end_iter;
                for (std::filesystem::directory_iterator dir_itr(p); dir_itr != end_iter; ++dir_itr) {
                    if (is_regular_file(dir_itr->status())) {
                        writer.StartObject();
                        writer.Key("name");
                        writer.String(dir_itr->path().filename().c_str());
                        writer.Key("last_updated");
                        auto lwt = std::filesystem::last_write_time(dir_itr->path().wstring());
                        writer.String(file_time_to_string(lwt).c_str());
                        writer.EndObject();
                    }
                }
            }
        }
        writer.EndArray();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Pistache::Http::Code::Ok, s.GetString(),
                      MIME(Application, Json));
    }

    void getPatients(const Rest::Request &request,
                     Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);

        writer.StartArray();
        if (std::filesystem::exists(config.patient_path) && std::filesystem::is_directory(config.patient_path)) {
            std::filesystem::path p(config.patient_path);
            if (std::filesystem::is_directory(p)) {
                std::filesystem::directory_iterator end_iter;
                for (std::filesystem::directory_iterator dir_itr(p); dir_itr != end_iter; ++dir_itr) {
                    if (is_regular_file(dir_itr->status())) {
                        writer.StartObject();
                        writer.Key("name");
                        writer.String(dir_itr->path().filename().c_str());
                        writer.Key("description");
                        std::stringstream writeTime;
                        auto lwt = std::filesystem::last_write_time(dir_itr->path().wstring());
                        writer.String(file_time_to_string(lwt).c_str());
                        writer.EndObject();
                    }
                }
            }
        }
        writer.EndArray();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Pistache::Http::Code::Ok, s.GetString(),
                      MIME(Application, Json));
    }

    void getAssessments(const Rest::Request &request, Http::ResponseWriter response) {
        LOG_INFO << "Get a list of all assessment CSVs";

    }

    int writeToFile(const std::string & filename, const std::string & data) {
        std::fstream out;

        out.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);

        LOG_INFO << "Writing to " << filename;

        // If file does not exist, Create new file
        if (!out)
        {
            LOG_WARNING << "Cannot open file, file does not exist. Creating new file.";
            out.open(filename);
        }

        if (out.fail()) {
            LOG_ERROR << "Open failure: " << strerror(errno);
            return -1;
        }

        out << data;
        out.close();

        LOG_INFO << "All done";
        return 0;
    }

    void createAssessment(const Rest::Request &request,
                          Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
        if (name.empty()) {
            LOG_INFO << "Name was empty";
            name = "test.csv";
        }

        std::string filename = "assessments/" + name;
        LOG_INFO << "Create an assessment from a POST.  Filename is " << filename;
        auto s = std::chrono::steady_clock::now();
        writeToFile(filename, request.body());
        auto dt = std::chrono::steady_clock::now() - s;
        LOG_INFO << "File upload processed in: " << std::chrono::duration_cast<std::chrono::microseconds>(dt).count()<< std::endl;
        response.send(Http::Code::Ok);

        LOG_INFO << "Sending out system message that there's an assessment available.";
        std::string command = "[SYS]ASSESSMENT_AVAILABLE:" + name;
        AMM::Command cmdInstance;
        cmdInstance.message(command);
        mgr->WriteCommand(cmdInstance);
    }

    void deleteAssessment(const Rest::Request &request,
                          Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
    }

    void getAssessment(const Rest::Request &request, Http::ResponseWriter response) {
        std::string name = request.param(":name").as<std::string>();
        std::string fileName = "assessments/" + name;
        LOG_INFO << "Recieved a request to GET an assessment file, so we're serving this up: " << fileName;
        Http::serveFile(response, fileName.c_str());
    }



    void getActions(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);

        writer.StartArray();
        if (std::filesystem::exists(config.action_path) && std::filesystem::is_directory(config.action_path)) {
            std::filesystem::path p(config.action_path);
            if (std::filesystem::is_directory(p)) {
                std::filesystem::directory_iterator end_iter;
                for (std::filesystem::directory_iterator dir_itr(p); dir_itr != end_iter; ++dir_itr) {
                    if (is_regular_file(dir_itr->status())) {
                        writer.StartObject();
                        writer.Key("name");
                        writer.String(dir_itr->path().filename().c_str());
                        writer.Key("description");
                        auto lwt = std::filesystem::last_write_time(dir_itr->path().wstring());
                        writer.String(file_time_to_string(lwt).c_str());
                        writer.EndObject();
                    }
                }
            }
        }
        writer.EndArray();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Pistache::Http::Code::Ok, s.GetString(),
                      MIME(Application, Json));
    }

    void executeCommand(const Rest::Request &request,
                        Http::ResponseWriter response) {
        Document document;
        document.Parse(request.body().c_str());
        std::string payload = document["payload"].GetString();

        SendCommand(payload);

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Pistache::Http::Code::Ok,
                      "{\"message\":\"Command executed\"}");
    }

    void executePhysiologyModification(const Rest::Request &request,
                                       Http::ResponseWriter response) {
        std::string type, location, practitioner, payload;
        Document document;
        document.Parse(request.body().c_str());
        if (document.HasMember("type")) {
            type = document["type"].GetString();
        }
        if (document.HasMember("location")) {
            location = document["location"].GetString();
        }
        if (document.HasMember("practitioner")) {
            practitioner = document["practitioner"].GetString();
        }
        if (document.HasMember("payload")) {
            payload = document["payload"].GetString();
        }
        AMM::UUID erID = SendEventRecord(location, practitioner, type);
        SendPhysiologyModification(erID, type, payload);
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Pistache::Http::Code::Ok,
                      "{\"message\":\"Physiology modification published\"}");
    }

    void executeRenderModification(const Rest::Request &request,
                                   Http::ResponseWriter response) {
        std::string type, location, practitioner, payload;
        Document document;
        document.Parse(request.body().c_str());
        if (document.HasMember("type")) {
            type = document["type"].GetString();
        }
        if (document.HasMember("location")) {
            location = document["location"].GetString();
        }
        if (document.HasMember("practitioner")) {
            practitioner = document["practitioner"].GetString();
        }
        if (document.HasMember("payload")) {
            payload = document["payload"].GetString();
        }
        AMM::UUID erID = SendEventRecord(location, practitioner, type);
        SendRenderModification(erID, type, payload);
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Pistache::Http::Code::Ok,
                      "{\"message\":\"Render modification published\"}");
    }

    void executePerformanceAssessment(const Rest::Request &request,
                                      Http::ResponseWriter response) {
        std::string type, location, practitioner, payload, info, step, comment;
        Document document;
        document.Parse(request.body().c_str());
        if (document.HasMember("type")) {
            type = document["type"].GetString();
        }
        if (document.HasMember("location")) {
            location = document["location"].GetString();
        }
        if (document.HasMember("practitioner")) {
            practitioner = document["practitioner"].GetString();
        }
        if (document.HasMember("info")) {
            info = document["info"].GetString();
        }
        if (document.HasMember("step")) {
            step = document["step"].GetString();
        }
        if (document.HasMember("comment")) {
            comment = document["comment"].GetString();
        }
        AMM::UUID erID = SendEventRecord(location, practitioner, type);
        SendPerformanceAssessment(erID, type, info, step, comment);
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Pistache::Http::Code::Ok,
                      "{\"message\":\"Performance assessment published\"}");
    }

    void executeOptions(const Rest::Request &request,
                        Http::ResponseWriter response) {
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::AccessControlAllowHeaders>("*");
        response.send(Pistache::Http::Code::Ok, "{\"message\":\"success\"}");
    }

    void createAction(const Rest::Request &request,
                      Http::ResponseWriter response) {}

    void deleteAction(const Rest::Request &request,
                      Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
    }

    void updateAction(const Rest::Request &request,
                      Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
    }

    void getAction(const Rest::Request &request, Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
    }

    void issueCommand(const Rest::Request &request,
                      Http::ResponseWriter response) {
        auto name = request.param(":name").as<std::string>();
        SendCommand(name);
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartObject();
        writer.Key("Sent command");
        writer.String(name.c_str());
        writer.EndObject();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString());
    }

    void getModuleById(const Rest::Request &request,
                       Http::ResponseWriter response) {
        auto id = request.param(":id").as<std::string>();
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        db << "SELECT "
              "module_id AS module_id,"
              "module_name AS module_name,"
              "description AS description,"
              "capabilities as capabilities,"
              "manufacturer as manufacturer,"
              "model as model "
              " FROM "
              " module_capabilities "
              " WHERE module_id = ?"
           << id >>
           [&](std::string module_id, std::string module_guid, std::string module_name, std::string description,
               std::string capabilities, std::string manufacturer, std::string model) {
               writer.StartObject();

               writer.Key("Module_ID");
               writer.String(module_id.c_str());

               writer.Key("Module_Name");
               writer.String(module_name.c_str());

               writer.Key("Description");
               writer.String(description.c_str());

               writer.Key("Manufacturer");
               writer.String(manufacturer.c_str());

               writer.Key("Model");
               writer.String(model.c_str());

               writer.Key("Module_Capabilities");
               writer.String(capabilities.c_str());

               writer.EndObject();
           };

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getModuleByGuid(const Rest::Request &request,
                         Http::ResponseWriter response) {
        auto guid = request.param(":guid").as<std::string>();
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        db << "SELECT "
              "module_id AS module_id,"
              "module_guid as module_guid,"
              "module_name AS module_name,"
              "capabilities as capabilities,"
              "manufacturer as manufacturer,"
              "model as model "
              " FROM "
              " module_capabilities "
              " WHERE module_guid = ?"
           << guid >>
           [&](std::string module_id, std::string module_guid, std::string module_name,
               std::string capabilities, std::string manufacturer, std::string model) {
               writer.StartObject();

               writer.Key("Module_ID");
               writer.String(module_id.c_str());

               writer.Key("Module_GUID");
               writer.String(module_guid.c_str());

               writer.Key("Module_Name");
               writer.String(module_name.c_str());

               writer.Key("Manufacturer");
               writer.String(manufacturer.c_str());

               writer.Key("Model");
               writer.String(model.c_str());

               writer.Key("Module_Capabilities");
               writer.String(capabilities.c_str());

               writer.EndObject();
           };

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getModuleCount(const Rest::Request &request,
                        Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);

        int totalCount = 0;
        int coreCount = 0;
        int otherCount = 0;
        db << "SELECT COUNT(DISTINCT module_name) FROM module_capabilities" >> totalCount;
        //db << "SELECT COUNT(DISTINCT module_name) FROM module_capabilities where module_name LIKE 'AMM_%'" >> coreCount;

        otherCount = totalCount - coreCount;

        writer.StartObject();
        writer.Key("module_count");
        writer.Int(totalCount);
        writer.Key("core_count");
        writer.Int(coreCount);
        writer.Key("other_count");
        writer.Int(otherCount);
        writer.EndObject();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getOtherModules(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();
        db << "SELECT DISTINCT module_name FROM module_capabilities where module_name NOT LIKE 'AMM_%'"
           >> [&](std::string module_name) {
               writer.String(module_name.c_str());
           };
        writer.EndArray();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getModules(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();
        db << "SELECT "
              "module_capabilities.module_id AS module_id,"
              "module_capabilities.module_name AS module_name,"
              "module_capabilities.description AS description,"
              "module_capabilities.capabilities as capabilities,"
              "module_capabilities.manufacturer as manufacturer,"
              "module_capabilities.model as model "
              " FROM "
              " module_capabilities; " >>
           [&](std::string module_id, std::string module_name, std::string description,
               std::string capabilities,
               std::string manufacturer, std::string model) {
               writer.StartObject();

               writer.Key("Module_ID");
               writer.String(module_id.c_str());

               writer.Key("Module_Name");
               writer.String(module_name.c_str());

               writer.Key("Description");
               writer.String(description.c_str());

               writer.Key("Manufacturer");
               writer.String(manufacturer.c_str());

               writer.Key("Model");
               writer.String(model.c_str());

               writer.Key("Module_Capabilities");
               writer.String(capabilities.c_str());

               writer.EndObject();
           };

        writer.EndArray();

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getEventLog(const Rest::Request &request,
                     Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();
        db << "SELECT "
              "module_capabilities.module_id as module_id,"
              "module_capabilities.module_name as module_name,"
              "events.source,"
              "events.topic,"
              "events.timestamp,"
              "events.data "
              "FROM "
              "events "
              "LEFT JOIN module_capabilities "
              "ON "
              "events.source = module_capabilities.module_guid" >>
           [&](std::string module_id, std::string module_name, std::string source, std::string topic, int64_t timestamp,std::string data) {
               writer.StartObject();
               writer.Key("module_id");
               writer.String(module_id.c_str());
               writer.Key("source");
               writer.String(module_name.c_str());
               writer.Key("module_guid");
               writer.String(source.c_str());
               writer.Key("timestamp");
               writer.Uint64(timestamp);
               writer.Key("topic");
               writer.String(topic.c_str());
               writer.Key("message");
               writer.String(data.c_str());
               writer.EndObject();
           };

        writer.EndArray();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getEventLogCSV(const Rest::Request &request,
                        Http::ResponseWriter response) {
        std::ostringstream s;
        db << "SELECT "
              "module_capabilities.module_name,"
              "events.source,"
              "events.topic,"
              "events.tick,"
              "events.timestamp,"
              "events.data "
              "FROM "
              "events "
              "LEFT JOIN module_capabilities "
              "ON "
              "events.source = module_capabilities.module_guid" >>
           [&](std::string module_name, std::string source, std::string topic, int64_t tick, int64_t timestamp,
               std::string data) {
               std::time_t temp = timestamp;
               std::tm *t = std::gmtime(&temp);
               s << std::put_time(t, "%Y-%m-%d %I:%M:%S %p") << "," << module_name << "," << source << "," << topic
                 << "," << data << std::endl;
           };


        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        auto csvHeader = Http::Header::Raw("Content-Disposition", "attachment;filename=amm_timeline_log.csv");
        response.headers().addRaw(csvHeader);
        response.send(Http::Code::Ok, s.str(), Http::Mime::MediaType::fromString("text/csv"));
    }

    void getDiagnosticLog(const Rest::Request &request,
                          Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();
        db << "SELECT "
              "logs.module_name, "
              "logs.module_guid, "
              "logs.module_id, "
              "logs.message,"
              "logs.log_level,"
              "logs.timestamp "
              "FROM "
              "logs " >>
           [&](std::string module_name, std::string module_guid, std::string module_id, std::string message, std::string log_level,
               int64_t timestamp) {

               writer.StartObject();
               writer.Key("source");
               writer.String(module_name.c_str());
               writer.Key("module_guid");
               writer.String(module_guid.c_str());
               writer.Key("module_id");
               writer.String(module_id.c_str());
               writer.Key("timestamp");
               writer.Uint64(timestamp);
               writer.Key("log_level");
               writer.String(log_level.c_str());
               writer.Key("message");
               writer.String(message.c_str());
               writer.EndObject();
           };

        writer.EndArray();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getDiagnosticLogCSV(const Rest::Request &request,
                             Http::ResponseWriter response) {
        std::ostringstream s;
        db << "SELECT "
              "logs.module_name, "
              "logs.module_guid, "
              "logs.module_id, "
              "logs.message,"
              "logs.log_level,"
              "logs.timestamp "
              "FROM "
              "logs " >>
           [&](std::string module_name, std::string module_guid, std::string module_id, std::string message, std::string log_level,
               int64_t timestamp) {
               std::time_t temp = timestamp;
               std::tm *t = std::gmtime(&temp);
               s << std::put_time(t, "%Y-%m-%d %I:%M:%S %p") << "," << log_level << "," << module_name << "," << message
                 << std::endl;
           };

        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        auto csvHeader = Http::Header::Raw("Content-Disposition", "attachment;filename=amm_diagnostic_log.csv");
        response.headers().addRaw(csvHeader);
        response.send(Http::Code::Ok, s.str(), Http::Mime::MediaType::fromString("text/csv"));
    }

    void getNodes(const Rest::Request &request, Http::ResponseWriter response) {
        StringBuffer s;
        Writer<StringBuffer> writer(s);
        writer.StartArray();

        auto nit = nodeDataStorage.begin();
        while (nit != nodeDataStorage.end()) {
            writer.StartObject();
            writer.Key(nit->first.c_str());
            writer.Double(nit->second);
            writer.EndObject();
            ++nit;
        }

        auto sit = statusStorage.begin();
        while (sit != statusStorage.end()) {
            writer.StartObject();
            writer.Key(sit->first.c_str());
            writer.String(sit->second.c_str());
            writer.EndObject();
            ++sit;
        }

        writer.EndArray();
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
    }

    void getLabsReport(const Rest::Request &request, Http::ResponseWriter response) {
        std::string labReport = boost::algorithm::join(labsStorage, "\n");
        auto mime = Http::Mime::MediaType::fromString("text/csv");
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.send(Http::Code::Ok, labReport, mime);
    }

    void getNode(const Rest::Request &request, Http::ResponseWriter response) {

        auto name = request.param(":name").as<std::string>();
        auto it = nodeDataStorage.find(name);
        if (it != nodeDataStorage.end()) {
            StringBuffer s;
            Writer<StringBuffer> writer(s);
            writer.StartObject();
            writer.Key(it->first.c_str());
            writer.Double(it->second);
            writer.EndObject();
            response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
            response.send(Http::Code::Ok, s.GetString(), MIME(Application, Json));
        } else {
            response.send(Http::Code::Not_Found, "Node data does not exist");
        }
    }

    void doDebug(const Rest::Request &request, Http::ResponseWriter response) {
        printCookies(request);
        response.cookies().add(Http::Cookie("lang", "en-US"));
        response.send(Http::Code::Ok);
    }

    void doShutdown(const Rest::Request &request, Http::ResponseWriter response) {
        m_runThread = false;
        response.cookies().add(Http::Cookie("lang", "en-US"));
        response.send(Http::Code::Ok);
    }

    typedef std::mutex Lock;
    typedef std::lock_guard<Lock> Guard;
    Lock commandLock;

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
};


Port port(static_cast<uint16_t>(config.portNumber));
Address addr(Ipv4::any(), port);
DDSEndpoint server(addr);

int main(int argc, char *argv[]) {
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);

    using namespace boost::program_options;
    std::string help_message;
    try {

      // Declare the supported options.
      options_description desc("Allowed options");
      desc.add_options() 
        ("help,h", "produce help message")
        ("daemon,d", bool_switch()->default_value(true), "[Not implemented] Run in daemon mode for service design." )
        ("discovery", bool_switch()->default_value(true), "[Not implemented] Enable Net Discovery." )
        ("version,v", bool_switch()->default_value(false), "version")
        ("port", value<int>()->default_value(9080), "Server Port")
        ("jobs", value<int>()->default_value(2), "Number of thread_pool jobs")
        ("configs", value<std::string>(), "Path to required resource files")
        ("directory,-C", value<std::string>(), "Path to required resource files");

      options_description all_options;
      all_options.add(desc);
      variables_map vm;
      store(command_line_parser(argc, argv).options(all_options).run(), vm);
      notify(vm);

      if (vm.count("help")) {
        std::cout << desc << std::endl;
        return static_cast<int>(ExecutionErrors::NONE);
      }

      if (vm["version"].as<bool>()) {
        //TODO: PROVIDE THIS AS A #DEFINE FROM CMAKE
        std::cout << "v1.2.0" << std::endl;
        return static_cast<int>(ExecutionErrors::NONE);
      }

      config.daemonize = vm["daemon"].as<bool>();
      config.discovery = vm["discovery"].as<bool>();
      config.portNumber = vm["port"].as<int>();
      config.thread_pool_size = vm["jobs"].as<int>();

      if (vm.count("configs")) {
          config.resource_path= vm["configs"].as<std::string>();
      }
      if (vm.count("directory")) {
          config.runtime_directory = vm["directory"].as<std::string>();
          try {
            std::filesystem::current_path(config.runtime_directory);
          } catch ( std::filesystem::filesystem_error ) {
               std::cerr << "Unable to change directory to " << config.runtime_directory                           << "given by option -C ";
               exit(static_cast<int>(ExecutionErrors::FILESYSTEM));
          }
      }
    } catch (boost::program_options::required_option /*e*/) {
      std::cout << argv[0]  << ": " << help_message << std::endl;
      return (static_cast<int>(ExecutionErrors::ARGUMENTS));
    } catch (std::exception& e) {
      std::cerr << argv[0]  << ": " << e.what() << std::endl;
      return (static_cast<int>(ExecutionErrors::UNKNOWN));
    }


    std::string action;

    ResetLabs();

    RESTListener al;
    
    if ( !std::filesystem::exists( config.resource_path+ rest_adapter_amm )){
      config.resource_path+= "/config";
    }
    mgr = new AMM::DDSManager<RESTListener>(config.resource_path + rest_adapter_amm);

    mgr->InitializeCommand();
    mgr->InitializeSimulationControl();
    mgr->InitializePhysiologyValue();
    mgr->InitializeTick();
    mgr->InitializeEventRecord();
    mgr->InitializeRenderModification();
    mgr->InitializePhysiologyModification();
    mgr->InitializeAssessment();

    mgr->InitializeOperationalDescription();
    mgr->InitializeModuleConfiguration();
    mgr->InitializeStatus();

    mgr->CreateTickSubscriber(&al, &RESTListener::onNewTick);
    mgr->CreatePhysiologyValueSubscriber(&al, &RESTListener::onNewPhysiologyValue);
    mgr->CreateCommandSubscriber(&al, &RESTListener::onNewCommand);
    mgr->CreateStatusSubscriber(&al, &RESTListener::onNewStatus);
    mgr->CreateRenderModificationSubscriber(&al, &RESTListener::onNewRenderModification);
    mgr->CreateSimulationControlSubscriber(&al, &RESTListener::onNewSimulationControl);

    mgr->CreateAssessmentPublisher();
    mgr->CreateRenderModificationPublisher();
    mgr->CreatePhysiologyModificationPublisher();
    mgr->CreateSimulationControlPublisher();
    mgr->CreateCommandPublisher();
    mgr->CreateEventRecordPublisher();

    mgr->CreateOperationalDescriptionPublisher();
    mgr->CreateModuleConfigurationPublisher();
    mgr->CreateStatusPublisher();

    m_uuid.id(mgr->GenerateUuidString());

    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    PublishOperationalDescription();

    gethostname(config.hostname, HOST_NAME_MAX);

    server.init(config.thread_pool_size);
    LOG_INFO << "Listening on *:" << config.portNumber;

    m_runThread = true;

    server.start();

    LOG_INFO << "Ready.";

    while (m_runThread) {
        getline(std::cin, action);
        transform(action.begin(), action.end(), action.begin(), ::toupper);
        if (action == "EXIT") {
            m_runThread = false;
            LOG_INFO << "Shutting down from command-line.";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        std::cout.flush();
    }

    server.shutdown();

    LOG_INFO << "Shutdown complete";

    return 0;
}
