

#include "compressed.h"
#include <iso646.h>
#include <extras/gzip.h>

namespace wolf {
namespace to {
void compressed::process(json &&next) {
  std::string s = next.get_string();
  std::string result = gzip::compress(s.data(), s.size(), Z_BEST_SPEED);

  result = extras::gzip::get_prefix() + extras::gzip::get_serialized_size(result.size()) + result;

  output(json(result).copy_all(next));
}
}
}