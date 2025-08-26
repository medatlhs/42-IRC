#pragma once

#include <string>
#include <map>
#include "Client.hpp"

class Client;

class Channel {
private:
    std::string _name;
    bool        _inviteOnly;
    int         _membersLimit;
    int         _membersCount;
    // 
    std::map<std::string, Client*> _members;
    std::map<std::string, Client*> _invited;
    std::map<std::string, Client*> _bannedMembers;

public:
    Channel(const std::string& name);

    std::string& getChannelName();
    bool         isPrivateChannel();
    int          getLimit();
    int          getMembersCount();

    // Membership Checks
    bool isUserInChannel(Client* client);
    bool isUserInvited(Client* client);
    bool isUserBanned(Client* client);

    // Member Management
    void addMember(Client *client);
    void removeMember(Client *client);
    void removeInvitation(Client *client);
    void inviteUser(Client *client);
    void banUser(Client *client);
    void unbanUser(Client *client);

    void broadcast(Client *sender, const std::string& message, bool isNewJoin);
};