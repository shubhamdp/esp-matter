#!/bin/bash
# Copyright 2022 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# This script generates the launchpad config based on the application binaries
# present in the images/ directory
#
# This requires the merged binary names in a specific format
# <TARGET>_<APP_NAME>.bin
# eg: esp32_light.bin, esp32c3_light.bin

OUT_FILE=launchpad.toml

cat <<EOF >> $OUT_FILE
esp_toml_version = 1.0
firmware_images_url = "https://shubhamdp.github.io/esp-matter/"

EOF

APPS=($(ls | grep esp32 | cut -d'_' -f2- | cut -d'.' -f1 | sort | uniq))

# Build supported applications list
SUPPORTED_APPS="supported_apps = ["
for app in "${APPS[@]}"
do
	SUPPORTED_APPS+="\"$app\","
done
SUPPORTED_APPS+="]"
# Remove the last comma
SUPPORTED_APPS=`echo $SUPPORTED_APPS | sed 's/\(.*\),/\1/'`

echo "$SUPPORTED_APPS" >> $OUT_FILE
echo "" >> $OUT_FILE

# build config for each app
for app in "${APPS[@]}"
do
	echo "[$app]" >> $OUT_FILE

    # Get the supported targets
    TARGETS=($(ls | grep "$app".bin | awk -F "_"$app".bin$" '{print $1}' | grep -v '_'))

    IMAGES=()

    CHIPSETS="chipsets = ["
    for target in "${TARGETS[@]}"
    do
        tUP=`echo $target |  tr 'a-z' 'A-Z'`
        CHIPSETS+="\"$tUP\","
        image="image."$target" = \""$target"_$app.bin\""
        IMAGES+=("$image")
    done
    CHIPSETS+="]"
    # remove last comma
    CHIPSETS=`echo $CHIPSETS | sed 's/\(.*\),/\1/'`

    echo $CHIPSETS >> $OUT_FILE

    for img in "${IMAGES[@]}"
    do
        echo $img >> $OUT_FILE
    done

    unset CHIPSETS IMAGES

    echo "" >> $OUT_FILE

done
