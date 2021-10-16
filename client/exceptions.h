#include <string>
#include <exception>

class ServerError : public std::exception {  
    private:
    std::string err;

    public:  
        ServerError();    
        ServerError(std::string err);
        ServerError(const char* err);
        const char * what() const throw(); 
};  

class NoSuchUser : public std::exception {  
    private:
        std::string internal_message;
    public:  
        NoSuchUser(const char * err_message);
        NoSuchUser(std::string err_message);
        const char * what() const throw(); 
};  

class NoPublicKey : public std::exception {  
    private:
        std::string err;
        std::string client_name;
    public:  
        NoPublicKey(const std::string& client_id);
        const char * what() const throw(); 
};  

class NoSymmeticKey : public std::exception {  
    private:
        std::string err;
        std::string client_name;
    public:  
        NoSymmeticKey(const std::string& client_id);
        const char * what() const throw(); 
};

class AlreadyRegistered : public std::exception {  
    public:  
        const char * what() const throw(); 
};
