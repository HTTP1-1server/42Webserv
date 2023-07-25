#pragma once

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include "ResponseMessage.hpp"

class InputView
{
private:
public:
    InputView() {};
    virtual ~InputView() {};

    virtual void sendResponseMessage(int connectSd, ResponseMessage &responseMessage) const = 0;
};

class ConsoleInputView: public virtual InputView
{
private:
public:
    ConsoleInputView() {};
    virtual ~ConsoleInputView() {};

    void sendResponseMessage(int connectSd, ResponseMessage &responseMessage) const {
        send(connectSd, responseMessage.message.c_str(), responseMessage.message.length(), MSG_DONTWAIT);
        close(connectSd);
    };
};