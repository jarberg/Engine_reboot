#pragma once
#include <functional>
#include <core/singleton.h>
#include <string>
#include <json.hpp>
#include <core/events.h>
#include <map>
#include <remote_client.h>
#include <core/world.h>
#include <core/material.h>

using json = nlohmann::json;

class WsClient : public Singleton<WsClient> {
    friend class Singleton<WsClient>;
public:
    EventDispatcher* msgDispatcher;
    int id;
    inline static std::map<int, RemoteClient*> rEvents{};

    WsClient() {
        rEvents = std::map<int, RemoteClient*>();

		msgDispatcher = new EventDispatcher();
        onOpen  = [this]() { std::puts("WS open"); send(R"({"type":"hello","from":"wasm"})"); };
        onClose = []() { std::puts("WS closed"); };
        onError = [](const std::string& e) { std::printf("WS error: %s\n", e.c_str()); };

        connect("ws://localhost:8080");

        onMessage = [this](const std::string& s) { pooler(s); };
    }

    bool connect(const std::string& url);
    void send(const std::string& text);
    void close();
    void poll(); 

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
                            auto rDispatcher = rEvents.find(id);
                            if (id == this->id) {
                                msgDispatcher->Dispatch<clientMSGEvent>(std::make_shared<clientMSGEvent>(e.dump()));
                            }
                            else {
                                msgDispatcher->Dispatch(std::make_shared<RClientMSGEvent>(e.dump(), id));
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

        StaticMeshComponent& meshComp = world->add_component<StaticMeshComponent>(remote, 3);
        meshComp.material = Material::defaultMat;

        auto rDispatcher = rEvents.find(otherId);

        if (rDispatcher == rEvents.end()) {
			rEvents[otherId] = new RemoteClient(otherId, remote);
			rDispatcher = rEvents.find(otherId);
            
            msgDispatcher->Subscribe<RClientMSGEvent>(
                [](std::shared_ptr<RClientMSGEvent> ev) {
                    rEvents[ev->id]->events.Dispatch(std::make_shared<RClientMSGEvent>(ev->msg, ev->id));
                }
			);
        }
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
            if (targetId == this->id) continue; 

            Entity ent = test->create_entity("remote_" + std::to_string(targetId));

            test->add_component<PositionComponent>(ent);
            test->add_component<RotationComponent>(ent);

            auto& mesh = test->add_component<StaticMeshComponent>(ent, 1);
            mesh.material = Material::defaultMat;

            if (rEvents.find(targetId) == rEvents.end()) {
                rEvents[targetId] = new RemoteClient(targetId, ent);

                msgDispatcher->Subscribe<RClientMSGEvent>(
                    [](std::shared_ptr<RClientMSGEvent> ev) {
                        WsClient::rEvents[ev->id]->events.Dispatch(std::make_shared<RClientMSGEvent>(ev->msg, ev->id));
                    }
                );
            }
        }
    }
};
