#include "jaclks-internal/cxx-detail/utils/string_utils.h"

namespace jaclks::cxx_detail {

namespace {
using std::string;

std::size_t *StealLength(const string &str);

template <const int string::*MEMBER_INT_PTR>
struct GenerateThiefFunction {
  // static_assert(std::is_same_v<MEMBER_INT_PTR, &string::_M_string_length>);

  friend std::size_t *StealLength(const string &victim_object) {
    // dereferencing member pointer on instance - might look exotic but no magic
    // here std::cout << victim_object.*MEMBER_INT_PTR << std::endl;
    return &(victim_object.*MEMBER_INT_PTR);
  }
};

template struct GenerateThiefFunction<&string::_M_string_length>;

}  // namespace

std::string StringUtils::ToStdString(javac_base::String str) {
  return "";
}

}  // namespace jaclks::cxx_detail
