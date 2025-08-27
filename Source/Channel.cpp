#include "../Includes/Channel.hpp"
#include "../Includes/Client.hpp"

Channel::Channel(const std::string &name)
    : _name(name), _inviteOnly(false), _membersLimit(0), _membersCount(0) { }

bool Channel::isPrivateChannel() {
    return _inviteOnly;
}

std::string& Channel::getKey() {
    return _key;
}

void Channel::setKey(const std::string &key) {
    _key = key;
}

int Channel::getLimit() {
    return _membersLimit;
}

int Channel::getMembersCount() {
    return _membersCount;
}

std::string& Channel::getChannelName() {
    return _name;
}


// Membership Checks
bool Channel::isUserInChannel(Client *client) {
    return _members.find(client->getNickName()) != _members.end();
}

bool Channel::isUserInvited(Client *client) {
    return _invited.find(client->getNickName()) != _invited.end();
}

bool Channel::isUserBanned(Client *client) {
    return _bannedMembers.find(client->getNickName()) != _bannedMembers.end();
}

// Member Management
void Channel::addMember(Client *client) {
    std::string nick = client->getNickName();
    if (_members.find(nick) == _members.end()) {
        _members[nick] = client;
        _membersCount++;
    }
}

void Channel::removeMember(Client *client) {
    std::string nick = client->getNickName();
    std::map<std::string, Client*>::iterator it = _members.find(nick);
    if (it != _members.end()) {
        _members.erase(it);
        _membersCount--;
        if (_membersCount < 0) _membersCount = 0;
    }
}

void Channel::removeInvitation(Client *client) {
    _invited.erase(client->getNickName());
}

void Channel::inviteUser(Client *client) {
    _invited[client->getNickName()] = client;
}

void Channel::banUser(Client *client) {
    std::string nick = client->getNickName();
    _bannedMembers[nick] = client;
    removeMember(client);
    _invited.erase(nick);
}

void Channel::unbanUser(Client *client) {
    _bannedMembers.erase(client->getNickName());
}

void Channel::broadcast(Client *sender, const std::string &message, bool newJoinFlag) {
    std::map<std::string, Client*>::iterator it;
    for (it = _members.begin(); it != _members.end(); ++it) {
        if (!newJoinFlag && it->first == sender->getNickName()) // 
            continue;
        it->second->setQueueBuffer(message);
        it->second->_dataWaiting = true;
    }
}
