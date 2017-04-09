/**
 * @file Backend.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../BenchmarkSet.h"
#include "../tools/Tool.h"
#include "../utils/regex.h"
#include "../results/Results.h"

namespace benchmax {

class Backend {
private:
	Results mResults;
protected:
	std::size_t mExpectedJobs;
	std::atomic<std::size_t> mFinishedJobs;
	std::atomic<std::size_t> mLastPercent;
	
	Backend(): mExpectedJobs(0), mFinishedJobs(0), mLastPercent(0) {}
	
	virtual void startTool(const Tool*) {}
	virtual void execute(const Tool*, const fs::path&, const fs::path&) {}
	void madeProgress(std::size_t files = 1) {
		mFinishedJobs += files;
		std::size_t newPercent = mFinishedJobs * 100 / mExpectedJobs;
		if (newPercent > mLastPercent) {
			mLastPercent = newPercent;
			BENCHMAX_LOG_INFO("benchmax", "Progress: " << mLastPercent << "% (" << mFinishedJobs << " / " << mExpectedJobs << ")");
		}
	}
public:
	void addResult(const Tool* tool, const fs::path& file, const fs::path& baseDir, BenchmarkResult& results) {
		tool->additionalResults(file, results);
		results.cleanup(tool, Settings::timeLimit);
		mResults.addResult(tool, file, baseDir, results);
	}
	void run(const std::vector<Tool*>& tools, const std::vector<BenchmarkSet>& benchmarks) {
		for (const BenchmarkSet& set: benchmarks) {
			mExpectedJobs += tools.size() * set.size();
		}
		for (const Tool* tool: tools) {
			this->startTool(tool);
			for (const BenchmarkSet& set: benchmarks) {
				for (const fs::path& file: set) {
					if (tool->canHandle(file)) {
						//BENCHMAX_LOG_DEBUG("benchmax", "Calling " << tool->binary().native() << " on " << file.native());
						this->execute(tool, file, set.baseDir());
					}
				}
			}
		}
		BENCHMAX_LOG_INFO("benchmax", "Scheduled all jobs, waiting for termination.");
	}
	virtual ~Backend() {
		//Database db;
		//mResults.store(db);
		BENCHMAX_LOG_INFO("benchmax", "Writing results to " << Settings::StatsXMLFile);
		XMLWriter xml(Settings::StatsXMLFile);
		mResults.store(xml);
	}
};

}
