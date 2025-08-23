#pragma once

#include <string>
#include <map>
#include <set>
#include "Client.hpp"

class Channel {
private:
    std::string _name;
    bool    _inviteOnly;
    int     _membersLimit;
    int     _membersCount;

    std::map<std::string, Client *> _members;    
    std::map<std::string, Client *> _invited;           
    std::map<std::string, Client *> _bannedMembers;       

public:

    Channel(const std::string &name);

    // Getters
    std::string getName() { return _name; }
    bool isPrivateChannel();
    int getLimit();
    int getMembersCount();

    // Membership checks rak tm 
    bool isUserInChannel(Client *client);
    bool isUserInvited(Client *client);
    bool isUserBanned(Client *client);

    void addRegularMember(Client *client);
    void inviteUser(Client *client);
    void banUser(Client *client);
    void removeUser(Client *client);

    // Broadcast
    void broadcast(Client *sender, const std::string &message, bool newJoinFlag);
    
};

