#pragma once

#include <string>
#include <chrono>
namespace wolf {
namespace extras {

std::string get_date(std::chrono::seconds timedelta = std::chrono::hours(0));

}
}


