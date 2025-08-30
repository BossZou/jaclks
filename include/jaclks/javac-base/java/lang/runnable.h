#pragma once

namespace jaclks::javac_base {

class Runnable {
 public:
  virtual ~Runnable() = default;

  virtual void Run() = 0;
};

}  // namespace jaclks::javac_base
