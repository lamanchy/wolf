

#include "pipeline_status.h"
namespace wolf {
bool pipeline_status::initialized{false};
bool pipeline_status::started{false};
std::atomic<bool> pipeline_status::running{false};
sleeper pipeline_status::pipeline_sleeper{};
bool pipeline_status::persistent{};
unsigned pipeline_status::buffer_size{};
}