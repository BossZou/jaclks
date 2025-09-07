#include "jaclks-internal/cxx-detail/utils/string_utils.h"

namespace jaclks::cxx_detail {

namespace {
using std::string;

/**
 * Steal to access private _M_string_length member
 */
using size_type = string::size_type;

size_type *StealLength(string *str);

template <size_type string::*MEMBER_INT_PTR>
struct LengthMemberAccesser {
  friend size_type *StealLength(string *victim_object) {
    return &(victim_object->*MEMBER_INT_PTR);
  }
};

template struct LengthMemberAccesser<&string::_M_string_length>;

/**
 * Steal local capacity
 */
int StealLocalCapacity(const string &str);

template <int Cap>
struct LocalCapacityMemberAccesser {
  friend int StealLocalCapacity(const string &victim_object) {
    return Cap;
  }
};

template struct LocalCapacityMemberAccesser<string::_S_local_capacity>;

/**
 * Steal to access private _M_is_local function
 */
bool IsLocalData(const string &str);

template <bool (string::*Func)() const>
struct LocalFunctionAccesser {
  friend bool IsLocalData(const string &victim_object) {
    return (victim_object.*Func)();
    ;
  }
};

template struct LocalFunctionAccesser<&string::_M_is_local>;

}  // namespace

std::string StringUtils::ToStdString(javac_base::String str) {
  std::string result;


  return "";
}

}  // namespace jaclks::cxx_detail
