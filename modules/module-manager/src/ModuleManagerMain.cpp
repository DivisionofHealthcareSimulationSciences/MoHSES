#include "ModuleManager.h"

#include <filesystem>
#include <boost/program_options.hpp>

#include "thirdparty/sqlite_modern_cpp.h"

#include <mohses/BaseLogger.h>

#include <constants.h>

using namespace std;
using namespace sqlite;

struct Configuration {
    bool closed = false;
    bool daemonize = false;
    bool autostart = false;
    bool setup = false;
    bool wipe = false;

    std::string runtime_directory{MOHSES_RUNTIME_DIR};
    std::string dds_directory{MOHSES_RESOURCE_DIR};
} config;
/// Clears database tables.
void WipeTables() {
    try {
        sqlite_config config;
        database db("amm.db", config);

        db << "delete from events;";
        db << "delete from module_capabilities;";
        db << "delete from module_status;";
        db << "delete from logs;";

    } catch (exception &e) {
        LOG_ERROR << e.what();
    }

}

/// On start initialization for database tables.
void SetupTables() {

    try {
        sqlite_config config;
        database db("amm.db", config);

        LOG_INFO << "Creating event log table...";
        db << "create table if not exists events("
              "source text,"
              "module_id text,"
              "module_guid text,"
              "module_name text,"
              "event_id text,"
              "topic text,"
              "timestamp bigint,"
              "data text"
              ");";

        LOG_INFO << "Creating module capabilities table...";
        db << "create table if not exists module_capabilities ("
              "module_id text,"
              "module_guid text,"
              "module_name text,"
              "description text,"
              "manufacturer text,"
              "model text,"
              "module_version text,"
              "serial_number text,"
              "capabilities text"
              ");";

        LOG_INFO << "Creating module status table...";
        db << "create table if not exists module_status ("
              "module_id text,"
              "module_guid text,"
              "module_name text,"
              "capability text,"
              "status text,"
              "message text,"
              "timestamp bigint,"
              "encounter_id text"
              ");";

        LOG_INFO << "Creating log record table...";
        db << "create table if not exists logs("
              "module_id text,"
              "module_guid text,"
              "module_name text,"
              "message text,"
              "log_level text,"
              "timestamp bigint"
              ");";

    } catch (exception &e) {
        LOG_ERROR << e.what();
    }

    WipeTables();
}

/// Displays current manager configuration.
static void ShowUsage(const std::string &name) {
    cerr << "Usage: " << name << " <option(s)>"
         << "\nOptions:\n"
         << "\t-a\t\t\tAuto start\n"
         << "\t-d\t\t\tDaemonize\n"
         << "\t-s\t\t\tSetup module manager tables\n"
         << "\t-w\t\t\tWipe tables\n"
         << "\t-h,--help\t\t\tShow this help message\n"
         << endl;
}

/// Main menu for Module Manager.
void ShowMenu(AMM::ModuleManager *modManager) {
    string action;

    cout << endl;
    cout << " [1]Status " << endl;
    cout << " [2]Setup tables" << endl;
    cout << " [3]Wipe tables" << endl;
    cout << " [4]Shutdown" << endl;
    cout << " [5]Test scenario file loading" << endl;
    cout << " >> ";

    getline(cin, action);

    transform(action.begin(), action.end(), action.begin(), ::toupper);

    if (action == "1") {
        modManager->ShowStatus();
    } else if (action == "2") {
        SetupTables();
    } else if (action == "3") {
        WipeTables();
    } else if (action == "4") {
        LOG_INFO << "Shutting down Module Manager.";
        config.closed = true;
        modManager->Shutdown();
    } else if (action == "5") {
        LOG_INFO << "Loading scenario file via COMMAND";
        modManager->SendTestCommand("[SYS]LOAD_SCENARIO:BVM");
    } else if (action == "6") { 
	LOG_INFO << "Loading scenario file via COMMAND for manikin 1";
        modManager->SendTestCommand("[SYS]LOAD_SCENARIO:BVM;mid=manikin_1");

    } else {
            /// TODO: Unknown menu action.

    }
}

int main(int argc, char *argv[]) {
    using namespace boost::program_options;

    std::string help_message;
    try {
      // Declare the supported options.
      options_description desc("Allowed options");
      desc.add_options() 
        ("help,h", "produce help message")
        ("daemon,d", bool_switch()->default_value(false), "[Not implemented] Run in daemon mode for service design." )
        ("autostart,a", bool_switch()->default_value(false), "Starts module-manager immeditally." )
        ("setup,s", bool_switch()->default_value(false), "setup")
        ("wipe,w", bool_switch()->default_value(false), "wipe")
        ("version,v", bool_switch()->default_value(false), "version")
        ("configs", value<std::string>(), "Path to required resource files")
        ("directory,-C", value<std::string>(), "Path to required resource files");

        options_description all_options;
        all_options.add(desc);
        variables_map vm;
        store(command_line_parser(argc, argv).options(all_options).run(), vm);
        notify(vm);

        if (vm.count("help") || argc < 2) {
          std::cout << desc << std::endl;
          return static_cast<int>(ExecutionErrors::NONE);
        }

        if (vm["version"].as<bool>()) {
          //TODO: PROVIDE THIS AS A #DEFINE FROM CMAKE
          std::cout << "v1.2.0" << std::endl;
          return static_cast<int>(ExecutionErrors::NONE);
        }

        config.daemonize = vm["daemon"].as<bool>();
        config.autostart = vm["autostart"].as<bool>();
        config.setup = vm["setup"].as<bool>();
        config.wipe = vm["wipe"].as<bool>();

        if (vm.count("configs")) {
            config.dds_directory= vm["configs"].as<std::string>();
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

    

    static plog::ColorConsoleAppender <plog::TxtFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);

    LOG_INFO << "AMM - Module Manager";

    if (config.setup) {
        LOG_INFO << "Creating AMM database schema.";
        SetupTables();
    }

    if (config.wipe) {
        LOG_INFO << "Wiping tables on startup";
        WipeTables();
    }

    AMM::ModuleManager modManager(config.dds_directory);

    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    modManager.PublishOperationalDescription();
    modManager.PublishConfiguration();

    while (!config.closed) {
        if (!config.autostart) {
            ShowMenu(&modManager);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    modManager.Shutdown();
    LOG_INFO << "Exiting.";

    return 0;
}
