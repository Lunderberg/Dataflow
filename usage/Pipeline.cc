#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "MakeUnique.hh"
#include "Source.hh"
#include "Sink.hh"
#include "Transform.hh"

std::unique_ptr<double> produce_count(){
  static double value = 0;
  return make_unique<double>(value++);
}

std::unique_ptr<double> square_values(const std::shared_ptr<double> value){
  double x = *value;
  return make_unique<double>(x*x);
}

void print_values(const std::shared_ptr<double> value){
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << *value << std::endl;
}

int main(){
  auto step1 = make_source(produce_count);
  auto step2 = make_transform(square_values, step1->GetProducer());
  auto step3 = make_sink(print_values, step2->GetProducer());

  std::cout << "Started" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "Done sleeping" << std::endl;
}
