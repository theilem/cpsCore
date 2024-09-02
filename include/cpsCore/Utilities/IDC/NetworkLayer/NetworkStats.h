//
// Created by Mirco Theile on 2/9/24.
//

#ifndef IDCSTATS_H
#define IDCSTATS_H

#include <cpsCore/Utilities/NamedValue.hpp>

struct NetworkStats
{
    NamedValue<int> packetsReceived = {0, "packets_received"};
    NamedValue<int> packetsCorrupted = {0, "packets_corrupted"};

    NamedValue<std::size_t> dataReceived = {0, "data_received"};
    NamedValue<std::size_t> dataCorrupted = {0, "data_corrupted"};

    NamedValue<int> packetsSent = {0, "packets_sent"};
    NamedValue<std::size_t> dataSent = {0, "data_sent"};

    template <typename Parser>
    void
    parse(Parser& p) const
    {
        p & packetsReceived;
        p & packetsCorrupted;
        p & dataReceived;
        p & dataCorrupted;
        p & packetsSent;
        p & dataSent;
    }
};

#endif //IDCSTATS_H
