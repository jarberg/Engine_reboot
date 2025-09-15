#pragma once
#include <functional>
#include <core/singleton.h>
#include <string>
#include <json.hpp>
#include <core/events.h>
#include <core/world.h>
#include <core/material.h>

using json = nlohmann::json;

class WsClient : public Singleton<WsClient> {
    friend class Singleton<WsClient>;
public:
    EventDispatcher* msgDispatcher;
    int id;

    WsClient() {

		msgDispatcher = new EventDispatcher();
        onOpen = [this]() { std::puts("WS open"); send(R"({"type":"hello","from":"wasm"})"); };
        onClose = []() { std::puts("WS closed"); };
        onError = [](const std::string& e) { std::printf("WS error: %s\n", e.c_str()); };
        // Dev: plain ws over http. In prod, use wss over https.
        connect("ws://localhost:8080");

        onMessage = [this](const std::string& s) { pooler(s); };
     
    }

    bool connect(const std::string& url);
    void send(const std::string& text);
    void close();
    void poll(); // no-op on web

    std::function<void()> onOpen;
    std::function<void()> onClose;
    std::function<void(const std::string&)> onMessage;
    std::function<void(const std::string&)> onError;



    void pooler(const std::string& m) {
        if (!msgDispatcher) return;

        try {
            json j = json::parse(m);

            if (!j.is_object()) {
                // Not an object; log and bail
                fprintf(stderr, "Unexpected JSON (not object): %s\n", m.c_str());
                return;
            }

            const std::string type = j.value("type", "");
            if (type.empty()) {
                fprintf(stderr, "Missing 'type' in: %s\n", m.c_str());
                return;
            }

            if (type == "S2C_Welcome") {
                welcome(j, m);
            }
            else if (type == "S2C_ClientJoined") {  
                clientJoined(j, m);
            }
            else if (type == "S2C_ClientLeft") {
                int otherId = j.value("clientId", -1);
            }
            else if (type == "S2C_StateSnapshot") {
                int tick = j.value("tick", 0);

                if (j.contains("entities") && !j["entities"].is_null()) {
  
                    json &entities = j["entities"];
                    int id = -1;
                    for (const auto& e : entities) {
                        
                        for (auto& [key, val] : entities.items()){
                            id = std::stoi(key);

                            // route message based on whose entity it is
                            if (id == this->id) {
                                msgDispatcher->Dispatch<clientMSGEvent>(std::make_shared<clientMSGEvent>(e.dump()));
                            }
                            else {
                                msgDispatcher->Dispatch<RClientMSGEvent>(std::make_shared<RClientMSGEvent>(e.dump(), id));
                            }

                        }
                    }
                }
                else {
                    fprintf(stderr, "Bad 'entities' array in: %s\n", m.c_str());
                }
            }
            else {
                fprintf(stderr, "Unknown type '%s' in: %s\n", type.c_str(), m.c_str());
            }
        }
        catch (const nlohmann::json::type_error& e) {
            fprintf(stderr, "JSON type_error: %s\nRaw: %s\n", e.what(), m.c_str());
        }
        catch (const std::exception& e) {
            fprintf(stderr, "JSON error: %s\nRaw: %s\n", e.what(), m.c_str());
        }
    }

    void clientJoined(json j, std::string m) {

        if (!j.contains("clientId") || !j["clientId"].is_number_integer()) {
            fprintf(stderr, "Bad 'clientId' in: %s\n", m.c_str());
            return;
        }
        int otherId = j.value("clientId", -1);
        World* world = World::current();

		Entity remote = world->create_entity("test2");
        world->add_component<PositionComponent>(remote);
        world->add_component<RotationComponent>(remote);
        auto& netcomp = world->add_component<NetworkComponent>(remote, remote, otherId);
        StaticMeshComponent& meshComp = world->add_component<StaticMeshComponent>(remote, 2);
        meshComp.material = Material::defaultMat;



        msgDispatcher->Subscribe<RClientMSGEvent>([netcomp, remote](std::shared_ptr<RClientMSGEvent> ev) {
            World* test = World::current();
            PositionComponent& comp = test->get_component<PositionComponent>(remote);

            std::string const& s = ev->msg;
            json data = json::parse(s);

            if (data.contains("pos") && data["pos"].is_array()) {
                const auto& arr = data["pos"];
                comp.x = arr.at(0).get<float>();
                comp.y = arr.at(1).get<float>();
                comp.z = arr.at(2).get<float>();
                std::cout << "key: pos, value: " << arr << '\n';
            }
        });
    }

    void welcome(json j, std::string m) {

        if (!j.contains("clientId") || !j["clientId"].is_number_integer()) {
            fprintf(stderr, "Bad 'clientId' in: %s\n", m.c_str());
            return;
        }
        int clientId = j["clientId"].get<int>();
        double serverTime = j.value("serverTime", 0.0);
        int tick = j.value("tick", 0);
        this->id = clientId;
        World* test = World::current();

        if (auto view = test->get_registry().view<CharacterComponent>()) {

            for (auto [e, c] : view.each()) {
                Entity Ee(e, test);
                auto netcomp = NetworkComponent{ Ee,  clientId };
                test->add_component<NetworkComponent>(Ee, std::move(netcomp));

                msgDispatcher->Subscribe<clientMSGEvent>([ netcomp](std::shared_ptr<clientMSGEvent> ev) {
                        std::cout << ev->msg << std::endl;
                });
            }
        }
        if (!j.contains("entities")){
            fprintf(stderr, "Bad 'entities' in: %s\n", m.c_str());
            return;
        }
        auto entities = j["entities"]; 

        for (const auto& [key, val] : entities.items()) {
            int targetId = std::stoi(key);
            if (targetId == this->id) continue; // don't spawn a remote for myself

            Entity ent = test->create_entity("remote_" + std::to_string(targetId));
            test->add_component<PositionComponent>(ent);
            test->add_component<RotationComponent>(ent);
            test->add_component<NetworkComponent>(ent, ent, targetId);
            auto& mesh = test->add_component<StaticMeshComponent>(ent, 2);
            mesh.material = Material::defaultMat;

            // Capture the *plain int* and the Entity handle by value
            msgDispatcher->Subscribe<RClientMSGEvent>(
                [ent](std::shared_ptr<RClientMSGEvent> ev) {
                    // Process ONLY messages from this remote client
                    World* w = World::current();
                    auto& netcomp = w->get_component<NetworkComponent>(ent);
                    if (ev->id != netcomp.clientId) return;


                    auto& pos = w->get_component<PositionComponent>(ent);
                    

                    try {
                        const std::string& s = ev->msg;
                        json data = json::parse(s);

                        if (data.contains("pos") && data["pos"].is_array() && data["pos"].size() >= 3) {
                            const auto& a = data["pos"];
                            pos.x = a.at(0).get<float>();
                            pos.y = a.at(1).get<float>();
                            pos.z = a.at(2).get<float>();
                        }
                    }
                    catch (const std::exception& ex) {
                        std::cerr << "RClientMSGEvent parse error: " << ex.what() << '\n';
                    }
                }
            );
        }
    }
};
