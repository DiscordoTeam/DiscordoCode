//
// Created by pietb on 18.07.2022.
//

#include "User.h"

User user;

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