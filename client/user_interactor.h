#ifndef USER_INTERACTOR_H_
#define USER_INTERACTOR_H_

#include "client.h"

class UserInteractor {
    private:
        Client& client;

        /**
         * Displays the client menu on the screen. 
         * 
         */
        void display_client_menu() const;

        /**
         * Ask from the user an input for client menu's choice.
         * Repeats until got valid input -
         * 1) More than 0 chars.
         * 
         * @return unsigned int - user's choice.
         */
        unsigned int ask_user_choice() const;

        /**
         * Ask from the user an input for client name.
         * Repeats until got valid input - 
         * 1) Less than 255 chars include null terminator.
         * 2) More than 0 chars.
         * 
         * @return std::string - The client's name
         */
        std::string ask_client_name() const;

        /**
         * Asks for the user an input for text message.
         * 
         * @return std::string - The text message.
         */
        std::string ask_text() const;
        
        /**
         * Asks the user for file_path.
         * If the user has exit, returns empty string
         * 
         * @return std::string - file_path or empty string.
         */
        std::string ask_file_path() const;

        /**
         * Displays a Message obj on the screen, with the relevant headers
         * and fields. 
         * 
         * @param message - The message obj.
         */
        void print_message(const Message& message) const;
        
    public:
        /**
         * Construct a new UserInteractor object from the given Client reference.
         * 
         * @param client - The aggregated client obj that will preform the actions.
         */
        UserInteractor(Client& client);

        /**
         * Starts the main loop - 
         *  While True:
         *      1) Displays the client's menu.
         *      2) Asks a choice from the user.
         *      3) Calling the corresponding method. 
         *         If one of the methods throws an exception,
         *         catches it and displays it on the screen.
         */
        void start_loop();

        /**
         * Ask the user for client name, and send a register request accordingly.
         */
        void register_request();

        /**
         * Send a 'client list' request to the server, and display it on the screen.
         * 
         */
        void get_client_list();

        /**
         * Ask the user for client name, and Send to the server a corresponding
         * 'get public key' request to the server.
         */
        void get_public_key();

        /**
         * Send a 'Request for waiting Messages' to the server,
         * and for each message - parse it, handle it, and finally display it 
         * to the screen.
         */
        void get_messages();

        /**
         * Ask the user for client name and for text,
         * and send a corresponding message to the server.
         */
        void send_message();

        /**
         * Ask the user for client name, and send a corresponding
         * 'Send a request for symmetric key' to the server. 
         */
        void get_symmetric_key();

        /**
         * Ask the user for client name, and send a corresponding
         * 'Send your symmetric key' to the server. 
         */
        void send_symmetric_key();

        /**
         * Ask the user for client name and for local file path,
         * and send to ther server a corresponding 'send file' request,
         * according to the protocol.
         */
        void send_file();
};

#endif // USER_INTERACTOR_H_
