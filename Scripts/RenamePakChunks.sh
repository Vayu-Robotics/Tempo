#!/usr/bin/env bash

set -e

# Extract unique level names from a chunk manifest file
extract_level_names() {
    local input_file="$1"
    local additional_dirs=("${@:2}")
    
    while IFS= read -r line; do
        if [[ $line == *"\_Generated_"* ]]; then
            if [[ $line == *"Engine\\Content"* ]]; then
              # Ignore engine content
              continue
            fi
            # Get everything before \_Generated_
            path=${line%\\_Generated_*}
            # Get everything after the last \Maps\ 
            map_section=${path##*\\Maps\\}
            
            # If no match found in Maps, check additional directories
            if [[ "$map_section" == "$path" ]]; then
                # Check additional directories if provided
                for dir in "${additional_dirs[@]}"; do
                    if [[ "$map_section" == "$path" ]]; then
                        map_section=${path##*\\${dir}\\}
                    fi
                done
            fi
            # Print the result
            echo "$map_section"
        fi
    done < "$input_file" | sort -u
}

# Check arguments
if [ $# -lt 2 ]; then
  echo "Usage: $0 <Package directory> <Package Metadata directory> [Additional Map Directories...]"
  exit 1
fi

if [ ! -d "$1" ]; then
  echo "Package directory $1 does not exist"
  exit 1
fi

if [ ! -d "$2" ]; then
  echo "Package Metadata directory $2 does not exist"
  exit 1
fi

PACKAGE_PATH=$(realpath "$1")
METADATA_PATH=$(realpath "$2")
ADDITIONAL_DIRS=("${@:3}")

if [ ! -d "$METADATA_PATH/ChunkManifest" ]; then
  echo "ChunkManifest directory not found. Are chunks enabled in the project?"
  exit 1
fi

# PAK_PATH will be in different locations depending on several factors including platform.
# So just find it. It will be the only directory with pak files. This will quit after the first one it finds.
PAK_PATH=$(find "$PACKAGE_PATH" -name "*.pak" -exec dirname {} \; -quit)

if [ ! -d "$PAK_PATH" ]; then
  echo "Could not find pak path"
  exit 1
fi

# We're interested pak chunk manifest files, which look like pak12345.txt
MANIFEST_FILES=$(find "$METADATA_PATH/ChunkManifest" -regex ".*pakchunk[0-9]*.txt")
for MANIFEST_FILE in $MANIFEST_FILES; do
  LEVELS=$(extract_level_names "$MANIFEST_FILE" "${ADDITIONAL_DIRS[@]}" | tr ' ' -)
  FILENAME=$(basename "$MANIFEST_FILE")
  PAK_NAME="${FILENAME%.*}"
  CHUNK_ID="${PAK_NAME#*pakchunk}"

  if [ $((CHUNK_ID)) -gt 1000 ]; then
    if [[ "$LEVELS" == '' ]]; then
      echo "Pak chunk $CHUNK_ID has ID > 1000 but no levels"
      continue
    fi
    # Rename "Level" chunks by their level names
    find "$PAK_PATH" -name "*pakchunk$CHUNK_ID*" -exec bash -c 'mv "$1" "${1/pakchunk$2/pakchunk-$3}"' _ {} "$CHUNK_ID" "$LEVELS" \;
  fi
done
