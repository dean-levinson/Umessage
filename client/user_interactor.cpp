#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>

#include "exceptions.h"
#include "user_interactor.h"


typedef enum user_input_e {
    REGISTER=10,
    GET_CLIENT_LIST=20,
    GET_PUBLIC_KEY=30,
    GET_MESSAGES=40,
    SEND_MESSAGE= 50,
    GET_SYMMETRIC_KEY = 51,
    SEND_SYMMETRIC_KEY = 52,
    EXIT_CLIENT = 0
} user_input;

UserInteractor::UserInteractor(Client& client): client(client) {
    std::cout << "Connecting client..." << std::endl;
    client.connect();
}

void UserInteractor::display_client_menu() const {
    std::cout << "\n-----------------------------------------" << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    std::string client_name(client.get_client_name());
    if (client_name.length()) {
        std::cout << "-- " << "Hello " << client_name << "!" << std::endl;
    }
    std::cout << "-- " << "MessageU client at your service." << std::endl;
    std::cout << "-- " << std::endl;
    std::cout << "-- " << REGISTER << ") Register" << std::endl;
    std::cout << "-- " << GET_CLIENT_LIST << ") Request for clients list" << std::endl;    
    std::cout << "-- " << GET_PUBLIC_KEY << ") Request for public key" << std::endl;
    std::cout << "-- " << GET_MESSAGES << ") Request for waiting messages" << std::endl;
    std::cout << "-- " << SEND_MESSAGE << ") Send a text message" << std::endl;
    std::cout << "-- " << GET_SYMMETRIC_KEY << ") Send a request for symmetric key" << std::endl;
    std::cout << "-- " << SEND_SYMMETRIC_KEY << ") Send your symmetric key" << std::endl;
    std::cout << "-- " << EXIT_CLIENT << ") Exit client" << std::endl;
    std::cout << "-- " << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "-----------------------------------------\n" << std::endl;
}

unsigned int UserInteractor::ask_user_choice() const {
    std::string user_choice_str;
    unsigned int user_choice = 0;
    bool success = false;
    do {
        std::cout << "Enter choice: ";
        std::getline(std::cin, user_choice_str);

        try {
            user_choice = std::stoi(user_choice_str);

            if (user_choice_str.length() == std::to_string(user_choice).length()) {
                success = true;
            } else {
                throw std::invalid_argument("Got unexpected input");
            }

        } catch (std::invalid_argument) {
            std::cout << "Invalid input, try again..." << std::endl; 
        };

    } while(!success);

    return user_choice;
}

std::string UserInteractor::ask_client_name() const {
    bool client_name_valid = false;
    std::string client_name;

    while (!client_name_valid) {
        std::cout << "Enter client name: ";
        std::getline(std::cin, client_name);

        // 255 include null terminator
        if (client_name.size() > 254) {
            std::cout << "Invalid length - client name is limited to 255 chars" << std::endl;
        } else if (client_name.size() == 0) {
            std::cout << "Invalid input - client name cannot be empty" << std::endl;
        } else {
            client_name_valid = true;
        }
    }

    return client_name;
}

std::string UserInteractor::ask_text() const {
    bool valid_text = false;
    std::string text;

    while (!valid_text) {
        std::cout << "Enter text: ";
        std::getline(std::cin, text);

        if (text.size() < 1) {
            std::cout << "Invalid length - text must be bigger than 0 bytes" << std::endl;
        } else {
            valid_text = true;
        }
    }

    return text;
}

void UserInteractor::register_request() {
    std::string client_name = ask_client_name();
    client.register_client(client_name);
    std::cout << "Registered successfully!" << std::endl;
}

void UserInteractor::get_client_list() {
    list<User> client_list = client.get_client_list();
    std::cout << "Client list:" << std::endl;
    for (const User& user: client_list) {
        std::cout << "[*] " << user.get_client_name() << std::endl;
    }
}

void UserInteractor::get_public_key() {
    string client_name = ask_client_name();
    string client_id = client.get_client_id_by_name(client_name);
    string public_key = client.get_public_key(client_id);
    std::cout << "Got public key!" << std::endl;
}

void UserInteractor::get_messages() {
    // The protocol doesn't support pulling one message, so if the client will throw an Exception,
    // UserInteractor won't be able to see the rest of the messages. Therefor, Client Catches the excpetions and
    // print DEBUG error to stdout.
    list<Message> messages(client.pull_messages());
    for (const Message& message: messages) {
        print_message(message);
    }
}

void UserInteractor::print_message(const Message& message) const {
    std::string other_name;
    try {
        other_name = (client.get_user_by_client_id(message.from_client_id).get_client_name());
    } catch (const NoSuchUser& e) {
        other_name = std::string("UNKNOWN");
    }

    std::cout << "Message from: " << other_name << std::endl;  
    std::cout << "Message type: " << static_cast<int>(message.message_type) << std::endl;
    std::cout << "Message content: " << std::endl;

    switch (message.message_type) {
        case (1):
            std::cout << "Request for symmetric key. Creating and sending symmetric key..." << std::endl;
            break;
        case (2):
            std::cout << "Symmetric key received. Accepting..." << std::endl;
            break;
        case (3):
            std::cout << message.content << std::endl;
            break;
        case (4):
            std::cout << "Got file in path: " << std::endl; // todo - complete
            break;
    }
    std::cout << "------<EOM>------" << std::endl;
}

void UserInteractor::send_message() {
    std::string target_client_name = ask_client_name();
    std::string text = ask_text();
    client.send_text_message(target_client_name, text);
    std::cout << "Sent text message successfully!" << std::endl;
}

void UserInteractor::get_symmetric_key() {
    client.get_symmetric_key(ask_client_name());
    std::cout << "Sent symmetric key request successfully!" << std::endl;
}

void UserInteractor::send_symmetric_key() {
    client.send_symmetric_key(ask_client_name());
    std::cout << "Sent symmetric key successfully!" << std::endl;
}

void UserInteractor::start_loop() {
    bool should_exit = false;
    while (!should_exit) {
        display_client_menu();
        unsigned int user_choice = ask_user_choice();
        try {
            switch (user_choice) {
                case REGISTER:
                    std::cout << "[!] Got register request" << std::endl; 
                    register_request(); 
                    break;

                case GET_CLIENT_LIST:
                    std::cout << "[!] Got client list request" << std::endl;
                    get_client_list();
                    break;

                case GET_PUBLIC_KEY:
                    std::cout << "[!] got public key request" << std::endl;
                    get_public_key();
                    break;

                case GET_MESSAGES:
                    std::cout << "[!] got waiting messages request" << std::endl;
                    get_messages();
                    break;

                case SEND_MESSAGE:
                    std::cout << "[!] got sending message request" << std::endl;
                    send_message();
                    break;
                
                case GET_SYMMETRIC_KEY:
                    std::cout << "[!] got symmetric key request" << std::endl;
                    get_symmetric_key();
                    break;

                case SEND_SYMMETRIC_KEY:
                    std::cout << "[!] got sending symmetric key request" << std::endl;
                    send_symmetric_key();
                    break;

                case EXIT_CLIENT :
                    std::cout << "[!] got exit request" << std::endl; 
                    should_exit = true;
                    std::cout << "Thanks for using Umessage! Goodbye :)" << std::endl;
                    break;

                default:
                    std::cout << "ERROR - No such code!, try again...\n" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Got error: " << e.what() << std::endl << "aborting..." << std::endl;
        }
    }
}