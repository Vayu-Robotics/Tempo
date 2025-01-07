ARG UNREAL_VERSION=5.4.4

FROM ghcr.io/epicgames/unreal-engine:dev-slim-${UNREAL_VERSION} AS base

FROM scratch

ENV UNREAL_ENGINE_PATH='/home/ue4/UnrealEngine'

# Copy all non-Unreal stuff
COPY --from=base /bin /bin
COPY --from=base /boot /boot
COPY --from=base /dev /dev
COPY --from=base /etc /etc
COPY --from=base /home/ue4/.bash_logout /home/ue4/.bash_logout
COPY --from=base /home/ue4/.bashrc /home/ue4/.bashrc
COPY --from=base /home/ue4/.cache /home/ue4/.cache
COPY --from=base /home/ue4/.config /home/ue4/.config
COPY --from=base /home/ue4/.gitconfig /home/ue4/.gitconfig
COPY --from=base /home/ue4/.local /home/ue4/.local
COPY --from=base /home/ue4/.profile /home/ue4/.profile
COPY --from=base /lib /lib
COPY --from=base /lib32 /lib32
COPY --from=base /lib64 /lib64
COPY --from=base /libx32 /libx32
COPY --from=base /media /media
COPY --from=base /mnt /mnt
COPY --from=base /opt /opt
COPY --from=base /proc /proc
COPY --from=base /root /root
COPY --from=base /run /run
COPY --from=base /sbin /sbin
COPY --from=base /srv /srv
COPY --from=base /sys /sys
COPY --from=base /tmp /tmp
COPY --from=base /usr /usr
COPY --from=base /var /var

# Fix permissions of /tmp
RUN chmod 1777 /tmp

# Copy Unreal stuff
COPY --from=base /home/ue4/UnrealEngine/Engine/Binaries /home/ue4/UnrealEngine/Engine/Binaries
COPY --from=base /home/ue4/UnrealEngine/Engine/Build /home/ue4/UnrealEngine/Engine/Build
COPY --from=base /home/ue4/UnrealEngine/Engine/Config /home/ue4/UnrealEngine/Engine/Config
COPY --from=base /home/ue4/UnrealEngine/Engine/Content /home/ue4/UnrealEngine/Engine/Content
COPY --from=base /home/ue4/UnrealEngine/Engine/Extras /home/ue4/UnrealEngine/Engine/Extras
COPY --from=base /home/ue4/UnrealEngine/Engine/Intermediate /home/ue4/UnrealEngine/Engine/Intermediate
COPY --from=base /home/ue4/UnrealEngine/Engine/Platforms /home/ue4/UnrealEngine/Engine/Platforms
COPY --from=base /home/ue4/UnrealEngine/Engine/Programs /home/ue4/UnrealEngine/Engine/Programs
COPY --from=base /home/ue4/UnrealEngine/Engine/Shaders /home/ue4/UnrealEngine/Engine/Shaders
COPY --from=base /home/ue4/UnrealEngine/Engine/Source /home/ue4/UnrealEngine/Engine/Source
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Experimental/AnimToTexture /home/ue4/UnrealEngine/Engine/Plugins/Experimental/AnimToTexture
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Runtime/AR/AppleAR/AppleARKitFaceSupport /home/ue4/UnrealEngine/Engine/Plugins/Runtime/AR/AppleAR/AppleARKitFaceSupport
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Experimental/ChaosVehiclesPlugin /home/ue4/UnrealEngine/Engine/Plugins/Experimental/ChaosVehiclesPlugin
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Runtime/HairStrands /home/ue4/UnrealEngine/Engine/Plugins/Runtime/HairStrands
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Animation/LiveLink /home/ue4/UnrealEngine/Engine/Plugins/Animation/LiveLink
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/AI/MassAI /home/ue4/UnrealEngine/Engine/Plugins/AI/MassAI
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/AI/MassCrowd /home/ue4/UnrealEngine/Engine/Plugins/AI/MassCrowd
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Runtime/MassEntity /home/ue4/UnrealEngine/Engine/Plugins/Runtime/MassEntity
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Runtime/MassGameplay /home/ue4/UnrealEngine/Engine/Plugins/Runtime/MassGameplay
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Runtime/MeshModelingToolset /home/ue4/UnrealEngine/Engine/Plugins/Runtime/MeshModelingToolset
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Animation/RigLogic /home/ue4/UnrealEngine/Engine/Plugins/Animation/RigLogic
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Experimental/StructUtils /home/ue4/UnrealEngine/Engine/Plugins/Experimental/StructUtils
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Runtime/ZoneGraph /home/ue4/UnrealEngine/Engine/Plugins/Runtime/ZoneGraph
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Runtime/ZoneGraphAnnotations /home/ue4/UnrealEngine/Engine/Plugins/Runtime/ZoneGraphAnnotations
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Compression/OodleNetwork /home/ue4/UnrealEngine/Engine/Plugins/Compression/OodleNetwork
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Online/OnlineSubsystem /home/ue4/UnrealEngine/Engine/Plugins/Online/OnlineSubsystem
COPY --from=base /home/ue4/UnrealEngine/Engine/PluginsOnline/OnlineSubsystemUtils /home/ue4/UnrealEngine/Engine/PluginsOnline/OnlineSubsystemUtils
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Runtime/LinuxDeviceProfileSelector /home/ue4/UnrealEngine/Engine/Plugins/Runtime/LinuxDeviceProfileSelector
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/EnhancedInput /home/ue4/UnrealEngine/Engine/Plugins/EnhancedInput
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/PCG /home/ue4/UnrealEngine/Engine/Plugins/PCG
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Runtime/OpenXR /home/ue4/UnrealEngine/Engine/Plugins/Runtime/OpenXR
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Runtime/OpenXRHandTracking /home/ue4/UnrealEngine/Engine/Plugins/Runtime/OpenXRHandTracking
COPY --from=base /home/ue4/UnrealEngine/Engine/Plugins/Runtime/RigVM /home/ue4/UnrealEngine/Engine/Plugins/Runtime/RigVM

# Install dependencies
RUN sudo apt-get update && sudo apt-get install -y jq rsync

ENTRYPOINT ["/bin/bash"]
CMD ["-l"]

USER ue4
WORKDIR /home/ue4
