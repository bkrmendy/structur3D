//
// Created by Berci on 2020. 12. 02..
//

#ifndef STRUCTUR3D_BASE_SERVERCONTROLLER_H
#define STRUCTUR3D_BASE_SERVERCONTROLLER_H

#include <memory>
#include <db/Database.h>

#include "Server.h"

namespace S3D {
    struct ServerController {
        std::unique_ptr<Database> db_;
        std::unique_ptr<Server> server_;

        ServerController(
                std::unique_ptr<Database> db,
                std::unique_ptr<Server> server)
            : db_{std::move(db)}
            , server_{std::move(server)}
            {}

        void sync(ID& document, Timestamp& timestamp, std::vector<Protocol::Message>& messages) const;
        void receive(Protocol::Message& message) const;
        void broadcast(Protocol::Message& message) const;
    };
}

#endif //STRUCTUR3D_BASE_SERVERCONTROLLER_H
