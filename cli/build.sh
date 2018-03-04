#/bin/bash
protoc -I. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./jp4cli.proto
protoc -I. --cpp_out=. ./jp4cli.proto
python3 -m grpc_tools.protoc -I. --python_out=. --grpc_python_out=. ./jp4cli.proto

g++ -std=c++14 `pkg-config --cflags protobuf grpc` -c -o jp4cli.pb.o jp4cli.pb.cc
g++ -std=c++14 `pkg-config --cflags protobuf grpc` -c -o jp4cli.grpc.pb.o jp4cli.grpc.pb.cc
g++ -std=c++14 `pkg-config --cflags protobuf grpc` -c -o handle_cmd.o handle_cmd.cpp
g++ jp4cli.pb.o jp4cli.grpc.pb.o handle_cmd.o -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc` -lgrpc++_reflection -ldl -o handle_cmd
