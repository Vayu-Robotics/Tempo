ARG UNREAL_VERSION=5.4.4

FROM ghcr.io/epicgames/unreal-engine:dev-slim-${UNREAL_VERSION}

ENV UNREAL_ENGINE_PATH='/home/ue4/UnrealEngine'

# Install dependencies
RUN sudo apt-get update && sudo apt-get install -y jq rsync

# Free up space
RUN rm /home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealServer-Linux-Shipping
RUN rm /home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealClient-Linux-Shipping
RUN rm /home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealGame-Linux-Shipping
RUN rm /home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealServer
RUN rm /home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealServer-Linux-DebugGame
RUN rm /home/ue4/UnrealEngine/Engine/Binaries/Linux/LiveLinkHub-Linux-Shipping
RUN rm /home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealClient
RUN rm /home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealClient-Linux-DebugGame
RUN rm /home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealGame
RUN rm /home/ue4/UnrealEngine/Engine/Binaries/Linux/UnrealGame-Linux-DebugGame
RUN rm -rf /home/ue4/UnrealEngine/Engine/Plugins/Experimental/MoverTests
RUN rm -rf /home/ue4/UnrealEngine/Engine/Plugins/Experimental/MoverExamples
RUN rm -rf /home/ue4/UnrealEngine/Engine/Plugins/Experimental/Avalanche
RUN rm -rf /home/ue4/UnrealEngine/Engine/Plugins/Experimental/TextToSpeech
RUN rm -rf /home/ue4/UnrealEngine/Engine/Plugins/Experimental/NNERuntimeIREE
