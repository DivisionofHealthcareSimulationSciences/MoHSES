
#include "SimulationManager.h"

#include <filesystem>
#include <boost/program_options.hpp>
#include <mohses/BaseLogger.h>

#include <constants.h>

struct Configuration {
   bool closed = false;
   bool daemonize = false;
   bool autostart = false;

   int sampleRate = 50;

   std::string runtime_directory{MOHSES_RUNTIME_DIR};
   std::string dds_directory {MOHSES_RESOURCE_DIR};
} config;
using namespace std;
using namespace std::chrono;

bool closed = false;

/// Main menu for this module.
void ShowMenu(AMM::SimulationManager *simManager) {
   string action;

   // std::endl is an automatic flush and should be avoided unless required.
   cout << "\n"
   " [1]Status\n"
   " [2]Run/Resume\n"
   " [3]Reset/Halt\n"
   " [4]Halt/Pause\n"
   " [5]Save State\n"
   " [6]Shutdown\n"
   " [7]Publish OpDesc\n"
   // " [7]Command console\n"
   " >> ";

   getline(cin, action);
   transform(action.begin(), action.end(), action.begin(), ::toupper);

   // Tell Sim Manager to write a new Simulation Control Topic to the network.
   bool doWriteTopic = true;

   // Get state
   if (action == "1") {
      if (simManager->IsRunning()) {
         cout << " == Running!  At tick count: ";
      } else {
         cout << " == Not currently running, paused at tick count: ";
      }

      cout << simManager->GetTickCount() << endl;
      cout << "  = Operating at " << simManager->GetSampleRate()
      << " frames per second." << endl;

      // Start sim
   } else if (action == "2") {
      if (!simManager->IsRunning()) {
         cout << " == Running simulation..." << endl;
         simManager->RunSimulation(doWriteTopic);
      } else {
         cout << " == Simulation already running" << endl;
      }

      // Reset sim
   } else if (action == "3") {
      cout << " == Resetting simulation ..." << endl;
      simManager->ResetSimulation(doWriteTopic);

      // Halt sim
   } else if (action == "4") {
      if (simManager->IsRunning()) {
         cout << " == Halting simulation..." << endl;
         simManager->HaltSimulation(doWriteTopic);
      } else {
         cout << " == Simulation not running" << endl;
      }

      // Save sim state
   } else if (action == "5") {
      cout << " == Saving simulation ..." << endl;
      cout << " ==   Not implemented" << endl;
      simManager->SaveSimulation(doWriteTopic);

      // Shutdown sim
   } else if (action == "6") {
      if (!simManager->IsRunning()) {
         cout << " == Simulation not running, but shutting down anyway" << endl;
      } else {
         cout << " == Stopping simulation and sending shutdown notice..." << endl;
      }

      simManager->HaltSimulation(doWriteTopic);
      cout << " == Exited after " << simManager->GetTickCount() << " ticks."
      << endl;

      cout << "=== [SimManager] Shutting down Simulation Manager." << endl;
      closed = true;
      simManager->Shutdown();
   } else if (action == "7") {
      simManager->PublishOperationalDescription();
   } else {
      // unknown menu action
   }
}

/// Main program
int main(int argc, char *argv[]) {
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
         ("sample-rate,r", value<int>()->default_value(50), "Starts module-manager immeditally.")
         ("version,v", bool_switch()->default_value(false), "version")
         ("configs", value<std::string>(), "Path to required resource files")
         ("directory,C", value<std::string>(), "Path to required resource files");

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
      config.sampleRate= vm["sample-rate"].as<int>();

      if (vm.count("configs")) {
         config.dds_directory = vm["configs"].as<std::string>();
      }
      if (vm.count("directory")) {
         config.runtime_directory = vm["directory"].as<std::string>();
         try {
            std::filesystem::current_path(config.runtime_directory);
         }
         catch (std::filesystem::filesystem_error) {
            std::cerr << "Unable to change directory to " << config.runtime_directory << "given by option -C ";
            exit(static_cast<int>(ExecutionErrors::FILESYSTEM));
         }
      }

   }
   catch (boost::program_options::required_option /*e*/) {
      std::cout << argv[0] << ": " << help_message << std::endl;
      return (static_cast<int>(ExecutionErrors::ARGUMENTS));
   }
   catch (std::exception& e) {
      std::cerr << argv[0] << ": " << e.what() << std::endl;
      return (static_cast<int>(ExecutionErrors::UNKNOWN));
   }


   LOG_INFO << "Simulation Manager starting";
   AMM::SimulationManager simManager(config.dds_directory, config.runtime_directory);
   simManager.SetSampleRate(config.sampleRate);

   std::this_thread::sleep_for(std::chrono::milliseconds(250));

   simManager.PublishOperationalDescription();
   simManager.PublishConfiguration();

   if (config.autostart == 1) {
      LOG_INFO << "Auto-starting simulation";
      simManager.RunSimulation(true);
   }

   while (!closed) {
      if (config.daemonize != 1 && config.autostart != 1) {
         ShowMenu(&simManager);
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
   }

   LOG_INFO << "Exiting.";

   return 0;
}
