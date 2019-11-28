#include <iostream>
#include <thread>
#include <chrono>
#include <functional>

#include "mavsdk/mavsdk.h"
#include "mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h"

using namespace std::chrono;

bool callback(mavlink_message_t& msg)
{
    std::cout << "Received MAVLink message. It's msgid: " << static_cast<uint32_t>(msg.msgid) << std::endl;

    if(msg.msgid == MAVLINK_MSG_ID_PING)
    {
        std::cout << "Received PING response!" << std::endl;
    }

    return true;
}

int main()
{
    mavsdk::Mavsdk mavsdk;
    bool system_discovered = false;
    uint64_t gcs_system_id = 0;

    //Connect to QGC
    auto result = mavsdk.setup_udp_remote(std::string("127.0.0.1"), 14550);
    if(result != mavsdk::ConnectionResult::SUCCESS)
    {
        std::cout << "Error connecting to QGC!" << std::endl;
        return -1;
    }

    mavsdk.register_on_discover([&system_discovered, &gcs_system_id](uint64_t uuid) {
        std::cout << "Discovered system with UUID: " << uuid << std::endl;
        gcs_system_id = uuid;
        system_discovered = true;
    });

    //Wait for system discover
    std::this_thread::sleep_for(std::chrono::seconds(2));

    if(system_discovered != true)
    {
        std::cout << "System not discovered!" << std::endl;
        return -2;
    }

    //Get system, use MAVLink passthrough
    auto& system = mavsdk.system(gcs_system_id);
    auto passthrough = std::make_shared<mavsdk::MavlinkPassthrough>(system);

    //Subscribe for ALL MAVLink incomming traffic
    std::function<bool(mavlink_message_t&)> intercept = callback;
    passthrough->intercept_incoming_messages_async(intercept);

    //Send PING request
    auto timestamp = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
    mavlink_message_t msg;
    mavlink_msg_ping_pack(1, 1, &msg, static_cast<uint64_t>(timestamp.count()), 0, 0, 0);
    passthrough->send_message(msg);

    //Wait for response
    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}
