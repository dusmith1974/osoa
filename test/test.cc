// Copyright 2013 Duncan Smith

#include <iostream>

#include <service/service.h>

int main(int argc, const char *argv[]) {
  osoa::Service service;
  service.Initialize(argc, argv);

  if (0 == service.Start())
    service.Stop();
}

