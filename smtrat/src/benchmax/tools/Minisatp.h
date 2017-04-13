/**
 * @file   smtratSolverTool.h
 *         Created on April 10, 2013, 3:37 PM
 * @author: Sebastian Junges
 *
 *
 */

#pragma once

#include "Tool.h"

#include "../Settings.h"
#include "../utils/Execute.h"
#include "../utils/regex.h"

namespace benchmax {

class Minisatp: public Tool {
public:
	Minisatp(const fs::path& binary, const std::string& arguments): Tool("Minisatp", binary, arguments) {
		mArguments += " -v0";
	}

	virtual bool canHandle(const fs::path& path) const override {
		return isExtension(path, ".opb");
	}
	
	std::string getStatus(const BenchmarkResult& result) const override {
		if (result.stdout.find("s OPTIMUM FOUND") != std::string::npos) return "sat";
		if (result.stdout.find("s SATISFIABLE") != std::string::npos) return "sat";
		if (result.stdout.find("s UNSATISFIABLE") != std::string::npos) return "unsat";
		if (result.stdout.find("s UNKNOWN") != std::string::npos) return "unknown";
		return "timeout";
	}
};

}
