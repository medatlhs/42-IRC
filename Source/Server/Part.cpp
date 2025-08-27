# include "../../Includes/replyCodes.hpp"
# include "../../Includes/IrcServer.hpp"
# include "../../Includes/Channel.hpp"
# include "../../Includes/Client.hpp"

void    IrcServer::handlePart(Client *client, std::vector<std::string> &allparams) {
    std::string cmd = "PART";
    bool leftAmsj = (allparams.size() == 2);
    if ((leftAmsj && allparams[1][0] != ':')|| allparams.size() < 1)
        return numericReply(client, ERR_NEEDMOREPARAMS, cmd, msg_param);
    std::vector<std::string> channels = seperator(allparams[0], ',');
    for (size_t i = 0; i < channels.size(); i++) {
        std::string channelname = channels[i];
        Channel *channel = getChannel(channelname.erase(0, 1));
        if (!channel) {
            std::string msg = msg_no_such_channel + std::string(" ") + channels[i];
            numericReply(client, ERR_NOSUCHCHANNEL, cmd, msg);
            continue;
        } 
        
        if (!client->isChannelMember(channel)) {
            std::string msg = msg_not_member + std::string(" #") + channelname;
            numericReply(client, ERR_NOSUCHCHANNEL, cmd, msg);
            continue;
        }

        // :<nick>!<user>@<host> PART #channel [:part message]
        std::string fullmessage = ":"+client->genHostMask()+" PART "+channelname;
        if (leftAmsj) { fullmessage.append(" ").append(allparams[1]); }
        channel->broadcast(client, fullmessage, false);
        channel->removeMember(client);
        if (channel->getMembersCount() == 0) {
            _allChannels.erase(channelname);
            delete channel;
        }
    }
}