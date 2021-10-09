#ifndef USER_INTERACTOR_H_
#define USER_INTERACTOR_H_

#include "client.h"

class UserInteractor {
    private:
        Client& client;
        void display_client_menu() const;
        unsigned int ask_user_choice() const;
        std::string ask_client_name() const;
        std::string ask_text() const;
        void print_message(const Message& message) const;
        
    public:
        UserInteractor(Client& client);

        void start_loop();
        void register_request();
        void get_client_list();
        void get_public_key();
        void get_messages();
        void send_message();
        void get_symmetric_key();
        void send_symmetric_key();
};

#endif // USER_INTERACTOR_H_
