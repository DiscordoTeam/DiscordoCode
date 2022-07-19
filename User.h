//
// Created by pietb on 18.07.2022.
//

#include <iostream>
#include <vector>
#include <array>
#include "json.hpp"

#pragma once

struct User {

   std::string name;
   std::string bio;
   std::string password;
   std::string mail;

   uint16_t idFrontEnd;
   uint64_t idBackEnd;

   uint32_t phone;

   bool online;
   bool afk;
   bool dnd;
   bool invisible;

   std::vector<uint32_t> servers;
   std::vector<uint32_t> friends;
   std::vector<uint32_t> groups;

   void udelete();
   void unoActivity();
   void udnd();
   void uinvisible();
   void udisconnect();
   void uinitialization(std::string name, std::string email, std::string password);

    enum a {

        PrivacyPower, WritingPower, SpeakingPower
    };

    uint8_t powerLevelList[3];
};