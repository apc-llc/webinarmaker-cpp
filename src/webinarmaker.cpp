#include "movie.h"
#include "task_manager.h"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>
#include <vector>

class ExternalCommand
{
	TaskManager taskManager;

public :

	ExternalCommand() { }
	
	void execute(std::string command, std::vector<std::string> args)
	{
		auto task = taskManager.startTask(command, args, { }, command);

		if (task.first != TaskStarted)
		{
			fprintf(stderr, "Task %s cannot be started\n", command.c_str());
			exit(EXIT_FAILURE);
		}
 
		std::vector<UserTask*> userTasks = { task.second };

		// Wait for the task to finish.
		while (!userTasks.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			std::vector<std::pair<TaskStatus, UserTask*>> events;
			if (taskManager.tryPopTaskEvent(events))
			{
				for (const auto& event : events)
				{
					auto& status = event.first;
					auto& task = event.second;
					auto it = std::find(userTasks.begin(), userTasks.end(), task);

					if (status == TaskTerminatedBySignal)
					{
						fprintf(stderr, "%s exited with a signal\n", (*it)->getName().c_str());
						exit(EXIT_FAILURE);
					}
					if (status == TaskFinishedWithExitCode)
					{
						int exitCode = task->getExitCode();
						if (exitCode != 0)
						{
							fprintf(stderr, "%s exited with code %d\n", (*it)->getName().c_str(), exitCode);
							exit(EXIT_FAILURE);
						}
						userTasks.erase(it);
					}
				}
			}
		}
	}
};

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s <config.json>\n", argv[0]);
		return 1;
	}

	nlohmann::json config;
	{
		std::string configFilename = argv[1];
		std::ifstream configFS(configFilename);
		configFS >> config;
	}

	std::string outputFilename = config["output"];
	std::vector<nlohmann::json> sequence = config["sequence"];

	const unsigned int fps = config["fps"];
	const unsigned int width = config["width"];
	const unsigned int height = config["height"];

	MovieWriter movie(outputFilename, width, height, fps);
	ExternalCommand externalCommand;

	for (auto& item : sequence)
	{
		if (item["type"] == "slide")
		{
			const unsigned int page = item["page"];
			const std::string path = item["path"];
			const double durationSeconds = item["duration"];

			std::cout << "Encoding page " << page << " from " << path << " for " <<
				durationSeconds << " seconds" << std::endl;
			
			// Extract single slide from the PDF using ptftk, e.g.:
			// pdftk A=input.pdf cat A02 output slide.pdf
			std::string pdfFilename = "slide.pdf";
			{
				std::string filenameArg;
				{
					std::stringstream ss;
					ss << "A=";
					ss << path;
					filenameArg = ss.str();
				}
				
				std::string pageArg;
				{
					std::stringstream ss;
					ss << "A";
					ss << page;
					pageArg = ss.str();
				}
				
				externalCommand.execute("pdftk", { filenameArg, "cat", pageArg, "output", pdfFilename });
			}

			// Convert PDF page to SVG image using inkscape, e.g.:
			// inkscape --without-gui --file=slide.pdf --explain-plain-svg=slide.svg 
			const std::string svgFilename = "slide.svg";
			{
				externalCommand.execute("pdf2svg", { pdfFilename, svgFilename });
			}

			// Encode still image to video for the number of seconds.
			const unsigned int nframes = fps * durationSeconds;
			if (nframes)
			{
				// Cache frame in memory, do not read and scale it again.
				AVFrame* cachedFrame = nullptr;
				movie.addFrame(svgFilename, &cachedFrame);
				for (unsigned int iframe = 1; iframe < nframes; iframe++)
					movie.addFrame(cachedFrame);
			}
		}
		else if (item["type"] == "video")
		{
			const std::string path = item["path"];

			std::cout << "Encoding video " << path << std::endl;

			MovieReader video(path, width, height);
			std::vector<uint8_t> pixels;
			while (video.getFrame(pixels))
			{
				movie.addFrame(&pixels[0]);
			}
		}
	}

	return 0;
}

