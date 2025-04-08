
#include "AMM/PhysiologyEngineManager.h"

#include <mohses/BaseLogger.h>

#include <boost/program_options.hpp>
#include <filesystem>

#include <constants.h>
using namespace AMM;

struct Configuration {
  bool closed = false;
  bool daemonize = false;
  bool autostart = false;
  bool logging = false;

  std::string biogears_directory{ MOHSES_RESOURCE_DIR};
  std::string runtime_directory { MOHSES_RUNTIME_DIR};
  std::string dds_directory { MOHSES_RESOURCE_DIR };
} config;

void show_menu(AMM::PhysiologyEngineManager* pe)
{
  std::string action;

  std::cout << std::endl;
  std::cout << " [1]Status" << std::endl;
  std::cout << " [2]Advance Time Tick" << std::endl;
  std::cout
    << " [3]Start\t\t\tRun physiology engine with simulation-manager ticks"
    << std::endl;
  std::cout << " [4]Stop\t\t\tStop running based on simulation-manager ticks"
            << std::endl;
  std::cout
    << " [5]Publish data\t\tPublish all data, right now (running or not)"
    << std::endl;
  std::cout << " [6]Toggle logging  (currently ";
  if (config.logging) {
    std::cout << "enabled";
  } else {
    std::cout << "disabled";
  }
  std::cout << ")" << std::endl;
  std::cout << " [7]Quit" << std::endl;
  std::cout << " [8]Test physmod XML" << std::endl;
  std::cout << " [9]Save state" << std::endl;
  std::cout << " >> ";
  getline(std::cin, action);
  transform(action.begin(), action.end(), action.begin(), ::toupper);

  if (action == "1") {
    pe->Status();
    if (pe->isRunning()) {
      std::cout << " == Running!  At tick count: ";
    } else {
      std::cout << " == Not currently running, paused at tick count: ";
    }
    std::cout << pe->GetTickCount() << std::endl;
  } else if (action == "2") {
    if (!pe->isRunning()) {
      std::cout << " == Simulation not running" << std::endl;
      return;
    }
    std::cout << " == Advancing time one tick" << std::endl;
    pe->AdvanceTimeTick();
  } else if (action == "3") {
    if (!pe->isRunning()) {
      std::cout << " == Starting simulation based on ticks..." << std::endl;
      pe->StartTickSimulation();
    } else {
      std::cout << " == Already running" << std::endl;
    }
  } else if (action == "4") {
    if (pe->isRunning()) {
      std::cout << " == Stopping simulation based on ticks..." << std::endl;
      pe->StopTickSimulation();
    } else {
      std::cout << " == Not running" << std::endl;
    }
  } else if (action == "5") {
    if (pe->isRunning()) {
      std::cout << " == Publishing all data" << std::endl;
      pe->PublishData(true);
      std::cout << " == Done publishing " << pe->GetNodePathCount() << " items." << std::endl;
    }
  } else if (action == "6") {
    config.logging = !config.logging;
    pe->SetLogging(config.logging);
  } else if (action == "7") {
    if (!pe->isRunning()) {
      std::cout << " == Simulation not running, but shutting down anyway" << std::endl;
    }
    pe->StopSimulation();
    pe->Shutdown();
    config.closed = true;
  } else if (action == "8") {
    if (!pe->isRunning()) {
      std::cout << " == Simulation not running" << std::endl;
      return;
    }
    std::string XML = // "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<PhysiologyModification type=\"SubstanceBolus\">\n"
      "  <Substance>Succinylcholine</Substance>\n"
      "  <Concentration value=\"20\" unit=\"mg/mL\"/>\n"
      "  <Dose value=\"5\" unit=\"mL\"/>\n"
      "  <AdminRoute>Intravenous</AdminRoute>\n"
      "</PhysiologyModification>";
    pe->ExecutePhysiologyModification(XML);
  } else if (action == "Z") {
    if (!pe->isRunning()) {
      std::cout << " == Simulation not running" << std::endl;
      return;
    }
    std::string XML = // "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<PhysiologyModification type=\"SubstanceNasalDose\">\n"
      "  <Substance>Naloxone</Substance>\n"
      "  <Dose unit=\"mg\">1.6</Dose>\n"
      "</PhysiologyModification>";
    pe->ExecutePhysiologyModification(XML);
  } else if (action == "9") {
    pe->m_pe->SaveState("test.xml");
    return;
  } else if (action == "0") {
    pe->StopSimulation();
  } else if (action == "LIST") {
    pe->PrintAvailableNodePaths();
  } else if (action == "PRINT") {
    pe->PrintAllCurrentData();
  }
}

int main(int argc, char* argv[])
{
  static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
  plog::init(plog::verbose, &consoleAppender);


  using namespace boost::program_options;

  std::string help_message;
  try {
    // Declare the supported options.
    options_description desc("Allowed options");
    desc.add_options()("help,h", "produce help message")
       ("daemon,d", bool_switch()->default_value(false), "Run in daemon mode for service design.")
       ("autostart,a", bool_switch()->default_value(false), "Starts module-manager immeditally.")
       ("version,v", bool_switch()->default_value(false), "version")
       ("configs", value<std::string>(), "Path to required resource files")
       ("resources", value<std::string>(), "Path to required resource files")
       ("C", value<std::string>(), "Change runtime directory before continuing");

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
      // TODO: PROVIDE THIS AS A #DEFINE FROM CMAKE
      std::cout << "v1.2.0" << std::endl;
      return static_cast<int>(ExecutionErrors::NONE);
    }

    config.daemonize = vm["daemon"].as<bool>();
    config.autostart = vm["autostart"].as<bool>();

    if (vm.count("configs")) {
      config.dds_directory = vm["configs"].as<std::string>();
    }
    if (vm.count("resources")) {
      config.biogears_directory= vm["resources"].as<std::string>();
    }
    if (vm.count("C")) {
      config.runtime_directory = vm["C"].as<std::string>();
      try {
        std::filesystem::current_path(config.runtime_directory);
      } catch (std::filesystem::filesystem_error) {
        std::cerr << "Unable to change directory to " << config.runtime_directory << "given by option -C ";
        exit(static_cast<int>(ExecutionErrors::FILESYSTEM));
      }
    }

  } catch (boost::program_options::required_option /*e*/) {
    std::cout << argv[0] << ": " << help_message << std::endl;
    return (static_cast<int>(ExecutionErrors::ARGUMENTS));
  } catch (std::exception& e) {
    std::cerr << argv[0] << ": " << e.what() << std::endl;
    return (static_cast<int>(ExecutionErrors::UNKNOWN));
  }

  auto* pe = new AMM::PhysiologyEngineManager(config.dds_directory, config.biogears_directory);
  pe->SetLogging(config.logging);
  std::this_thread::sleep_for(std::chrono::milliseconds(250));

  pe->PublishOperationalDescription();
  pe->PublishConfiguration();

  if (config.autostart) {
    LOG_INFO << "Physiology engine wrapper started.";
  }

  while (!config.closed) {
    if (!config.autostart ) {
      show_menu(pe);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout.flush();
  }

  pe->Shutdown();
  LOG_INFO << "Exiting.";

  return 0;
}
