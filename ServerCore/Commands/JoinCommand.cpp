# include "../../Includes/replyCodes.hpp"
# include "../../Includes/IrcServer.hpp"
# include "../../Includes/Channel.hpp"
# include "../../Includes/Client.hpp"

void IrcServer::handleCreateChannel(Client* client, const std::string& channelName) {
    Channel* newChannel = new Channel(channelName);
    newChannel->addMember(client);
    client->addChannelMembership(newChannel);
    _allChannels[channelName] = newChannel;
    std::string fullMessage = ":" + client->genHostMask() + " JOIN :" + channelName + "\r\n";
    newChannel->broadcast(client, fullMessage, true); // TODO RPL_TOPIC RPL_NAMREPLY
}

void IrcServer::channelManager(Client *client, std::vector<std::string> &allparams) {
    std::string cmd = "JOIN";
    if (allparams.size() > 2)
        return numericReply(client, ERR_NEEDMOREPARAMS, cmd, msg_param);
    std::vector<std::string> channels = seperator(allparams[0], ',');
    std::vector<std::string> keys;
    if (allparams.size() == 2)
        keys = seperator(allparams[1], ',');
    for (size_t i = 0; i < channels.size(); i++) {
        std::string chanName = channels[i];
        if (!isValidChannelName(chanName)) {
            std::string msg = msg_no_such_channel + std::string(" ") + chanName;
            numericReply(client, ERR_NOSUCHCHANNEL, cmd, msg_no_such_channel);
            continue;
        }
        if (channelExists(chanName)) {
            handleJoinExisting(_allChannels[chanName], client, chanName);
        } else {
            handleCreateChannel(client, chanName);
        }
    }
}

void IrcServer::handleJoinExisting(Channel* channel, Client* client, const std::string& channelName) {
    std::string cmd = "JOIN";
    if (channel->isUserInChannel(client))
        return numericReply(client, ERR_USERONCHANNEL, cmd, msg_useronchannel);
    if (channel->getLimit() > 0 && (channel->getMembersCount() + 1) > channel->getLimit())
        return numericReply(client, ERR_CHANNELISFULL, cmd, msg_user_limit);
    if (channel->isUserBanned(client))
        return numericReply(client, ERR_BANNEDFROMCHAN, cmd, msg_banned);
    if (channel->isPrivateChannel() && !channel->isUserInvited(client))
        return numericReply(client, ERR_INVITEONLYCHAN, cmd, msg_invite_only);
    channel->addMember(client);
    std::string fullMessage = ":" + client->genHostMask() + " JOIN :" + channelName + "\r\n";
    channel->broadcast(client, fullMessage, true); // TODO RPL_TOPIC RPL_NAMREPLY
}

bool IrcServer::channelExists(const std::string &channelname) {
    return _allChannels.find(channelname) != _allChannels.end();
}

bool IrcServer::isValidChannelName(const std::string& name) {
    return !name.empty() && (name[0] == '#' || name[0] == '&');
}

