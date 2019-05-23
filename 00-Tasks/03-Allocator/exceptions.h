//
// Created by svl on 5/23/19.
//
#include "exception"

#pragma once

struct NotEnoughMemory : public std::exception {
    const char* what() const noexcept override {
        return "Not Enough Memory!!";
    }
};
