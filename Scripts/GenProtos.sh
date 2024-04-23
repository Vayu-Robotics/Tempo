#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
TEMPO_ROOT=$(realpath "$SCRIPT_DIR/..")
PROTOC="$TEMPO_ROOT/Plugins/TempoCore/Source/ThirdParty/gRPC/Binaries/Mac/protoc"
GRPC_CPP_PLUGIN="$TEMPO_ROOT/Plugins/TempoCore/Source/ThirdParty/gRPC/Binaries/Mac/grpc_cpp_plugin"
GRPC_PYTHON_PLUGIN="$TEMPO_ROOT/Plugins/TempoCore/Source/ThirdParty/gRPC/Binaries/Mac/grpc_python_plugin"
SOURCE_DIR="$TEMPO_ROOT/Plugins/TempoCore/Source/TempoTime/Services"
CPP_DEST_DIR="$TEMPO_ROOT/Plugins/TempoCore/Source/TempoTime/Private"
PYTHON_DEST_DIR="$TEMPO_ROOT/Content/Python"

for STALE in "$CPP_DEST_DIR"/*.pb.*; do
      rm -rf "$STALE"
done

for STALE in "$CPP_DEST_DIR"/*.grpc.*; do
      rm -rf "$STALE"
done

for STALE in "$PYTHON_DEST_DIR"/*_pb2.py; do
      rm -rf "$STALE"
done

for STALE in "$PYTHON_DEST_DIR"/*_pb2_grpc.py; do
      rm -rf "$STALE"
done

for SOURCE in "$SOURCE_DIR"/*.proto; do
      eval "$PROTOC" --cpp_out="$CPP_DEST_DIR" --grpc_out="$CPP_DEST_DIR" --plugin=protoc-gen-grpc="$GRPC_CPP_PLUGIN" "$SOURCE" -I "$SOURCE_DIR"
      eval "$PROTOC" --python_out="$PYTHON_DEST_DIR" --grpc_out="$PYTHON_DEST_DIR" --plugin=protoc-gen-grpc="$GRPC_PYTHON_PLUGIN" "$SOURCE" -I "$SOURCE_DIR"
done
