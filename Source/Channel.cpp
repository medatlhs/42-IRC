#include "../Includes/Channel.hpp"
#include "../Includes/Client.hpp"
#include "../Includes/Channel.hpp"

Channel::Channel(const std::string &name) 
    : _name(name), _inviteOnly(false), _membersLimit(0), _membersCount(0) { }

bool Channel::isUserInChannel(Client *client) {
    return _members.find(client->getNickName()) != _members.end();
}

bool Channel::isUserInvited(Client *client ) {
    return _invited.find(client->getNickName()) != _invited.end();
}
bool Channel::isUserBanned(Client *client) {
    return _bannedMembers.find(client->getNickName()) != _bannedMembers.end();
}

bool Channel::isPrivateChannel(void) { return _inviteOnly; }

int Channel::getLimit() { return _membersLimit; }

int Channel::getMembersCount() { return _membersCount; }

void Channel::addRegularMember(Client *client) {
    _members.insert(std::make_pair(client->getNickName(), client));
    _membersCount++;
}

void Channel::broadcast(Client *sender, const std::string &message, bool newJoinFlag) {
    std::map<std::string, Client *>::iterator it = _members.begin();
    while (it != _members.end()) {
        if (!newJoinFlag && it->first == sender->getNickName()) { it++; continue; }
        it->second->setQueueBuffer(message);
        it->second->_dataWaiting = true;
        it++;
    }
}
