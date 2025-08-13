#pragma once
#include "Client.hpp"
#include <string>
#include <vector>

class Channel {
    private:
        std::string _channelName;
        std::vector<Client*> _channelMembers;
    public:
        Channel();
};