#include <iostream>
#include <string>
#include <exception>

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

void UserInteractor::display_client_menu() {
    std::cout << "\n---------------------------------------" << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "MessageU client at your service.\n" << std::endl;
    std::cout << REGISTER << ") Register" << std::endl;
    std::cout << GET_CLIENT_LIST << ") Request for clients list" << std::endl;    
    std::cout << GET_PUBLIC_KEY << ") Request for public key" << std::endl;
    std::cout << GET_MESSAGES << ") Request for waiting messages" << std::endl;
    std::cout << SEND_MESSAGE << ") Send a text message" << std::endl;
    std::cout << GET_SYMMETRIC_KEY << ") Send a request for symmetric key" << std::endl;
    std::cout << SEND_SYMMETRIC_KEY << ") Send your symmetric key" << std::endl;
    std::cout << EXIT_CLIENT << ") Exit client\n" << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "---------------------------------------\n" << std::endl;
}

unsigned int UserInteractor::ask_user() {
    std::string user_choice_str;
    unsigned int user_choice = 0;
    bool success = false;
    do {
        std::cout << "Enter choice: ";
        std::cin >> user_choice_str;
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

void UserInteractor::register_request() {
    client.register_client(std::string("dean levinson"));
}

void UserInteractor::get_client_list() {
    // client.get_client_list();
}

void UserInteractor::get_public_key() {
    // client.get_public_key();
}

void UserInteractor::get_messages() {
    // client.get_waiting_messages();
}

void UserInteractor::send_message() {
    // client.send_message(target);
}

void UserInteractor::get_symmetric_key() {
    // client.get_symmetric_key();
}

void UserInteractor::send_symmetric_key() {
    // client.send_symmetric_key();
}

void UserInteractor::start_loop() {
    bool should_exit = false;
    while (!should_exit) {
        display_client_menu();
        unsigned int user_choice = ask_user();

        switch (user_choice) {
            case REGISTER:
                std::cout << "[!] Got register request" << std::endl; 
                register_request(); 
                break;

            case GET_CLIENT_LIST:
                std::cout << "[!] Got client's list request" << std::endl;
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
    }
}