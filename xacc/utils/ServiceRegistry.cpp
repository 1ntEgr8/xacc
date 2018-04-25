#include "ServiceRegistry.hpp"
#include <boost/filesystem.hpp>
#ifdef XACC_HAS_PYTHON
#include <pybind11/embed.h>
#endif

namespace xacc {

void ServiceRegistry::initialize() {

	if (!initialized) {
		framework = FrameworkFactory().NewFramework();

		// Initialize the framework
		framework.Init();
		context = framework.GetBundleContext();
		if (!context) {
			XACCLogger::instance()->error(
					"Invalid XACC Framework plugin context.");
		}

		// Get the Library path
		std::string xaccLibDir = "";
#ifdef XACC_HAS_PYTHON
		namespace py = pybind11;
		auto getLibraryDir = []() -> std::string {
   		   auto locals = py::dict();
   		   py::exec(R"(
			import os
			imported = True
			try:
			   import pyxacc as xacc
			except ImportError:
			   imported = False

			if imported:
			    xaccLocation = os.path.dirname(os.path.realpath(xacc.__file__))
			   )", py::globals(), locals);
		   bool foundPythonInstall = locals["imported"].cast<bool>();
		   if (foundPythonInstall) {
		        return locals["xaccLocation"].cast<std::string>() + std::string("/lib");
  		   } else {
			XACCLogger::instance()->warning("ServiceRegistry Warning, could not find pyxacc install");
			return std::string(XACC_INSTALL_DIR) + std::string("/lib");;
		   }
		};

		if(!Py_IsInitialized()) {
			py::scoped_interpreter guard{};
			xaccLibDir = getLibraryDir();
		}  else {
			xaccLibDir = getLibraryDir();
		}
#else
		xaccLibDir = std::string(XACC_INSTALL_DIR) + std::string("/lib");
#endif

		XACCLogger::instance()->enqueueLog(
				"XACC Library Directory: " + xaccLibDir);
		for (auto &entry : boost::filesystem::directory_iterator(xaccLibDir)) {
			// We want the gate and aqc bundles that come with XACC
			if (boost::contains(entry.path().filename().string(),
					"libxacc-quantum")) {
				auto name = entry.path().filename().string();
				boost::replace_all(name, "lib", "");
				boost::replace_all(name, ".so", "");
				boost::replace_all(name, ".dy", "");
				XACCLogger::instance()->enqueueLog(
						"Installing base plugin " + name);
				context.InstallBundles(entry.path().string());
			}
		}

		const std::string xaccPluginPath = std::getenv("HOME")
					+ std::string("/.xacc/plugins");

		// Add external plugins...
		boost::filesystem::directory_iterator end_itr;
		if (boost::filesystem::exists(xaccPluginPath)) {
			for (auto& entry : boost::filesystem::directory_iterator(
					xaccPluginPath)) {
				auto p = entry.path();
				if (boost::filesystem::is_directory(p)) {
					for (auto& subentry : boost::filesystem::directory_iterator(
							p)) {
						auto name = subentry.path().filename().string();
						boost::replace_all(name, "lib", "");
						boost::replace_all(name, ".so", "");
						boost::replace_all(name, ".dy", "");
						XACCLogger::instance()->enqueueLog(
								"Installing Plugin " + name);
						context.InstallBundles(subentry.path().string());
					}
				}

			}

		} else {
			XACCLogger::instance()->enqueueLog(
					"There are no plugins. Install plugins to begin working with XACC.");
		}

		XACCLogger::instance()->enqueueLog(
				"Starting the C++ Microservices Framework.");
		// Start the framework itself.
		framework.Start();

		// Our bundles depend on each other in the sense that the consumer
		// bundle expects a ServiceTime service in its activator Start()
		// function. This is done here for simplicity, but is actually
		// bad practice.
		auto bundles = context.GetBundles();
		for (auto b : bundles) {
			b.Start();
		}

		initialized = true;
	}
}

}
