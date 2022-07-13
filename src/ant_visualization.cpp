#include <ant_visualization.hpp>

#include <set>
#include <fstream>

#include <json.hpp>

namespace arti {

    bool AntVisualization::onInit() {
        // Initialize variables and ImGui stuff
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForSDLRenderer(wind.get(), renderer.get());
        ImGui_ImplSDLRenderer_Init(renderer.get());

        renderer.resizeLayer({640, 640});
        renderer.offsetLayer({220, 0});
        renderer.clear(color::OffBlack);

        movingLayer = renderer.createLayer();
        renderer.setTargetedLayer(movingLayer);
        renderer.resizeLayer({640, 640});
        renderer.offsetLayer({220, 0});
        renderer.clear(color::Transparent);

        guiLayer = renderer.createLayer();

        zoom = 1.0f;
        accTime = 0.0f;

        addNode = false;
        algoStep = false;
        autoRun = false;
        showAllEdges = true;
        modalOpen = false;
        editMode = false;
        nAlgoStep = 0;

        maxPheromone = 1.0;
        bestPathSoFarLength = std::numeric_limits<double>::max();
        actBestPath = std::numeric_limits<double>::max();

        updateStaticLayer();

        return true;
    }

    void AntVisualization::onPollEvent(const SDL_Event& event) {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }

    bool AntVisualization::onUpdate(double delta) {
        // Clear screen
        renderer.setTargetedLayer(movingLayer);
        renderer.clear(color::Transparent);

        math::vec2df mousePos = input.mousePos();

        if (! modalOpen) {

            // Panning and zoomming
            if (mousePos >= math::vec2dd{220, 0}) {
                math::vec2df worldPos{
                    (mousePos.x - renderer.getLayerOffset().x) / renderer.getLayerScale().x,
                    (mousePos.y - renderer.getLayerOffset().y) / renderer.getLayerScale().y
                };

                if (input.keyHeld(key_t::Lshift)) {
                    if (input.buttonPressed(button_t::Left)) {
                        panning = mousePos;
                    }
                    else if (input.buttonHeld(button_t::Left)) {
                        renderer.offsetLayer(mousePos - panning);
                        renderer.targetDefaultLayer();
                        renderer.offsetLayer(mousePos - panning);
                        renderer.setTargetedLayer(movingLayer);
                        panning = mousePos;
                    }
                    else if (input.buttonReleased(button_t::Left)) {
                        panning = {0, 0};
                    }

                    auto vScroll = input.verticalScroll();

                    if (vScroll > 0) {
                        zoom *= 1.1;
                        renderer.scaleLayer({zoom, zoom}, mousePos);
                        renderer.targetDefaultLayer();
                        renderer.scaleLayer({zoom, zoom}, mousePos);
                        renderer.setTargetedLayer(movingLayer);
                    }
                    else if (vScroll < 0) {
                        zoom *= 0.9;
                        renderer.scaleLayer({zoom, zoom}, mousePos);
                        renderer.targetDefaultLayer();
                        renderer.scaleLayer({zoom, zoom}, mousePos);
                        renderer.setTargetedLayer(movingLayer);
                    }
                }
                else {
                    // Edit mode
                    if (editMode) {
                        // Add a node
                        if (input.buttonReleased(button_t::Right) && !chosenNode.has_value()) {
                            addNode = false;
                            auto nodeId = g.addNode(); ph.addNode();
                            nodesPos[nodeId] = worldPos;
                            resetAlgo();
                            updateStaticLayer();
                        }
                        
                        bool onTop = false;

                        // Check if the mouse is on top of a node
                        for (auto& [it, neighbors] : g) {
                            if ((worldPos - nodesPos[it]).length() <= (node_size + 2)) {
                                renderer.fillCircle(nodesPos[it], node_size + 1, color::Red);
                                onTop = true;
                                // Pick a node or connect with other
                                if (input.buttonReleased(button_t::Left)) {
                                    if (chosenNode.has_value()) {
                                        if (chosenNode.value() != it) {
                                            g.connect(chosenNode.value(), it, 1.0);
                                            ph.connect(chosenNode.value(), it, 1.0);
                                            updateStaticLayer();
                                            weightsHelper.clear();
                                            weightsHelper = g.getNeighbors(chosenNode.value());
                                        }
                                    }
                                    else {
                                        chosenNode = it;
                                        weightsHelper.clear();
                                        weightsHelper = g.getNeighbors(chosenNode.value());
                                    }
                                }
                                break;
                            }
                        }

                        // Move a node in the screen
                        if (chosenNode.has_value()) {
                            if (input.keyReleased(key_t::M)) {
                                nodesPos[chosenNode.value()] = worldPos;
                                updateStaticLayer();
                            }
                        }
                        else {
                            // Draw 'cursor' (empty circle)
                            if (!onTop)
                                renderer.drawCircle(worldPos, node_size, color::White);
                        }
                    }
                }
            }

            // Edit mode
            if (editMode) {
                // Remove selection
                if (input.keyReleased(key_t::Escape))
                    chosenNode = {};

                // Render the edges of the current node and the node itseld
                if (chosenNode.has_value()) {
                    for (auto& [neigh, weight] : g.getNeighbors(chosenNode.value())) {
                        renderer.drawLine(nodesPos[chosenNode.value()], nodesPos[neigh], color::CornflowerBlue);
                    }

                    renderer.fillCircle(nodesPos[chosenNode.value()], node_size + 1, color::DarkCobaltBlue);
                }
            }
            // ALGORITH!!!
            else if (algoStep && g.size() > 0) {
                std::set<uint64_t> alreadyUpdated;

                ++nAlgoStep;
                accTime += delta;
                if (!autoRun) 
                    algoStep = false;

                // Reset the previous state of the ants
                // And randomly choose the starting node of the new path
                for (auto &a : ants) {
                    a.reset();
                    auto visited = random::i_zero_intMax() % g.size();
                    a.visitNode(visited);
                }

                // Iterate until the paths of avery ant is complete
                for (int itNodes = 1; itNodes < g.size(); ++itNodes) {
                    // For every ant choose the next node in the path based
                    // On the probabilities and pheromones trails
                    int antsMoving = 0;
                    for (auto& ant : ants) {
                        if (ant.stuck) continue;
                        
                        auto currNode = ant.currNode();

                        std::vector<std::pair<double, graph_t::node_id>> probs;
                        probs.reserve(ant.graph.size());
                        double probTotal = 0.0;

                        // Calculate probabilities of choosing a node
                        for (auto& [neighId, neighWeight] : ant.graph.getNeighbors(currNode)) {
                            if (! ant.visited[neighId]) {
                                auto prob = std::pow(ph.getWeigth(currNode, neighId), alpha) * std::pow(1.0 / neighWeight, beta);
                                probs.push_back({ prob, neighId });
                                probTotal += probs.back().first;
                                assert(!std::isnan(prob) && !std::isinf(prob) && std::abs(neighWeight) > math::constants::EPS);
                            }
                        }

                        // The ant got stuck!
                        if (probs.size() == 0 || (std::abs(probTotal) <= math::constants::EPS)) {
                            ant.stuck = true;
                            continue;
                        }

                        ++antsMoving;

                        // Calculate the prefix sum array of probabilities
                        probs[0].first /= probTotal;
                        for (size_t i = 1; i < probs.size(); ++i) {
                            probs[i].first /= probTotal;
                            probs[i].first += probs[i - 1].first;
                        }

                        // Choose any random node based on the probabilties
                        // in the prefix sum array
                        auto choice = random::f_zero_to_one();
                        graph_t::node_id chosen = -1;

                        if (choice < probs.front().first) {
                            chosen = probs.front().second;
                        }
                        else {
                            for (graph_t::node_id itNeighs = 1; itNeighs < probs.size(); ++itNeighs) {
                                if (choice > probs[itNeighs - 1].first && choice <= probs[itNeighs].first) {
                                    chosen = probs[itNeighs].second;
                                    break;
                                }
                            }
                        }

                        if (chosen == -1) {
                            throw std::runtime_error("How this happened?");
                        }

                        // The ant visit the node
                        ant.visitNode(chosen);
                    }

                    if (antsMoving == 0) {
                        logger::critical("What?? there are no paths?");
                        break;
                    }
                }

                // Update pheromones
                graph_t newPh;

                // Create the new pheromone graph
                for (graph_t::node_id itNode = 0; itNode < ph.size(); ++itNode) {
                    newPh.addNode();
                }

                // Set the new pheromones graph to 0 and 
                // 'vanish' the old pheromone graph
                alreadyUpdated.clear();
                for (auto& [itNode, neighbors] : ph) {
                    for (auto& [neigh, weight] : neighbors) {
                        auto key = ((uint64_t(std::min(itNode, neigh)) << 32) | uint64_t(std::max(itNode, neigh)));
                        if (alreadyUpdated.find(key) == alreadyUpdated.end()) {
                            newPh.connect(itNode, neigh, 0);
                            ph.connect(itNode, neigh, (1.0 - rho) * weight);
                        }
                    }
                }

                // For every ant update the pheromone graph
                // Based on the total length of the chosen path
                aco::ant* chosenPath = &(ants.front());
                double minPath = ants.front().distanceTraveled();

                for (auto& ant : ants) {
                    auto pathLength = ant.distanceTraveled();
                    auto pheromoneUpdate = 1.0 / pathLength;
                    
                    // Save the best path
                    if (pathLength < minPath) {
                        minPath = pathLength;
                        chosenPath = &ant;
                    }

                    newPh.connect(ant.path.front(), ant.path.back(), newPh.getWeigth(ant.path.front(), ant.path.back()) + pheromoneUpdate);
                    
                    auto lIt = ant.path.begin();
                    for (auto it = ++(ant.path.begin()); it != ant.path.end(); ++it) {
                        newPh.connect(*lIt, *it, newPh.getWeigth(*lIt, *it) + pheromoneUpdate);
                        lIt = it;
                    }
                }

                // If the best path found on this iteration
                // is better than the already found update it
                if (minPath < bestPathSoFarLength) {
                    bestPathSoFarLength = minPath;
                    bestPathSoFar = chosenPath->path;
                }

                actBestPath = minPath;

                maxPheromone = std::numeric_limits<double>::min();

                // Update the pheromone graph, sum the 'old' updated pheromones
                // And the new calculated pheromones
                alreadyUpdated.clear();
                for (auto& [itNode, neighbors] : ph) {
                    for (auto& [neigh, weight] : neighbors) {
                        auto key = ((uint64_t(std::min(itNode, neigh)) << 32) | uint64_t(std::max(itNode, neigh)));
                        if (alreadyUpdated.find(key) == alreadyUpdated.end()) {
                            ph.connect(itNode, neigh, weight + newPh.getWeigth(itNode, neigh));
                            maxPheromone = std::max(maxPheromone, weight);
                        }
                    }
                }

                // Re-render the graph
                updateStaticLayer();
            }
        }

        // IMGUI STUFF!!
        renderer.setTargetedLayer(guiLayer);

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        renderer.clear(color::Transparent);
        
        ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Appearing);
        ImGui::SetNextWindowSize({220, 640});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoDecoration | 
                                        ImGuiWindowFlags_NoMove | 
                                        ImGuiWindowFlags_NoResize | 
                                        ImGuiWindowFlags_NoSavedSettings | 
                                        ImGuiWindowFlags_NoTitleBar);

        // Reset zooming and panning
        if (ImGui::Button("Reset viewport")) {
            renderer.setTargetedLayer(movingLayer);
            zoom = 1.0;
            renderer.scaleLayer({zoom, zoom}, mousePos);
            renderer.offsetLayer(-renderer.getLayerOffset() + math::vec2df{220, 0});
            renderer.targetDefaultLayer();
            renderer.scaleLayer({zoom, zoom}, mousePos);
            renderer.offsetLayer(-renderer.getLayerOffset() + math::vec2df{220, 0});
            renderer.setTargetedLayer(guiLayer);
        }

        // Enable edition mode
        if (ImGui::Checkbox("Edition mode", &editMode)) {
            autoRun = false;
            updateStaticLayer();
            if (!editMode)
                chosenNode = {};
        }

        ImGui::Separator();
        ImGui::Spacing();

        // General graph info
        ImGui::Text("Graph size: %d", g.size());
        ImGui::Text("Number of edges: %d", g.edgesCount());

        ImGui::Separator();
        ImGui::Spacing();

        // Show edit mode panel
        if (editMode) {
            ImGui::BeginChild("Edges", {205, 300}, true);

            if (chosenNode.has_value()) {
                bool  update = false;
                graph_t::node_id toDelete;
                for (auto& [neigh, dist] : weightsHelper) {
                    std::string t = fmt::format("D {}", neigh);
                    if (ImGui::Button(t.c_str(), {16, 20})) {
                        g.disconnect(chosenNode.value(), neigh);
                        ph.disconnect(chosenNode.value(), neigh);
                        update = true;
                        break;
                    }
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(100);
                    std::string text = fmt::format("to {}", neigh);
                    if (ImGui::InputDouble(text.c_str(), &(dist))) {
                        g.connect(chosenNode.value(), neigh, dist);
                    }
                }

                if (update) {
                    weightsHelper.clear();
                    weightsHelper = g.getNeighbors(chosenNode.value());
                    resetAlgo();
                    updateStaticLayer();
                }
            }

            ImGui::EndChild();
        }
        else {
            // Algorithm run or reset buttons
            if (ImGui::Checkbox("Autorun", &autoRun)) {
                algoStep = autoRun;
                updateStaticLayer();
            }

            if (ImGui::Button("Algorith Step")) {
                algoStep = true;
            }

            if (ImGui::Button("Reset algorithm")) {
                resetAlgo();
                updateStaticLayer();
            }

            ImGui::Separator();
            ImGui::Spacing();

            // Algorithm info
            ImGui::Text("Algorithm step: %d", nAlgoStep);
            if (bestPathSoFarLength == graph_t::inf) {
                ImGui::Text("MinPathLength: inf");
            }
            else {
                ImGui::Text("MinPathLength: %.3f", bestPathSoFarLength);
            }

            if (actBestPath == graph_t::inf) {
                ImGui::Text("ActMinPathLength: inf");
            }
            else {
                ImGui::Text("ActMinPathLength: %.3f", actBestPath);
            }
            ImGui::Text("Time running: %.3f", accTime);

            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::InputInt("Nº Ants", &nAnts)) {
                resetAlgo();
                updateStaticLayer();
            }

            ImGui::Separator();
            ImGui::Spacing();

            // Algorithm variables and stuff
            if (ImGui::InputDouble("alpha", &alpha)) {
                updateStaticLayer();
            }
            if (ImGui::InputDouble("beta", &beta)) {
                updateStaticLayer();
            }
            if (ImGui::InputDouble("rho", &rho)) {
                updateStaticLayer();
            }

            ImGui::Separator();
            ImGui::Spacing();
        }

        ImGui::Separator();
        ImGui::Spacing();

        // Other random options
        if (ImGui::Checkbox("Show all edges", &showAllEdges)) {
            updateStaticLayer();
        }

        if (ImGui::Button("Random case")) {
            numberOfNodes = 10;
            modalOpen = true;
            ImGui::OpenPopup("RandomCaseModal");

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        }

        if (ImGui::Button("Save data")) {
            modalOpen = true;
            ImGui::OpenPopup("SaveData");
        }

        if (ImGui::Button("Load data")) {
            modalOpen = true;
            radioGroup = 2;
            ImGui::OpenPopup("LoadData");
        }

        if (ImGui::BeginPopupModal("RandomCaseModal", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Choose the number of nodes for the random case\n\n");
            ImGui::Separator();

            ImGui::SliderInt("Number of nodes", &numberOfNodes, 10, 150);
            
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
                modalOpen = false;
                g.reset();
                ph.reset();
                nodesPos.clear();
                resetAlgo();

                for (int i = 0; i < numberOfNodes; ++i) {
                    auto nodeId = g.addNode();
                    ph.addNode();
                    nodesPos[nodeId] = math::vec2df{
                        10.0f + static_cast<float>(random::i_zero_intMax() % 620),
                        10.0f + static_cast<float>(random::i_zero_intMax() % 620)
                    };
                    for (graph_t::node_id it = 0; it < nodeId; ++it) {
                        g.connect(nodeId, it, (nodesPos[nodeId] - nodesPos[it]).length());
                        ph.connect(nodeId, it, 1.0);
                    }
                }

                updateStaticLayer();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
                modalOpen = false;
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("SaveData", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Enter the name of the file to save the current data in json format\n\n");
            ImGui::Separator();

            ImGui::InputText("Filename", &filename);
            
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                nlohmann::json saveData;

                std::vector<std::vector<double>> graphMatrix(g.size(), std::vector<double>(g.size(), 0));

                for (graph_t::node_id it = 0; it < g.size(); ++it) {
                    saveData["nodesCoords"][it]["x"] = nodesPos[it].x;
                    saveData["nodesCoords"][it]["y"] = nodesPos[it].y;
                    for (graph_t::node_id jt = 0; jt < g.size(); ++jt) {
                        graphMatrix[it][jt] = g.getWeigth(it, jt);
                    }
                }

                saveData["graph"] = graphMatrix;

                for (graph_t::node_id it = 0; it < g.size(); ++it) {
                    for (graph_t::node_id jt = 0; jt < g.size(); ++jt) {
                        graphMatrix[it][jt] = ph.getWeigth(it, jt);
                    }
                }

                saveData["pheromonesMatrix"] = graphMatrix;

                saveData["bestPathSoFarLength"] = bestPathSoFarLength;

                saveData["bestPathSoFar"] = bestPathSoFar;
                saveData["number_iterations"] = nAlgoStep;
                
                saveData["algorithmParameters"]["alpha"] = alpha;
                saveData["algorithmParameters"]["beta"] = beta;
                saveData["algorithmParameters"]["rho"] = rho;
                saveData["algorithmParameters"]["nAnts"] = ants.size();

                std::ofstream saveFile(filename);

                if (saveFile.is_open()) {
                    saveFile << saveData.dump() << std::endl;
                }
                else {
                    logger::error("Couldn't save the data");
                }

                ImGui::CloseCurrentPopup();
                modalOpen = false;
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
                modalOpen = false;
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("LoadData", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Enter the name of the file to load\n\n");
            ImGui::Separator();

            ImGui::InputText("Filename", &filename);

            ImGui::BeginGroup();
            ImGui::RadioButton("Json format", &radioGroup, 1);
            ImGui::RadioButton("Weights matrix", &radioGroup, 2);
            ImGui::EndGroup();
            
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                if (radioGroup == 1) {
                    std::ifstream fileInput(filename);
                    if (fileInput.is_open()) {
                        nlohmann::json inputData;
                        fileInput >> inputData;

                        g.reset();
                        ph.reset();

                        for (graph_t::node_id it = 0; it < inputData["graph"].size(); ++it) {
                            g.addNode();
                            ph.addNode();
                        }

                        auto numOfNodesPerRow = static_cast<int>(std::ceil(std::sqrt(inputData["graph"].size())));

                        for (graph_t::node_id it = 0; it < inputData["graph"].size(); ++it) {
                            if (inputData.contains("nodesCoords")) {
                                nodesPos[it] = { 
                                    inputData["nodesCoords"][it]["x"],
                                    inputData["nodesCoords"][it]["y"]
                                };
                            }
                            else {
                                nodesPos[it] = math::vec2df{
                                    30.0f + static_cast<float>(static_cast<float>(it % numOfNodesPerRow) * (600.0 / numOfNodesPerRow)),
                                    30.0f + static_cast<float>(static_cast<float>(it / numOfNodesPerRow) * (600.0 / numOfNodesPerRow))
                                };
                            }
                            for (graph_t::node_id jt = 0; jt < inputData["graph"][it].size(); ++jt) {
                                if (it == jt)
                                    continue;
                                g.connect(it, jt, inputData["graph"][it][jt]);
                                ph.connect(it, jt, 1.0);
                            }
                        }

                        if (inputData.contains("algorithmParameters")) {
                            if (inputData.contains("nAnts"))
                                nAnts = inputData["algorithmParameters"]["nAnts"];
                            if (inputData.contains("alpha"))
                                alpha = inputData["algorithmParameters"]["alpha"];
                            if (inputData.contains("beta"))
                                beta = inputData["algorithmParameters"]["beta"];
                            if (inputData.contains("rho"))
                                rho = inputData["algorithmParameters"]["rho"];
                        }

                        resetAlgo();
                        updateStaticLayer();
                    }
                    else {
                        logger::error("Couldn't open the file");
                    }
                }
                else if (radioGroup == 2) {
                    std::ifstream fileInput(filename);
                    if (fileInput.is_open()) {
                        int gSize;
                        double weight;

                        fileInput >> gSize;

                        g.reset();
                        ph.reset();
                        nodesPos.clear();
                        weightsHelper.clear();

                        for (int i = 0; i < gSize; ++i) {
                            g.addNode();
                            ph.addNode();
                        }

                        auto numOfNodesPerRow = static_cast<int>(std::ceil(std::sqrt(gSize)));

                        for (int i = 0; i < gSize; ++i) {
                            nodesPos[i] = math::vec2df{
                                30.0f + static_cast<float>(static_cast<float>(i % numOfNodesPerRow) * (600.0 / numOfNodesPerRow)),
                                30.0f + static_cast<float>(static_cast<float>(i / numOfNodesPerRow) * (600.0 / numOfNodesPerRow))
                            };
                            for (int j = 0; j < gSize; ++j) {
                                fileInput >> weight;
                                if (i == j)
                                    continue;
                                g.connect(i, j, weight);
                                ph.connect(i, j, 1.0);
                            }
                        }                    

                        resetAlgo();
                        updateStaticLayer();
                    }
                    else {
                        logger::error("Couldn't open the file");
                    }
                }
                else {
                    logger::error("What??");
                }
                
                ImGui::CloseCurrentPopup();
                modalOpen = false;
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
                modalOpen = false;
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();
        ImGui::Spacing();

        // Maybe i should ask for confirmation before?
        if (ImGui::Button("Reset Graph")) {
            g.reset();
            ph.reset();
            nodesPos.clear();
            renderer.targetDefaultLayer();
            renderer.clear(color::OffBlack);
            renderer.setTargetedLayer(guiLayer);
            chosenNode = {};
            resetAlgo();
        }

        // Au revoir
        if (ImGui::Button("Exit")) {
            return false;
        }

        ImGui::PopStyleVar();
        ImGui::End();

        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

        return true;
    }

    bool AntVisualization::onExit() {
        // CLEAAAN STUFF
        ImGui_ImplSDLRenderer_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        return true;
    }

    void AntVisualization::updateStaticLayer() {
        renderer.targetDefaultLayer();
        renderer.clear(color::OffBlack);
        if (bestPathSoFar.size() > 1) {
            auto lIt = bestPathSoFar.back();
            for (auto& e : bestPathSoFar) {
                renderer.drawThickLine(nodesPos[lIt], nodesPos[e], 3, color::DarkRed);
                lIt = e;
            }
        }

        if (showAllEdges) {
            std::set<uint64_t> alreadyDrawed;
            for (auto& [it, neighbors] : ph) {
                auto& nodePos = nodesPos[it];
                for (auto& [neigh, weight] : neighbors) {
                    auto key = ((uint64_t(std::min(it, neigh)) << 32) | uint64_t(std::max(it, neigh)));
                    if (alreadyDrawed.find(key) == alreadyDrawed.end()) {
                        auto edgeColor = color::OffWhite;
                        if (editMode)
                            edgeColor.a = 255;
                        else edgeColor.a = ((weight / (maxPheromone)) * 255.0);
                        renderer.drawLine(nodePos, nodesPos[neigh], edgeColor);
                        alreadyDrawed.insert(key);
                    }
                }
            }
        }

        for (auto& [it, _] : ph) {
            auto &nodePos = nodesPos[it];
            renderer.fillCircle(nodePos, node_size, color::White);
            renderer.drawString(nodePos - math::vec2df{static_cast<float>(node_size) * 0.5f, static_cast<float>(node_size) * 2.0f}, fmt::to_string(it), color::CornflowerBlue);
        }
        renderer.setTargetedLayer(movingLayer);
    }

    void AntVisualization::resetAlgo() {
        // RESET EVERYTHING!
        nAlgoStep = 0;
        maxPheromone = 1.0;
        bestPathSoFar.clear();
        bestPathSoFarLength = std::numeric_limits<double>::max();
        actBestPath = std::numeric_limits<double>::max();
        accTime = 0.0f;
        std::set<uint64_t> alreadyUpdated;

        for (auto& [it, neighbors] : g) {
            for (auto& [neigh, weight] : neighbors) {
                auto key = ((uint64_t(std::min(it, neigh)) << 32) | uint64_t(std::max(it, neigh)));
                if (alreadyUpdated.find(key) == alreadyUpdated.end()) {
                    ph.connect(it, neigh, 1.0);
                }
            }
        }
        
        ants.clear();
        for (size_t i = 0; i < nAnts; ++i) {
            ants.emplace_back(g);
        }
        nAnts = ants.size();
    }

}