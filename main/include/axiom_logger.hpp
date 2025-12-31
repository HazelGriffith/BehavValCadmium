#ifndef CADMIUM_SIMULATION_LOGGER_AXIOM_HPP_
#define CADMIUM_SIMULATION_LOGGER_AXIOM_HPP_

#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include "cadmium/simulation/logger/logger.hpp"

namespace fs = std::filesystem;

namespace cadmium {
    class AxiomLogger: public Logger {
        private:
            std::string outputpath; // filepath to the transition checks
            std::string atppath; // filepath to the ATP executeable
            std::map<std::string, std::string> modelAxiomPaths; // filepaths to axiom files for each atomic model
            std::ofstream file;
            std::map<std::string, std::map<std::string, std::string>> state_variable_values_per_model; // state variable {name,value} pairs for each atomic model
            std::map<std::string, bool> firstLog_per_model; // is true for given model if it has not been logged yet
            
        public:
            /**
             * Constructor function.
             * @param outputpath filepath to the transition checks
             * @param atppath filepath to the ATP executeable
             */
            AxiomLogger(std::string outputpath, 
                    std::string atppath, 
                    std::map<std::string, std::string> modelAxiomPaths): 
                                                Logger(), 
                                                outputpath(std::move(outputpath)), 
                                                atppath(std::move(atppath)), 
                                                modelAxiomPaths(std::move(modelAxiomPaths)), 
                                                file() {}

            // Starts the output file stream
            void start() override {
                for (auto modelAxiomPath : modelAxiomPaths){
                    auto modelName = modelAxiomPath.first;
                    auto filepath = modelAxiomPath.second;

                    std::ifstream modelAxiomFile(filepath);
                    std::string line;
                    std::map<std::string, std::string> variables;

                    if (modelAxiomFile.is_open()){
                        while (std::getline(modelAxiomFile, line)){
                            if (line == "%-----STATE VARIABLE DEFINITIONS"){
                                break;
                            }
                        }

                        while (std::getline(modelAxiomFile, line)){
                            if (line == "%-----INPUT PORT DEFINITIONS") {
                                break;
                            } else if (line != ""){
                                auto start = line.find("(")+1;
                                auto end = line.find("_type");
                                variables.insert({line.substr(start, end - start),""});
                                std::getline(modelAxiomFile, line);
                                std::getline(modelAxiomFile, line); // skips "next_" variable types
                            }
                        }

                        modelAxiomFile.close();
                    } else {
                        std::cerr << "Unable to open the model axiom file at: " << filepath << std::endl;
                    }

                    state_variable_values_per_model.insert({modelName, variables});
                    firstLog_per_model.insert({modelName, true});
                }


                file.open(outputpath);
                file << "Checking state transition correctness with " << atppath << std::endl;

            }

            //! It closes the output file after the simulation.
            void stop() override {
                file.close();
            }

            /**
             * Virtual method to log atomic models' output messages.
             * @param time current simulation time.
             * @param modelId ID of the model that generated the output message.
             * @param modelName name of the model that generated the output message.
             * @param portName name of the model port in which the output message was created.
             * @param output string representation of the output message.
             */
            void logOutput(double time, long modelId, const std::string& modelName, const std::string& portName, const std::string& output) override {
                std::cout << "\x1B[32m" << time << "," << modelId << "," << modelName << "," << portName << "," << output << "\033[0m" << std::endl;
            }

            /**
             * Virtual method to log atomic models' states.
             * @param time current simulation time.
             * @param modelId ID of the model that generated the output message.
             * @param modelName name of the model that generated the output message.
             * @param state string representation of the state.
             */
            void logState(double time, long modelId, const std::string& modelName, const std::string& state) override {
                if (firstLog_per_model[modelName]){
                    for (auto variable : state_variable_values_per_model[modelName]){
                        std::string variableName = variable.first;
                        auto variableNameLength = variableName.size();
                        auto position = state.find(variableName)+variableNameLength+1;
                        auto end_pos = state.find(";",position);
                        std::string value = state.substr(position,end_pos-position);
                        state_variable_values_per_model.at(modelName).at(variableName) = value;
                    }
                    firstLog_per_model[modelName] = false;
                }
            }


            void logModel(double time, long modelId, const std::shared_ptr<AtomicInterface>& model, bool logOutput) override {
                
                std::string modelName = model->getId();
                if (modelAxiomPaths.count(modelName) != 0){
                    std::string tempFilepath = modelAxiomPaths[modelName].substr(0,modelAxiomPaths[modelName].find(modelName));
                    tempFilepath += modelName + "_temp.p";

                    try{
                        fs::copy_file(modelAxiomPaths[modelName], tempFilepath, fs::copy_options::overwrite_existing);
                    } catch (fs::filesystem_error const& er){
                        std::cerr << "Could not copy the file: " << er.what() << std::endl;
                    }
                    
                    std::string state = model->logState();

                    for (auto variable : state_variable_values_per_model[modelName]){
                        std::string variableName = variable.first;
                        auto variableNameLength = variableName.size();
                        auto position = state.find(variableName)+variableNameLength+1;
                        auto end_pos = state.find(";",position);
                        std::string value = state.substr(position,end_pos-position);

                    }
                }
                
            }
    };
}

#endif // CADMIUM_SIMULATION_LOGGER_AXIOM_HPP_