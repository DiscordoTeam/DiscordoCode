//
// Created by pietb on 18.07.2022.
//

#include <fstream>
#include "User.h"

User user;

void User::uinitialization(std::string name, std::string email, std::string password) {

    std::ofstream createFileStream("maxId");
    if (!createFileStream.is_open()) {
        createFileStream << "0" << std::endl;
        createFileStream.flush();
        createFileStream.close();
    }
    
    uint64_t idInt = 0;

    std::fstream inputStream("maxId");
    inputStream >> idInt;
    idBackEnd = idInt + 1;
    inputStream.close();

    std::ofstream outStream("maxId");
    outStream << idBackEnd << std::endl;
    outStream.flush();
    outStream.close();

    std::ofstream of(std::to_string(idBackEnd));
    nlohmann::json json;
    json["username"] = name;
    json["email"] = email;
    json["password"] = password;
    of << std::setw(4) << json << std::endl;
    of.flush();
    of.close();
}

void User::udelete() {


}

void User::unoActivity() {

    user.online = true;
    user.afk = true;
    user.dnd = false;
    user.invisible = false;
};

void User::udnd() {

    user.online = true;
    user.afk = false;
    user.dnd = true;
    user.invisible = false;
}

void User::uinvisible() {

    user.online = true;
    user.afk = false;
    user.dnd = false;
    user.invisible = true;
}

void User::udisconnect() {

    user.online = false;
    user.afk = false;
    user.dnd = false;
    user.invisible = true;
}